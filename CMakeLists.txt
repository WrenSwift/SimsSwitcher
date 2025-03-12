cmake_minimum_required(VERSION 3.19)
project(SimsSwitcher LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

#if(WIN32)
#    set(APP_ICON_RESOURCE "${CMAKE_SOURCE_DIR}/icons/SimSwitcherIcon.rc")
#    add_executable(SimsSwitcher
#        WIN32
#        mainwindow.cpp
#        mainwindow.h
#        mainwindow.ui
#        main.cpp
#        resources.qrc
#        ${APP_ICON_RESOURCE}
#    )
#else()
#    add_executable(SimsSwitcher
#        main.cpp
#        mainwindow.cpp
#        mainwindow.h
#        mainwindow.ui
#        resources.qrc
#    )
#endif()

qt_standard_project_setup()

qt_add_translations(
    TARGETS SimsSwitcher
    TS_FILES SimsSwitcher_en_US.ts
)

set(APP_ICON_RESOURCE "${CMAKE_SOURCE_DIR}/icons/SimSwitcherIcon.rc")

add_executable(SimsSwitcher
        WIN32
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        main.cpp
        resources.qrc
        ${APP_ICON_RESOURCE}
)

qt_add_resources(RESOURCES resources.qrc)
target_sources(SimsSwitcher PRIVATE ${RESOURCES})

target_link_libraries(SimsSwitcher
    PRIVATE
        Qt6::Core
        Qt6::Widgets
)

include(GNUInstallDirs)

install(TARGETS SimsSwitcher
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SimsSwitcher
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# Define source and destination directories
set(ICONS_SOURCE_FOLDER "${CMAKE_SOURCE_DIR}/icons")
set(ICONS_DEST_FOLDER "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/icons")

# Add a custom command to copy the folder after build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ICONS_SOURCE_FOLDER}
    ${ICONS_DEST_FOLDER}
    COMMENT "Copying icons folder to the build directory..."
)

# Define source and destination directories
set(INC_SOURCE_FOLDER "${CMAKE_SOURCE_DIR}/inc")
set(INC_DEST_FOLDER "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/inc")

# Add a custom command to copy the folder after build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${INC_SOURCE_FOLDER}
    ${INC_DEST_FOLDER}
    COMMENT "Copying inclusions folder to the build directory..."
)

