cmake_minimum_required(VERSION 3.10.0)

# 指定cmake编译项目
project(xr_app_os CXX C)

# 增加XRRenderKi编译模块的搜索路径
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# 找寻XRRenderKit编译模块
find_package(XRAppOS REQUIRED)

#message(FATAL_ERROR "${XR_APP_OS_ROOT}")
include_directories(
        ${XR_APP_OS_ROOT})
