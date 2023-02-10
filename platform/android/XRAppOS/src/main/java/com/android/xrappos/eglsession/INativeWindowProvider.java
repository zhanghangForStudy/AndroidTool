package com.android.xrappos.eglsession;

import android.graphics.SurfaceTexture;
import android.view.Surface;

/**
 * 本地窗口提供器
 */
public interface INativeWindowProvider {
    /**
     * 提供相应的本地窗口;
     * 目前，支持以下三种类型的本地窗口对象
     * <ol>
     * <li>{@link SurfaceTexture}</li>
     * <li>{@link Surface}</li>
     * <li>返回空，此种场景下，只能进行离屏渲染</li>
     * </ol>
     *
     * @return
     */
    Object provideNativeWindow();
}
