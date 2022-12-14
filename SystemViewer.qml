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
        id: cpu_util
        objectName: "cpu_util"

        property string toolTipText: ""
        ToolTip.text: toolTipText
        ToolTip.visible: toolTipText ? ma_cpu.containsMouse : false

        MouseArea {
            id: ma_cpu
            anchors.fill: parent
            hoverEnabled: true
        }

        Rectangle {
            color: "green"
            border.color: "#d5d8dc"
            border.width: 1
            width: 0
            height: 10
            id: cpu_util_bar
            objectName: "cpu_util_bar"
        }

        function set_cpu(v) {
            cpu_util_bar.width = v * cpu_util.width / 100.0;
            cpu_util.toolTipText = "CPU load " + v.toFixed(2) + " %";
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
                implicitHeight: 95
                spacing: 4
                clip: true
                model: systemModelV2
                visible: main_viewer.v2.checked

                delegate: RowLayout {
                    width: ListView.view.width
                    spacing: 2
                    visible: model.value >= 0.01 && model.value <= 100.0
                    height:  model.value >= 0.01 && model.value <= 100.0 ? 16 : 0

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

    Item {
        height: 1
    }

    Label {
        font.bold: true
        font.styleName: "Bold"
        text: "RAM Utilization:"
    }

    RowLayout {
        id: ram_util
        objectName: "ram_util"
        spacing: 0
        Layout.margins: 0

        function set_ram1(v1, v2, v3, v4, v5) {
            ram_util_1.width = v2 * ram_util_bar_1.width / v1;
            ram_util_2.width = v3 * ram_util_bar_1.width / v1;
            ram_util_3.width = v4 * ram_util_bar_1.width / v1;
            ram_util_4.width = v5 * ram_util_bar_1.width / v1;

            ram_util_bar_1.toolTipText = "RAM 1 size: " + v1 + " KiB, ITCM: " + v2 + " KiB, data: " + v3 + " KiB, bss: " + v4 + " KiB, heap: " + v5 + " KiB";
        }

        function set_ram2(v1, v2) {
            ram_util_5.width = v2 * ram_util_bar_2.width / v1;

            ram_util_bar_2.toolTipText = "RAM 2 size: " + v1 + " KiB, used: " + v2 + " KiB";
        }

        function set_ram3(v1, v2) {
            ram_util_6.width = v2 * ram_util_bar_3.width / v1;

            ram_util_bar_3.toolTipText = "ext. RAM size: " + v1 + " KiB, used: " + v2 + " KiB";
        }

        Rectangle {
            color: "transparent"
            border.color: "#d5d8dc"
            border.width: 1
            width: 102
            height: 10
            id: ram_util_bar_1
            objectName: "ram_util_bar_1"

            property string toolTipText: ""
            ToolTip.text: toolTipText
            ToolTip.visible: toolTipText ? ma_ram_1.containsMouse : false

            MouseArea {
                id: ma_ram_1
                anchors.fill: parent
                hoverEnabled: true
            }

            Item {
                id: ram_util_top_1
                width: ram_util_bar_1.width
                height: 1
            }

            Item {
                id: ram_util_left_1
                width: 1
            }

            Rectangle {
                color: "steelblue"
                border.color: "#d5d8dc"
                border.width: 0
                anchors.leftMargin: 0
                anchors.left: ram_util_left_1.right
                anchors.top: ram_util_top_1.bottom
                width: 0
                height: 8
                id: ram_util_1
                objectName: "ram_util_1"
            }

            Rectangle {
                color: "blue"
                border.color: "#d5d8dc"
                border.width: 0
                anchors.leftMargin: 0
                anchors.left: ram_util_1.right
                anchors.top: ram_util_top_1.bottom
                width: 0
                height: 8
                id: ram_util_2
                objectName: "ram_util_2"
            }

            Rectangle {
                color: "tomato"
                border.color: "#d5d8dc"
                border.width: 0
                anchors.leftMargin: 0
                anchors.left: ram_util_2.right
                anchors.top: ram_util_top_1.bottom
                width: 0
                height: 8
                id: ram_util_3
                objectName: "ram_util_3"
            }

            Rectangle {
                color: "#9F0404"
                border.color: "#d5d8dc"
                border.width: 0
                anchors.leftMargin: 0
                anchors.left: ram_util_3.right
                anchors.top: ram_util_top_1.bottom
                width: 0
                height: 8
                id: ram_util_4
                objectName: "ram_util_4"
            }
        }

        Rectangle {
            color: "transparent"
            border.color: "#d5d8dc"
            border.width: 1
            width: ram_util_bar_1.width
            height: ram_util_bar_1.height
            id: ram_util_bar_2
            objectName: "ram_util_bar_2"

            property string toolTipText: ""
            ToolTip.text: toolTipText
            ToolTip.visible: toolTipText ? ma_ram_2.containsMouse : false

            MouseArea {
                id: ma_ram_2
                anchors.fill: parent
                hoverEnabled: true
            }

            Item {
                id: ram_util_top_2
                width: ram_util_bar_1.width
                height: 1
            }

            Item {
                id: ram_util_left_2
                width: 1
            }

            Rectangle {
                color: "tomato"
                border.color: "#d5d8dc"
                border.width: 0
                anchors.leftMargin: 0
                anchors.left: ram_util_left_2.right
                anchors.top: ram_util_top_2.bottom
                width: 0
                height: 8
                id: ram_util_5
                objectName: "ram_util_5"
            }
        }
    }

    Rectangle {
        color: "transparent"
        border.color: "#d5d8dc"
        border.width: 1
        width: 204
        height: 10
        id: ram_util_bar_3
        objectName: "ram_util_bar_3"

        property string toolTipText: ""
        ToolTip.text: toolTipText
        ToolTip.visible: toolTipText ? ma_ram_3.containsMouse : false

        MouseArea {
            id: ma_ram_3
            anchors.fill: parent
            hoverEnabled: true
        }

        Item {
            id: ram_util_top_3
            width: ram_util_bar_3.width
            height: 1
        }

        Item {
            id: ram_util_left_3
            width: 1
        }

        Rectangle {
            color: "tomato"
            border.color: "#d5d8dc"
            border.width: 0
            anchors.leftMargin: 0
            anchors.left: ram_util_left_3.right
            anchors.top: ram_util_top_3.bottom
            width: 0
            height: 8
            id: ram_util_6
            objectName: "ram_util_6"
        }
    }

    Item {
        height: 2
    }
}
