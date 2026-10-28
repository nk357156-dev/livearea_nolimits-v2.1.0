cmake_minimum_required(VERSION 3.10)

if(NOT DEFINED ENV{VITASDK})
    message(FATAL_ERROR "VITASDK environment variable is not set")
endif()

set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake")

project(vita_cube C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(vita_cube main.c)

target_link_libraries(vita_cube
    vita2d
    SceCtrl_stub
    SceKernel_stub
    SceDisplay_stub
    SceSysmodule_stub
    m
)

vitalink_package(vita_cube)
