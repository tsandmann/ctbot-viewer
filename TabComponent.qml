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

TabBar {
    currentIndex: layout.currentIndex

    TabButton {
        text: qsTr("Main")
        id: main_tab
        onClicked: layout.currentIndex = 0

        ToolTip.visible: hovered
        ToolTip.text: "Strg + 1"
    }

    TabButton {
        text: qsTr("Log")
        onClicked: layout.currentIndex = 1

        ToolTip.visible: hovered
        ToolTip.text: "Strg + 2"
    }

    TabButton {
        text: qsTr("Remote Calls")
//      width: Math.max(contentItem.implicitWidth + leftPadding + rightPadding, main_tab.width)
        width: contentItem.implicitWidth + leftPadding + rightPadding
        onClicked: layout.currentIndex = 2
        enabled: main_viewer.v1.checked

        ToolTip.visible: main_viewer.v1.checked ? hovered : false
        ToolTip.text: "Strg + 3"
    }

    TabButton {
        text: qsTr("Map")
        onClicked: layout.currentIndex = 3
        enabled: main_viewer.v1.checked

        ToolTip.visible: main_viewer.v1.checked ? hovered : false
        ToolTip.text: "Strg + 4"
    }

    TabButton {
        text: qsTr("Scripts")
        onClicked: layout.currentIndex = 4
        enabled: main_viewer.v1.checked

        ToolTip.visible: main_viewer.v1.checked ? hovered : false
        ToolTip.text: "Strg + 5"
    }

    TabButton {
        text: qsTr("Console")
        onClicked: layout.currentIndex = 5
        enabled: main_viewer.v2.checked

        ToolTip.visible: main_viewer.v2.checked ? hovered : false
        ToolTip.text: "Strg + 6"
    }
}
