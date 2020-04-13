/*
 * This file is part of the c't-Bot remote viewer tool.
 * Copyright (c) 2020 Timo Sandmann
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3


ApplicationWindow {
    id: applicationWindow
    visible: true
    height: 605
    width: 625
    minimumHeight: 605
    minimumWidth: 625
    title: qsTr("ct-Bot Remote Viewer 0.5")

    function fontsize(size) {
        if (Qt.platform.os == "osx" || Qt.platform.os == "ios") {
            return size * 4 / 3;
        }

        if (Qt.platform.os == "windows") {
            return size;
        }

        return size * 0.85;
    }

    menuBar: MenuComponent {
        id: menubar
    }

    StackLayout {
        id: layout
        anchors.fill: parent
        currentIndex: tabbar.currentIndex
        focus: true

        MainComponent {
            id: main_viewer
        }

        RemoteCallComponent {}

        LogComponent {}

        MapComponent {}

        ScriptComponent {}

        ConsoleComponent {}
    }

    footer: TabComponent {
        id: tabbar
    }
}
