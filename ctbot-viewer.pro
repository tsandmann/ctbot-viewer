# This file is part of the c't-Bot remote viewer tool.
# Copyright (c) 2020 Timo Sandmann
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

QT += quick
QT += quickcontrols2

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        actuator_viewer.cpp \
        command.cpp \
        connection_manager.cpp \
        log_viewer.cpp \
        main.cpp \
        map_image.cpp \
        map_viewer.cpp \
        remotecall_list.cpp \
        remotecall_model.cpp \
        remotecall_viewer.cpp \
        remotecontrol_viewer.cpp \
        script_editor.cpp \
        sensor_viewer.cpp \
        value_list.cpp \
        value_model.cpp \
        value_viewer.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

macx {
    ICON = images/AppIcon.icns
}

win32 {
    RC_ICONS = images/AppIcon.ico
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    actuator_viewer.h \
    command.h \
    connect_button.h \
    connection_manager.h \
    log_viewer.h \
    map_image.h \
    map_viewer.h \
    remotecall_list.h \
    remotecall_model.h \
    remotecall_viewer.h \
    remotecontrol_viewer.h \
    script_editor.h \
    sensor_viewer.h \
    value_list.h \
    value_model.h \
    value_viewer.h
