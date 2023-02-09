if (NOT TARGET cpufeatures)
    # 蛋疼的cpufeatures库
    add_library(cpufeatures STATIC ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
endif ()
include_directories(${ANDROID_NDK}/sources/android/cpufeatures)