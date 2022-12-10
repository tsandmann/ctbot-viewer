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
import QtQuick.Layouts

import Sensors 1.0

Frame {
    background: Rectangle {
        color: "transparent"
        border.color: "#d5d8dc"
        border.width: 1
    }

    RowLayout {
        spacing: 0
        Layout.margins: 0

        ListView {
            implicitWidth: 180
            implicitHeight: contentHeight
            clip: true
            model: sensorModel
            visible: main_viewer.v1.checked

            delegate: RowLayout {
                width: ListView.view.width
                spacing: 2

                Label {
                    Layout.alignment: Qt.AlignLeft
                    text: model.name
                }

                Label {
                    Layout.alignment: Qt.AlignRight
                    text: model.value
                }
            }
        }

        ListView {
            implicitWidth: 180
            implicitHeight: contentHeight
            spacing: 5
            clip: true
            model: sensorModelV2
            visible: main_viewer.v2.checked

            delegate: RowLayout {
                width: ListView.view.width
                spacing: 2

                Label {
                    Layout.alignment: Qt.AlignLeft
                    text: model.name
                }

                Label {
                    Layout.alignment: Qt.AlignRight
                    text: model.value
                }
            }
        }
    }
}
