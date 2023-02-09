package com.android.xrappos;

public class NativeLib {

    // Used to load the 'xrappos' library on application startup.
    static {
        System.loadLibrary("xrappos");
    }

    /**
     * A native method that is implemented by the 'xrappos' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}