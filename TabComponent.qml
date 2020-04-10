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

TabBar {
    font.pointSize: fontsize(11)
    width: 615
    currentIndex: layout.currentIndex

    TabButton {
        text: qsTr("Main")
        onClicked: layout.currentIndex = 0
    }

    TabButton {
        text: qsTr("Remote Calls")
        onClicked: layout.currentIndex = 1
    }

    TabButton {
        text: qsTr("Logs")
        onClicked: layout.currentIndex = 2
    }

    TabButton {
        text: qsTr("Map")
        onClicked: layout.currentIndex = 3
    }

    TabButton {
        text: qsTr("Scripts")
        onClicked: layout.currentIndex = 4
    }

    TabButton {
        text: qsTr("Console")
        onClicked: layout.currentIndex = 5
        enabled: main_viewer.v2.checked
    }
}
