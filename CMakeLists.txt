cmake_minimum_required(VERSION 3.19)

set(CMAKE_PREFIX_PATH
    "C:/Qt/6.8.3/msvc2022_64/lib/cmake"
    "C:/Qt/6.8.3/msvc2022_64/lib/cmake/Qt6"
)

project(SimsSwitcher LANGUAGES CXX)

find_package(Qt6 6.8.3 REQUIRED COMPONENTS Core Widgets Network LinguistTools)

qt_standard_project_setup()

add_executable(SimsSwitcher
    WIN32
    mainwindow.cpp
    mainwindow.ui
    main.cpp
    resources.qrc
    icons/SimSwitcherIcon.rc  # Uncomment if this file exists
)

target_link_libraries(SimsSwitcher
    PRIVATE Qt6::Core Qt6::Widgets Qt6::Network
)

add_custom_command(
    TARGET SimsSwitcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/icons"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/icons"
    COMMENT "Copying icons folder to the build directory..."
)

add_custom_command(
    TARGET SimsSwitcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/inc"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/inc"
    COMMENT "Copying inclusions folder to the build directory..."
)
