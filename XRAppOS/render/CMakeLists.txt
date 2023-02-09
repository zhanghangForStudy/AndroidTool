find_package(GLM REQUIRED)
xrappos_cc_submodule(
        NAME
            ${XR_APP_OS_TARGET_NAME}
        HDRS
        SRCS
            "${CMAKE_CURRENT_LIST_DIR}"
            "${CMAKE_CURRENT_LIST_DIR}/opengl"
            "${CMAKE_CURRENT_LIST_DIR}/utils"
        DEPS
            glm
)