package com.android.common.common_tools;

import android.graphics.Bitmap;
import android.opengl.GLES30;

import java.nio.ByteBuffer;

public class OpenGLUtils {
    // 此方法获取的bitmap与实际渲染的结果相比，有一个垂直翻转
    public static Bitmap readPixelFromGPUToBitmap(int width, int height) {
        // todo 检查线程环境
        Bitmap bitmap;
        ByteBuffer rgbaBuf = ByteBuffer.allocateDirect(width * height * 4);
        rgbaBuf.position(0);
//         默认读的是GL_COLOR_ATTACHMENT0 buffer，所以可以省略
//         GLES30.glReadBuffer(GLES30.GL_COLOR_ATTACHMENT0)
        GLES30.glReadPixels(0, 0, width, height, GLES30.GL_RGBA, GLES30.GL_UNSIGNED_BYTE, rgbaBuf);
        rgbaBuf.rewind();
        bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bitmap.copyPixelsFromBuffer(rgbaBuf);
        return bitmap;
    }
}