cmake_minimum_required(VERSION 3.4...3.18)
project(rtc_client)

# # 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python 3.10 EXACT REQUIRED COMPONENTS Interpreter Development)
# 找到 pybind11
set(pybind11_DIR "/usr/local/lib/python3.10/site-packages/pybind11/share/cmake/pybind11")
find_package(pybind11 REQUIRED)

# 包含 pybind11 头文件
# include_directories("${CMAKE_SOURCE_DIR}/pybind11/include/pybind11")
# 添加你的 RtcClient 头文件路径
include_directories("${CMAKE_SOURCE_DIR}/api")
include_directories("${CMAKE_SOURCE_DIR}")

# 添加源文件
# add_library(rtc_client MODULE
#     rtcclient.cpp
# )
# 添加你的模块
pybind11_add_module(rtc_client rtcclient.cpp)
set_target_properties(rtc_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/.."
)
# # 链接 pybind11 库
# target_link_libraries(rtc_client PRIVATE pybind11::module)

# 如果你的 RtcClient 依赖其他库，可以在这里链接
target_link_libraries(rtc_client PRIVATE "${CMAKE_SOURCE_DIR}/librtc_avd_so.so" pybind11::embed)

