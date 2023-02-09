#ifndef XR_COMMON_H_
#define XR_COMMON_H_

#ifdef __ANDROID__
#include <android/log.h>
#endif
//  xr render kit中3D模型节点的节点名
//#define XR_3D_MODEL_NODE_NAME "xr_3d_node"

#ifdef DEBUG
#ifdef __ANDROID__
#define XRCLog(format, ...) __android_log_print(ANDROID_LOG_ERROR, "XRAppOSJNI", format, ##__VA_ARGS__)
#else
#define XRCLog(format, ...) printf(format, ##__VA_ARGS__)
#endif
#else
#define XRCLog(format, ...)
#endif

#define XRCDebugLog(format, ...) XRCLog("[DEBUG] " format "\n", ##__VA_ARGS__)
#define XRCInfoLog(format, ...) XRCLog("[INFO] " format "\n", ##__VA_ARGS__)
#define XRCWarnLog(format, ...) XRCLog("[WARN] " format "\n", ##__VA_ARGS__)
#define XRCErrorLog(format, ...) XRCLog("[ERROR] " format "\n", ##__VA_ARGS__)

// 对外暴露API符号
#define XRAPPOS_PUBLIC __attribute__((visibility("default")))

#define XRAPPOS_GUARDED_BY(lock) __attribute__((guarded_by(lock)))

#if (__aarch64__) && (defined(__ARM_NEON__) || defined(__ARM_NEON))
// 是否可以通过NEON加速，为后续性能优化做准备
#define ARM_NEON_64
#endif

#ifndef CHECK
#define CHECK(condition)                                                   \
  if (!(condition)) {                                                      \
    XRCErrorLog("*** CHECK FAILED at %s:%d: %s", __FILE__, __LINE__, #condition); \
    abort();                                                               \
  }
#endif  // CHECK

#define SK_ARRAY_COUNT(array)   (sizeof(array) / sizeof(array[0]))
#define NON_RETURN_CHECK(condition, msg) if(!(condition)){__android_log_print(ANDROID_LOG_ERROR, "AliNNJNI", msg);return;}
#define VALUE_RETURN_CHECK(condition, value, msg) if(!(condition)){__android_log_print(ANDROID_LOG_ERROR, "AliNNJNI", msg);return value;}

//inline void ThrowException(void* env, const char* msg) {
//  XRCErrorLog("Throw Java exception: %s", msg);
//#ifdef __ANDROID__
//  jclass c = ((JNIEnv*)env)->FindClass("java/lang/RuntimeException");
//  ((JNIEnv*)env)->ThrowNew(c, msg);
//#endif
//}
//
//#ifndef CHECKANDTHROW
//#define CHECKANDTHROW(condition, env, msg, ...)                            \
//  if (!(condition)) {                                                      \
//    XRCErrorLog("*** CHECK FAILED at %s:%d: %s", __FILE__, __LINE__, #condition); \
//    ThrowException(env, msg);                                    \
//    return  ##__VA_ARGS__;                                                  \
//  }
//#endif  // CHECKANDTHROW
#endif //
