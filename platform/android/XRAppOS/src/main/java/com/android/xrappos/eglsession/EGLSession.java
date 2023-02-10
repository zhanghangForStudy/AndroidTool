package com.android.xrappos.eglsession;

import android.graphics.SurfaceTexture;
import android.opengl.EGL14;
import android.opengl.GLES20;
import android.os.HandlerThread;
import android.os.Message;
import android.view.Surface;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL;
import javax.microedition.khronos.opengles.GL10;

/**
 * 该类的核心功能为通过Java API构建EGL环境，同时，开辟一条新的线程来处理OPEN GL渲染任务；
 * <br>
 * 其目的是将TextureView/SurfaceView与open gl环境的搭建与管理进行解耦;
 * <ul>
 * <li>
 * 该类与{@link android.opengl.GLSurfaceView}中的渲染流程相比，依然有以下两点较为明显的区别:
 * <ul>
 * <li>处理opengl es的线程是基于{@link HandlerThread}</li>
 * <li>视图的生命周期与{@link android.opengl.GLSurfaceView}有一些区别，具体可见下面的生命周期说明</li>
 * <ul/>
 * </li>
 * <li>
 * 整个渲染线程与视图的生命周期基本一致，以此来保证渲染线程中的GL环境不会被污染;
 * 该类使用{@link IEGLRender}接口，作为外部接口，进行个性化的OpenGL ES相关的渲染;
 * 具体而言，视图的生命周期可以分为以下8个步骤:
 * <ol>
 * <li>视图创建；该阶段会创建渲染线程，并且会初始化EGL Display及EGL Context对象</li>
 * <li>
 * 当视图的SurfaceTexture可用的时候，会根据该SurfaceTexture创建EGL Surface对象，
 * 并将渲染线程的EGL环境绑定成已创建的EGL Display对象、EGL Context对象、EGL Surface对象.
 * </li>
 * <li>回调{@link #mRender}对象的onSurfaceCreated方法</li>
 * <li>如果尺寸发生变化，回调{@link #mRender}对象的onSurfaceChanged方法</li>
 * <li>渲染当前帧；该步骤是一个循环操作；</li>
 * <li>当视图的SurfaceTexture不可用时，销毁EGL Surface对象；</li>
 * <li>回调{@link #mRender}对象的onSurfaceDestroy方法</li>
 * <li>当整个视图不可用时，销毁EGL Display对象和EGL Context对象；该步骤需要上层主动调用{@link #destroy()}方法</li>
 * <ol/>
 * </li>
 * <li>
 * 其他：
 * <ul>
 * <li>一般而言，当视图不可见或者只是从视图树上剥离（有重新被添加到视图树的可能）那么只会重新执行2~7步;</li>
 * <li>此外，需要在视图对应的Activity或者Fragment的onDestroy方法中调用{@link #destroy()}方法</li>
 * </ul>
 * </li>
 * </ul>
 */
public class EGLSession {
    private static final int MSG_INIT_EGL_CONTEXT = 1;
    private static final int MSG_INIT_EGL_SURFACE = 2;
    private static final int MSG_CHANGE_SIZE = 3;
    private static final int MSG_RENDER = 4;
    private static final int MSG_DESTROY_SURFACE = 5;
    private static final int MSG_DESTROY_CONTEXT = 6;

    int mCurrentSurfaceWidth;

    int mCurrentSurfaceHeight;

    boolean mIsInit = false;

    INativeWindowProvider mNativeWindowProvider;

    private EGLConfig[] mEGLConfig = new EGLConfig[1];
    private EGL10 mEgl = null;
    private EGLDisplay mEGLDisplay = EGL10.EGL_NO_DISPLAY;
    private EGLContext mEGLContext = EGL10.EGL_NO_CONTEXT;
    private EGLSurface mEglSurface;
    private EGLContext mParentEGLContext;

    private IEGLRender mRender;

    private GL mGL;

    private Object nativeWindow = null;

    private final GLThread mGLThread = new GLThread();

    public EGLSession(IEGLRender renderer) {
        mRender = renderer;
        mGLThread.registerRenderMessageHandler(MSG_INIT_EGL_CONTEXT, this::initEGLContext);
        mGLThread.registerRenderMessageHandler(MSG_INIT_EGL_SURFACE, this::initEGLSurface);
        mGLThread.registerRenderMessageHandler(MSG_CHANGE_SIZE, this::changeSurface);
        mGLThread.registerRenderMessageHandler(MSG_RENDER, this::drawFrame);
        mGLThread.registerRenderMessageHandler(MSG_DESTROY_SURFACE, this::destroyEGLSurface);
        mGLThread.registerRenderMessageHandler(MSG_DESTROY_CONTEXT, this::destroyEGLContext);
    }

