# Generated from ctbot-viewer.pro.

cmake_minimum_required(VERSION 3.16)
project(ctbot-viewer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS QuickControls2)

qt_add_executable(ctbot-viewer WIN32 MACOSX_BUNDLE
    actuator_viewer.cpp actuator_viewer.h
    bot_console.cpp bot_console.h
    command.cpp command.h
    connect_button.h
    connection_manager.cpp connection_manager.h
    log_viewer.cpp log_viewer.h
    main.cpp
    map_image.cpp map_image.h
    map_viewer.cpp map_viewer.h
    remotecall_list.cpp remotecall_list.h
    remotecall_model.cpp remotecall_model.h
    remotecall_viewer.cpp remotecall_viewer.h
    remotecontrol_viewer.cpp remotecontrol_viewer.h
    script_editor.cpp script_editor.h
    sensor_viewer.cpp sensor_viewer.h
    value_list.cpp value_list.h
    value_model.cpp value_model.h
    value_viewer.cpp value_viewer.h
)

target_compile_definitions(ctbot-viewer PUBLIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

target_link_libraries(ctbot-viewer PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::QuickControls2
)

set_property(TARGET ctbot-viewer  PROPERTY CXX_STANDARD 20)

# Resources:
set(qml_resource_files
    "ActuatorViewer.qml"
    "AutoSizingMenu.qml"
    "ConsoleComponent.qml"
    "LogComponent.qml"
    "Main.qml"
    "MainComponent.qml"
    "MapComponent.qml"
    "MenuComponent.qml"
    "MiniLogComponent.qml"
    "RCButton.qml"
    "RemoteCallComponent.qml"
    "RemoteCallViewer.qml"
    "RemoteControl.qml"
    "ScriptComponent.qml"
    "SensorViewer.qml"
    "TabComponent.qml"
    "fonts/PTMono-Regular.ttf"
    "fonts/Roboto-Black.ttf"
    "fonts/Roboto-BlackItalic.ttf"
    "fonts/Roboto-Bold.ttf"
    "fonts/Roboto-BoldItalic.ttf"
    "fonts/Roboto-Italic.ttf"
    "fonts/Roboto-Light.ttf"
    "fonts/Roboto-LightItalic.ttf"
    "fonts/Roboto-Medium.ttf"
    "fonts/Roboto-MediumItalic.ttf"
    "fonts/Roboto-Regular.ttf"
    "fonts/Roboto-Thin.ttf"
    "fonts/Roboto-ThinItalic.ttf"
)

qt6_add_resources(ctbot-viewer "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)

install(TARGETS ctbot-viewer
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
