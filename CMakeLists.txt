cmake_minimum_required(VERSION 3.27)
project(BufferSharingInMultiTenantDb)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_TOOLCHAIN_FILE build/conan_toolchain.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)