    /***以下方法是对外配置相关的API************************************/

    /**
     * 设置父context，以便进行GL Context共享
     *
     * @param eglContext 父context
     */
    public void setParentEGLContext(EGLContext eglContext) {
        mParentEGLContext = eglContext;
    }

    public EGLContext getParentEGLContext() {
        return mParentEGLContext;
    }

    /**
     * 设置本地窗口提供器，如果提供的本地窗口为空，则会将EGL环境配置成离屏渲染
     *
     * @param nativeWindowProvider
     */
    public void setNativeWindowProvider(INativeWindowProvider nativeWindowProvider) {
        mNativeWindowProvider = nativeWindowProvider;
    }

    /***以下方法是生命周期相关方法************************************/
    public void surfaceAvailable(int width, int height) {
        if (width <= 0
                || height <= 0) {
            return;
        }

        // 确保渲染线程是活跃的;
        mGLThread.startRenderThreadIfNeed();

        // 初始化EGL环境
        mGLThread.sendRenderMessage(MSG_INIT_EGL_CONTEXT);

        // 更新surface
        mGLThread.sendRenderMessage(MSG_INIT_EGL_SURFACE);

        mGLThread.sendRenderMessage(MSG_CHANGE_SIZE, width, height);

        nextFrame();
    }


    private void nextFrame() {
        mGLThread.sendRenderMessage(MSG_RENDER);
    }

    public void surfaceUnavailable() {
        mGLThread.sendRenderMessage(MSG_DESTROY_SURFACE);
    }

    public void destroy() {
        // 在销毁的时候，需要考虑该方法先于surfaceTextureUnavailable方法调用的场景;
        // 所以此场景中，必须先销毁egl surface，再销毁egl display;
        // 如果时序颠倒， 则elg surface会保留，造成内存泄漏;
        mGLThread.sendRenderMessage(MSG_DESTROY_SURFACE);
        mGLThread.sendRenderMessage(MSG_DESTROY_CONTEXT);
    }

    /***以下是EGL配置管控及GL渲染流程相关方法, 由渲染线程执行************************************/
    private boolean initEGLContext(Message msg) {
        mEgl = (EGL10) EGLContext.getEGL();

        //获取显示设备
        mEGLDisplay = mEgl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
        if (mEGLDisplay == EGL10.EGL_NO_DISPLAY) {
            throw new RuntimeException("eglGetDisplay failed! " + mEgl.eglGetError());
        }

        //version中存放EGL版本号
        int[] version = new int[2];

        //初始化EGL
        if (!mEgl.eglInitialize(mEGLDisplay, version)) {
            throw new RuntimeException("eglInitialize failed! " + mEgl.eglGetError());
        }

        nativeWindow = fetchNativeWindow();
        if (nativeWindow != null
                && !(nativeWindow instanceof SurfaceTexture)
                && !(nativeWindow instanceof Surface)) {
            // 提供一个无实义的本地窗口
            nativeWindow = null;
        }

        if (nativeWindow instanceof SurfaceTexture) {
            // 这一行可以不用写，主要是为了体现SurfaceTexture对象可转换成Surface对象
            nativeWindow = new Surface((SurfaceTexture) nativeWindow);
        }

        //构造需要的配置列表
        int[] attributes;
        if (nativeWindow == null) {
            attributes = new int[]{
                    EGL10.EGL_RED_SIZE, 8,
                    EGL10.EGL_GREEN_SIZE, 8,
                    EGL10.EGL_BLUE_SIZE, 8,
                    EGL10.EGL_ALPHA_SIZE, 8,
                    EGL10.EGL_BUFFER_SIZE, 32,
                    EGL10.EGL_RENDERABLE_TYPE, 4,
                    EGL10.EGL_SURFACE_TYPE, EGL10.EGL_PBUFFER_BIT,
                    EGL10.EGL_NONE
            };
        } else {
            attributes = new int[]{
                    EGL10.EGL_RED_SIZE, 8,
                    EGL10.EGL_GREEN_SIZE, 8,
                    EGL10.EGL_BLUE_SIZE, 8,
                    EGL10.EGL_ALPHA_SIZE, 8,
                    EGL10.EGL_BUFFER_SIZE, 32,
                    EGL10.EGL_RENDERABLE_TYPE, 4,
                    EGL10.EGL_SURFACE_TYPE, EGL10.EGL_WINDOW_BIT,
                    EGL10.EGL_NONE
            };
        }
        int[] configsNum = new int[1];

        //EGL选择配置
        if (!mEgl.eglChooseConfig(mEGLDisplay, attributes, mEGLConfig, 1, configsNum)) {
            throw new RuntimeException("eglChooseConfig failed! " + mEgl.eglGetError());
        }

        //创建上下文
        int[] contextAttribs = {
                EGL14.EGL_CONTEXT_CLIENT_VERSION, 3,
                EGL10.EGL_NONE
        };
        EGLContext sharedContext = mParentEGLContext == null ? EGL10.EGL_NO_CONTEXT : mParentEGLContext;
        mEGLContext = mEgl.eglCreateContext(mEGLDisplay, mEGLConfig[0], sharedContext, contextAttribs);

        if (mEGLDisplay == EGL10.EGL_NO_DISPLAY || mEGLContext == EGL10.EGL_NO_CONTEXT) {
            throw new RuntimeException("eglCreateContext fail failed! " + mEgl.eglGetError());
        }

        mGL = mEGLContext.getGL();
        return true;
    }

