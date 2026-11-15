cmake_minimum_required(VERSION 3.16)
project(pr5_vizprog VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Одна цель сборки – одно приложение, без дополнительных программ.
add_executable(pr5_vizprog WIN32
    src/main.cpp src/mainwindow.h src/mainwindow.cpp
    src/scenebutton.h src/scenebutton.cpp
    src/buttoneventfilter.h src/buttoneventfilter.cpp
    src/scenecontroller.h src/scenecontroller.cpp
)
target_link_libraries(pr5_vizprog PRIVATE Qt6::Widgets)
if(MSVC)
    target_compile_options(pr5_vizprog PRIVATE /utf-8)
endif()
