if (NOT TARGET arsr::alicvkit)
    # opencv库
    add_library(arsr::alicvkit SHARED IMPORTED)
    set_target_properties(arsr::alicvkit PROPERTIES IMPORTED_LOCATION ${XR_APP_OS_ROOT}/third_parts/cvkit/libs/Android/${ANDROID_ABI}/libAliCVKit.so)
endif ()
include_directories(${XR_APP_OS_ROOT}/third_parts/cvkit/cvkit_includes)