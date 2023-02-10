package com.android.xrappos.eglsession;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.util.SparseArray;
 class GLThread {
    private HandlerThread mRenderThread;
    private Handler mRenderHandler;

    private SparseArray<Handler.Callback> mRenderMessageHandler;

    public void registerRenderMessageHandler(int what, Handler.Callback handler) {
        if (mRenderMessageHandler == null) {
            mRenderMessageHandler = new SparseArray<>();
        }
        mRenderMessageHandler.put(what, handler);
    }

    public void sendGLTask(Runnable glTask) {
        sendGLTask(glTask, 0);
    }

    public void sendGLTask(Runnable glTask, long delayTime) {
        if (mRenderHandler == null
                || !mRenderThread.isAlive()
                || glTask == null) {
            return;
        }

        mRenderHandler.postDelayed(glTask, delayTime);
    }

    /**
     * 当前Handler是否有效
     *
     * @return
     */
    private boolean isHandlerAvailable() {
        return isRenderThreadAvailable()
                && mRenderHandler != null
                && mRenderHandler.getLooper() != mRenderThread.getLooper();
    }

    private boolean isRenderThreadAvailable() {
        return mRenderThread != null
                && mRenderThread.isAlive();
    }

    void startRenderThreadIfNeed() {
        if (isRenderThreadAvailable()) {
            return;
        }
        mRenderThread = new HandlerThread("GL Renderer Thread");
        mRenderThread.start();

        if (mRenderHandler != null
                && mRenderHandler.getLooper() == mRenderThread.getLooper()) {
            return;
        }

        if (mRenderHandler != null) {
            // 防止内存泄漏
            mRenderHandler.removeCallbacksAndMessages(null);
        }
        mRenderHandler = new RenderHandler(mRenderThread.getLooper());
    }

    /**
     * 发送渲染消息
     *
     * @param what
     * @return 是否成功发出消息
     */
    boolean sendRenderMessage(int what) {
        if (!isHandlerAvailable()) {
            return false;
        }

        mRenderHandler.sendEmptyMessage(what);

        return true;
    }

    /**
     * 发送渲染消息
     *
     * @param what
     * @return 是否成功发出消息
     */
    boolean sendRenderMessage(int what, int arg1, int arg2) {
        if (!isHandlerAvailable()) {
            return false;
        }

        Message message = mRenderHandler.obtainMessage(what, arg1, arg2);
        message.sendToTarget();

        return true;
    }

    void destroyRenderThread() {
        if (mRenderThread != null) {
            mRenderThread.quit();
            mRenderThread = null;
        }
        if (mRenderHandler != null) {
            mRenderHandler.removeCallbacksAndMessages(null);
            mRenderHandler = null;
        }
    }

    /***Handler内部类, 路由渲染事件**************************/
    private class RenderHandler extends Handler {

        private RenderHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            if (mRenderMessageHandler == null
                    || mRenderMessageHandler.size() <= 0
                    || mRenderMessageHandler.indexOfKey(msg.what) < 0) {
                return;
            }

            Handler.Callback callback = mRenderMessageHandler.get(msg.what);
            // 剔除外部消息flag
            if (callback != null) {
                callback.handleMessage(msg);
            }
        }
    }
}
