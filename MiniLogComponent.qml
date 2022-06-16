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

RowLayout {
    FontLoader { id: ptMonoFont; source: "qrc:/fonts/PTMono-Regular.ttf" }

    ColumnLayout {
        spacing: 2
        Layout.margins: 0
        Layout.alignment: Qt.AlignTop

        RowLayout {
            Label {
                font.bold: true
                font.styleName: "Bold"
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
                    color: "#ffab91"
                    textMargin: 8
                    leftPadding: 8
                    rightPadding: 8
                    topPadding: 8
                    bottomPadding: 8
                    font.family: ptMonoFont.name
                    font.pixelSize: 11
                    readOnly: true
                    selectByKeyboard: true
                    selectByMouse: Qt.platform.os !== "ios"
                    clip: true

                    function add(text) {
                        mini_log_viewer.insert(mini_log_viewer.length, text);
                    }

                    background: Rectangle {
                        color: "#353637"
                        border.color: "#d5d8dc"
                        border.width: 1
                    }

                    MouseArea {
                        anchors.fill: parent
                        onDoubleClicked: {
                            mini_log_viewer.text = ""
                        }
                    }
                }
            }

            Layout.minimumHeight: 185
            Layout.minimumWidth: 480

            onWidthChanged: {
                mini_log_viewer.update();
            }
        }
    }
}
