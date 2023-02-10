package com.android.xrappos.eglsession;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * EGL环境中的渲染器;<br/>
 * 与{@link android.opengl.GLSurfaceView.Renderer}接口类似，
 * 除了多了一个onSurfaceDestroy方法;<br/>
 * 该接口的方法，都在GL渲染线程中被回调;
 */
public interface IEGLRender {
    void onSurfaceCreated(GL10 gl, EGLConfig config);

    void onSurfaceChanged(GL10 gl, int width, int height);

    void onDrawFrame(GL10 gl);

    void onSurfaceDestroy();
}
