if (NOT TARGET eigen)
    # head-only target
    add_library(eigen INTERFACE)
endif ()
include_directories(${XR_APP_OS_ROOT}/third_parts/Eigen/Eigen_includes)