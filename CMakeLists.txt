cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project("game")

set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories("/usr/include/libusb-1.0/")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

file(COPY src/content DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


add_executable(game ./src/game.cpp ./src/content/glad.c)
target_link_libraries(game glfw ${CMAKE_DL_LIBS})

