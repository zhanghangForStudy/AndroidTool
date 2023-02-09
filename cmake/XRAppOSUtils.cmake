# XRAppOS_OPTION(<option_variable>
#          "help string describing the option"
#          <initial value or boolean expression>
#          [C++代码中使用的宏名字])
macro(XRAppOS_OPTION variable description value)
    option(${variable} "${description}" ${value})

    if (${ARGN})
        set(__macro_name_in_code ${ARGN})

        if (${variable})
            add_definitions(-D${__macro_name_in_code})
        endif ()

    endif ()
endmacro()

function(find_cxx_source_files cxx_source_dirs out_source_files)
    set(source_files_local1 "")
    foreach (CPath ${cxx_source_dirs})
        if (CPath MATCHES ".cpp$")
            set(source_files_local1 ${CPath} ${CFiles})
            continue()
        endif ()
        #       C文件
        file(GLOB CFiles ${CPath}/*.c)
        set(source_files_local1 ${source_files_local1} ${CFiles})

        #       cc文件
        file(GLOB CCFiles ${CPath}/*.cc)
        set(source_files_local1 ${source_files_local1} ${CCFiles})

        #        cpp文件
        file(GLOB CPPFiles ${CPath}/*.cpp)
        set(source_files_local1 ${source_files_local1} ${CPPFiles})
    endforeach ()

    set(${out_source_files} "${source_files_local1}" PARENT_SCOPE)
endfunction()