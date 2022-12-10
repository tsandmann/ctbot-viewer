/*
 * This file is part of the c't-Bot remote viewer tool.
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

import SystemData 1.0


ColumnLayout {
    spacing: 5
    Layout.margins: 0
    Layout.alignment: Qt.AlignTop
    
    Label {
        font.bold: true
        font.styleName: "Bold"
        text: "CPU Utilization:                      [%]"
    }

    Rectangle {
        color: "transparent"
        border.color: "#d5d8dc"
        border.width: 1
        width: 204
        height: 10
        id: cpu_util_bar
        objectName: "cpu_util_bar"

        Rectangle {
            color: "green"
            border.color: "#d5d8dc"
            border.width: 1
            width: 0
            height: 10
            id: cpu_util
            objectName: "cpu_util"
        }
    }

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
                implicitHeight: 124
                spacing: 5
                clip: true
                model: systemModelV2
                visible: main_viewer.v2.checked

                delegate: RowLayout {
                    width: ListView.view.width
                    spacing: 2
                    visible: model.value >= 0.01
                    height:  model.value >= 0.01 ? 16 : 0

                    Label {
                        Layout.alignment: Qt.AlignLeft
                        text: model.name
                    }

                    Label {
                        Layout.alignment: Qt.AlignRight
                        text: model.value.toFixed(2)
                    }
                }
            }
        }
    }
}