    private boolean initEGLSurface(Message message) {
        //创建EGL显示窗口
        if (nativeWindow == null) {
            mEglSurface = mEgl.eglCreatePbufferSurface(mEGLDisplay, mEGLConfig[0], null);
        } else {
            mEglSurface = mEgl.eglCreateWindowSurface(mEGLDisplay, mEGLConfig[0], nativeWindow, null);
        }

        // 上下文与渲染线程绑定
        if (!mEgl.eglMakeCurrent(mEGLDisplay, mEglSurface, mEglSurface, mEGLContext)) {
            throw new RuntimeException("eglMakeCurrent failed! " + mEgl.eglGetError());
        }

        if (!mIsInit) {
            if (mRender != null) {
                mRender.onSurfaceCreated((GL10) mGL, mEGLConfig[0]);
            }

            mIsInit = true;
        }
        return true;
    }

    private boolean changeSurface(Message message) {
        if (message == null) {
            return true;
        }
        int width = message.arg1;
        int height = message.arg2;
        width = width < 0 ? 0 : width;
        height = height < 0 ? 0 : height;
        if (mCurrentSurfaceWidth == width
                && mCurrentSurfaceHeight == height) {
            return true;
        }
        mCurrentSurfaceWidth = width;
        mCurrentSurfaceHeight = height;
        if (mRender != null) {
            mRender.onSurfaceChanged((GL10) mGL, mCurrentSurfaceWidth, mCurrentSurfaceHeight);
        }
        return true;
    }

    private Object fetchNativeWindow() {
        if (mNativeWindowProvider == null) {
            return null;
        }

        return mNativeWindowProvider.provideNativeWindow();
    }

    private boolean drawFrame(Message message) {
        if (mEGLConfig == null
                || mEglSurface == null
                || mEGLContext == null
                || !mIsInit) {
            nextFrame();
            return true;
        }

        // 上下文与渲染线程绑定
        if (!mEgl.eglMakeCurrent(mEGLDisplay, mEglSurface, mEglSurface, mEGLContext)) {
            throw new RuntimeException("eglMakeCurrent failed! " + mEgl.eglGetError());
        }
        GLES20.glViewport(0, 0, mCurrentSurfaceWidth, mCurrentSurfaceHeight);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
        GLES20.glClearColor(1f, 1f, 1f, 1f);
        if (mRender != null) {
            mRender.onDrawFrame((GL10) mGL);
        }
        // 交换buffer
        mEgl.eglSwapBuffers(mEGLDisplay, mEglSurface);

        // 渲染循环
        nextFrame();
        return true;
    }

    private boolean destroyEGLSurface(Message message) {
        if (!mIsInit) {
            return true;
        }
        if (mRender != null) {
            mRender.onSurfaceDestroy();
        }
        mIsInit = false;
        if (mEglSurface != null
                && mEGLDisplay != null) {
            mEgl.eglMakeCurrent(mEGLDisplay, EGL10.EGL_NO_SURFACE,
                    EGL10.EGL_NO_SURFACE,
                    EGL10.EGL_NO_CONTEXT);
            mEgl.eglDestroySurface(mEGLDisplay, mEglSurface);
            mEglSurface = null;
        }

        mCurrentSurfaceWidth = 0;
        mCurrentSurfaceHeight = 0;
        return true;
    }

    private boolean destroyEGLContext(Message msg) {
        if (mEGLContext != null
                && mEgl != null) {
            mEgl.eglDestroyContext(mEGLDisplay, mEGLContext);
            mEGLContext = null;
        }

        if (mEGLDisplay != null
                && mEgl != null) {
            mEgl.eglTerminate(mEGLDisplay);
            mEGLDisplay = null;
        }

        mEgl = null;

        mIsInit = false;
        mCurrentSurfaceWidth = 0;
        mCurrentSurfaceHeight = 0;
        mGLThread.destroyRenderThread();
        return true;
    }
}
