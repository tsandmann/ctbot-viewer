/*
 * This file is part of the ct-Bot remote viewer tool.
 * Copyright (c) 2020-2022 Timo Sandmann
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

import QtQuick
import QtQuick.Controls

MenuBar {
    property alias connect_menu : connect
    property alias shutdown_menu : shutdown

    width: parent.width

    Menu {
        title: qsTr("Viewer")

        MenuItem {
            id: connect
            text: qsTr("Connect")
            horizontalPadding: 10

            onTriggered: {
                main_viewer.hostname.connectClicked(main_viewer.hostname.text, main_viewer.port.text);
            }
        }

        MenuItem {
            id: shutdown
            text: qsTr("Shutdown")
            horizontalPadding: 10
            enabled: false

            onTriggered: {
                main_viewer.shutdown_button.shutdownClicked("", "");
            }
        }

        MenuItem {
            text: qsTr("Exit")
            horizontalPadding: 10

            onTriggered: {
                Qt.quit();
            }
        }
    }

    Menu {
        title: qsTr("Components")

        MenuItem {
            text: qsTr("Main")
            horizontalPadding: 10

            ToolTip.visible: hovered
            ToolTip.text: "Strg + 1"

            onTriggered: layout.currentIndex = 0
        }

        MenuItem {
            text: qsTr("Log")
            horizontalPadding: 10

            ToolTip.visible: hovered
            ToolTip.text: "Strg + 2"

            onTriggered: layout.currentIndex = 1
        }

        MenuItem {
            text: qsTr("Remote Calls")
            horizontalPadding: 10

            ToolTip.visible: main_viewer.v1.checked ? hovered : false
            ToolTip.text: "Strg + 3"

            onTriggered: layout.currentIndex = 2
            enabled: main_viewer.v1.checked
        }

        MenuItem {
            text: qsTr("Map")
            horizontalPadding: 10

            ToolTip.visible: main_viewer.v1.checked ? hovered : false
            ToolTip.text: "Strg + 4"

            onTriggered: layout.currentIndex = 3
            enabled: main_viewer.v1.checked
        }

        MenuItem {
            text: qsTr("Scripts")
            horizontalPadding: 10

            ToolTip.visible: main_viewer.v1.checked ? hovered : false
            ToolTip.text: "Strg + 5"

            onTriggered: layout.currentIndex = 4
            enabled: main_viewer.v1.checked
        }

        MenuItem {
            text: qsTr("Console")
            horizontalPadding: 10

            ToolTip.visible: main_viewer.v2.checked ? hovered : false
            ToolTip.text: "Strg + 6"

            onTriggered: layout.currentIndex = 5
            enabled: main_viewer.v2.checked
        }
    }
}
