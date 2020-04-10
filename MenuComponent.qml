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

MenuBar {
    property alias connect_menu : connect

    font.pointSize: fontsize(12)
    width: parent.width

    AutoSizingMenu {
        title: qsTr("Viewer")

        MenuItem {
            id: connect
            text: qsTr("Connect")
            font.pointSize: fontsize(12)
            horizontalPadding: 10
            onTriggered: {
                main_viewer.hostname.connectClicked(main_viewer.hostname.text, main_viewer.port.text);
            }
        }

        MenuItem {
            text: qsTr("Exit")
            font.pointSize: fontsize(12)
            horizontalPadding: 10
            onTriggered: {
                Qt.quit();
            }
        }
    }

    AutoSizingMenu {
        title: qsTr("Components")

        MenuItem {
            text: qsTr("Main")
            font.pointSize: fontsize(12)
            horizontalPadding: 10
            onTriggered: layout.currentIndex = 0
        }

        MenuItem {
            text: qsTr("Remote Calls")
            font.pointSize: fontsize(12)
            horizontalPadding: 10
            onTriggered: layout.currentIndex = 1
        }

        MenuItem {
            text: qsTr("Logs")
            font.pointSize: fontsize(12)
            horizontalPadding: 10
            onTriggered: layout.currentIndex = 2
        }

        MenuItem {
            text: qsTr("Map")
            font.pointSize: fontsize(12)
            horizontalPadding: 10
            onTriggered: layout.currentIndex = 3
        }

        MenuItem {
            text: qsTr("Scripts")
            font.pointSize: fontsize(12)
            horizontalPadding: 10
            onTriggered: layout.currentIndex = 4
        }
    }
}
