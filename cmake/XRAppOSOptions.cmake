XRAppOS_OPTION(ENABLE_LOCAL_DEBUG_MODE "whether use the macro[ENABLE_LOCAL_DEBUG] to debug some features or not" OFF ENABLE_LOCAL_DEBUG_MODE)

option(FIND_ARFOUNDATION_CORE_WITHOUT_SOURCE "whether find arfoundation core so file without compilering the source codes" OFF)

# Android侧特性宏定义
if (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    add_definitions(-DUNIX_OS)
    add_definitions(-DIS_ANDROID)
    add_definitions(-DANDROID_ARM)
    if (${ANDROID_ABI} MATCHES "arm64-v8a")
        add_definitions(-DANDROID_ARM64)
    endif()
endif()