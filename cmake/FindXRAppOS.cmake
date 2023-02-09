# 本文件主要提供三个变量：
# XR_APP_OS_TARGET_NAME      :  共享使用目标名lixrappos.so
# XR_APP_OS_ROOT             :  XRAppOS的根目录
# XR_RENDER_PUBLIC_INCLUDE_ROOT  :  XRAppOS模块公共头文件根目录

# 设置xr_app_os模块的根目录
# nativepub.gradle中可能间接设置该属性
if (NOT DEFINED XR_APP_OS_ROOT)
    set(XR_APP_OS_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
endif ()

set(XR_APP_OS_TARGET_NAME "xrappos")

# 启用编译语言
enable_language(CXX C)

# 设置cxx相关的编译参数
# todo debug包也需要隐藏符号(控制对外暴露的头文件及API)，同时需要平衡调试功能
message(WARNING "[FindXRRenderKitDebug]::the GRADLE_BUILD_TYPE is ${GRADLE_BUILD_TYPE}")
if (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    if (${GRADLE_BUILD_TYPE} MATCHES "debug")
        message(WARNING "[FindXRRenderKitDebug]::the CMAKE_BUILD_TYPE is ${debug}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -fPIC -fno-rtti")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++17 -O0 -g -fPIC -fno-rtti")
        if (USEASAN)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
            # set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fsanitize=address")
        endif ()
    elseif (${GRADLE_BUILD_TYPE} MATCHES "releaseProfiling")
        set(CMAKE_BUILD_TYPE release)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O2 -finline-functions -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -Werror=format-security -fomit-frame-pointer -fstrict-aliasing -ffunction-sections -fdata-sections -ffast-math -fno-rtti -fno-exceptions -flax-vector-conversions")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -Werror=format-security -fomit-frame-pointer -fstrict-aliasing -ffunction-sections -fdata-sections -ffast-math -fno-rtti -fno-exceptions -flax-vector-conversions")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl, --version-script=${CMAKE_CURRENT_SOURCE_DIR}/export.def")
    else ()
        message(WARNING "[FindXRRenderKitDebug]::the CMAKE_BUILD_TYPE is ${release}")
        set(CMAKE_BUILD_TYPE release)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Os -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -Werror=format-security -fomit-frame-pointer -fstrict-aliasing -ffunction-sections -fdata-sections -ffast-math -fno-rtti -fno-exceptions -flax-vector-conversions")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -Werror=format-security -fomit-frame-pointer -fstrict-aliasing -ffunction-sections -fdata-sections -ffast-math -fno-rtti -fno-exceptions -flax-vector-conversions")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections") # -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/export.def")
    endif ()
else ()
    set(CMAKE_C_STANDARD 99)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -fPIC -fno-rtti")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -fPIC -fno-rtti")
endif ()

# 导入XRAppOS相关函数、宏定义的cmake模块
include(${CMAKE_CURRENT_LIST_DIR}/XRAppOSUtils.cmake)

# 导入相关option的camke模块
include(${CMAKE_CURRENT_LIST_DIR}/XRAppOSOptions.cmake)

# 导入so库编译相关定义的cmake模块
include(${CMAKE_CURRENT_LIST_DIR}/XRAppOSCompiler.cmake)

# xr_app_os模块依赖的第三方库
# 第三方库找寻路径
list(APPEND CMAKE_MODULE_PATH "${XR_APP_OS_ROOT}/third_parts")
# cpu feature库找寻路径
#list(APPEND CMAKE_MODULE_PATH "${XR_APP_OS_ROOT}/third_parts/cpufeatures")