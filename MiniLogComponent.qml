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

RowLayout {
    ColumnLayout {
        spacing: 3
        Layout.margins: 0
        Layout.alignment: Qt.AlignTop

        RowLayout {
            Label {
                font.pointSize: fontsize(12)
                font.bold: true
                text: "Log:"
            }
        }

        Rectangle {
            ScrollView {
                anchors.fill: parent
                property ScrollBar hScrollBar: ScrollBar.horizontal
                property ScrollBar vScrollBar: ScrollBar.vertical

                TextArea {
                    id: mini_log_viewer
                    objectName: "mini_log_viewer"
                    placeholderText: qsTr("Log")
                    textMargin: 3
                    leftPadding: 3
                    rightPadding: 3
                    topPadding: 3
                    bottomPadding: 3
                    font.pointSize: fontsize(9)
                    font.family: "courier"
                    readOnly: true
                    selectByKeyboard: true
                    selectByMouse: Qt.platform.os != "ios"
                    clip: true

                    MouseArea {
                        anchors.fill: parent
                        onDoubleClicked: {
                            mini_log_viewer.text = ""
                        }
                    }
                }
            }

            border.color: "gray"
            border.width: 1
            implicitHeight: 165
            Layout.fillWidth: true
            onWidthChanged: {
                mini_log_viewer.update();
            }
        }
    }
}
