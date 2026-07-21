cmake_minimum_required(VERSION 3.16)
project(GPUNameSwap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(GPUNameSwap WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    sidedrawer.cpp
    sidedrawer.h
    swaptab.cpp
    swaptab.h
    logstab.cpp
    logstab.h
    abouttab.cpp
    abouttab.h
    autostarttab.cpp
    autostarttab.h
    gpu_database.h
    resources.qrc
)

target_include_directories(GPUNameSwap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(GPUNameSwap PRIVATE Qt6::Widgets dwmapi)