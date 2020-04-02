cmake_minimum_required(VERSION 3.15)
project(vulkan_api_course_clion)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(vulkan_api_course_clion
        main.cpp
        renderer.cpp
        renderer.h
        utilities.h)

target_link_libraries(${PROJECT_NAME} vulkan glfw xcb)
