cmake_minimum_required(VERSION 3.4.1)

# 引入XRAppOS编译模块
# 必须优先引入
include(./cmake/CMakeLists.txt)

# 引入数据协议模块
include(./XRAppOS/protocol/CMakeLists.txt)

# 引入渲染引擎模块
include(./XRAppOS/render/CMakeLists.txt)

# android 系统相关库
if (${CMAKE_SYSTEM_NAME} MATCHES "Android")

    include(AndroidNdkModules)
    android_ndk_import_module_cpufeatures()

    # jni模块
    include("platform/android/XRAppOS/src/main/jni/CMakeLists.txt")
    target_link_libraries(${XR_APP_OS_TARGET_NAME} android log EGL GLESv3 jnigraphics z)
endif ()
