if (NOT TARGET glm)
    # head-only target
    add_library(glm INTERFACE)
endif ()
include_directories(${XR_APP_OS_ROOT}/third_parts/glm/glm_includes)