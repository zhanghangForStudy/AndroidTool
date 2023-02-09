# 本文件对外提供了两个核心方法：
# xrappos_cc_submodule     : 增加某一个xrappos C++子模块
# find_arsr_core           : [刚方法未使用]生成libarsr_core.so，此方法适用于依赖该so的aar
# 调用find_package(ARFoundationCore)后，可直接使用上述两个方法

function(xrappos_cc_submodule)
    cmake_parse_arguments(XR_APP_OS_CC_LIB
            "DISABLE_INSTALL;PUBLIC;TESTONLY"
            "NAME"
            "HDRS;SRCS;DEPS;COPTS;DEFINES;LINKOPTS"
            ${ARGN}
            )

    set(_NAME "${XR_APP_OS_CC_LIB_NAME}")

    # Check if this is a header-only library
    # Note that as of February 2019, many popular OS's (for example, Ubuntu
    # 16.04 LTS) only come with cmake 3.5 by default.  For this reason, we can't
    # use list(FILTER...)
    find_cxx_source_files("${XR_APP_OS_CC_LIB_SRCS}" ARSR_CC_SRCS)

    if (TARGET ${_NAME})
        target_sources(${_NAME} PRIVATE ${ARSR_CC_SRCS})
    else ()
        # 添加目标
        # 这里的目标一般是arsr模块对应的so文件
        # 对于子模块，之所以不使用静态库主要基于以下两点考虑：
        # 1. 如果子模块采取静态库，则增加符号重定义的风险；
        # 2. 如果子模块采取静态库，则在编译子模块时，对第三方库的依赖，
        #    并不能直接让模块so文件对第三库产生依赖，仍需要so文件
        #    手动链接依赖的第三方库；
        add_library(${_NAME} SHARED ${ARSR_CC_SRCS})
    endif ()

    target_include_directories(${_NAME}
            PUBLIC
            "${XR_APP_OS_CC_LIB_HDRS}"
            )
    target_compile_options(${_NAME}
            PRIVATE ${XR_APP_OS_CC_LIB_COPTS})

    target_link_libraries(${_NAME}
            ${XR_APP_OS_CC_LIB_DEPS}
            )
endfunction()

function(find_arsr_core)
    set(arsr_core_includes
            "${XR_RENDER_PUBLIC_INCLUDE_ROOT}")
    include_directories("${arsr_core_includes}")

    if (NOT TARGET arsr::core)
        add_library(arsr::core SHARED IMPORTED)

        #    设置libarsr_core.so文件的路径
        if (NOT FIND_ARSR_CORE_WITHOUT_SOURCE)
            message(WARNING "find arsr core so with compilering the source codes...")

            set(ARSR_SO_PATH "${XR_APP_OS_ROOT}/projects/android/ar_scene_render/build/intermediates/cmake/${CMAKE_BUILD_TYPE}/obj/${ANDROID_ABI}/libarsr_core.so")

            # 设置gradle命令路径
            set(GRADLE_COMMAND ":ar_scene_render:externalNativeBuild")

            if ("${CMAKE_BUILD_TYPE}" STREQUAL "release")
                set(GRADLE_COMMAND "${GRADLE_COMMAND}Release")
            else ()
                set(GRADLE_COMMAND "${GRADLE_COMMAND}Debug")
            endif ()

            #   定义生成libarsr_core.so的自定义命令${ARSR_SO_PATH}
            add_custom_command(
                    OUTPUT ${ARSR_SO_PATH}
                    COMMAND ./gradlew ${GRADLE_COMMAND}
                    WORKING_DIRECTORY "${XR_APP_OS_ROOT}/projects/android"
                    VERBATIM
                    COMMENT "start invoking the command: ${GRADLE_COMMAND}")


            add_custom_target(GENERATOR_ARSR_CORE_SO DEPENDS ${ARSR_SO_PATH})
            add_dependencies(arsr::core GENERATOR_ARSR_CORE_SO)
        else ()
            message(WARNING "find arsr core so without compilering the source codes...")
            set(ARSR_SO_PATH "${XR_APP_OS_ROOT}/jni/${ANDROID_ABI}/libarsr_core.so")
        endif ()
        set_target_properties(arsr::core PROPERTIES IMPORTED_LOCATION "${ARSR_SO_PATH}")
    endif ()
endfunction()