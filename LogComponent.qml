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
    ColumnLayout {
        spacing: 5
        Layout.margins: 10
        Layout.alignment: Qt.AlignTop

        RowLayout {
            Label {
                font.bold: true
                font.styleName: "Bold"
                text: "Log:"
            }

            Item {
                width: 20
            }

            Button {
                text: "Clear"

                onClicked: {
                    log_viewer.text = ""
                }
            }
        }

        Rectangle {
            ScrollView {
                anchors.fill: parent
                property ScrollBar hScrollBar: ScrollBar.horizontal
                property ScrollBar vScrollBar: ScrollBar.vertical

                TextArea {
                    id: log_viewer
                    objectName: "log_viewer"
                    placeholderText: qsTr("Log")
                    textMargin: 10
                    leftPadding: 10
                    rightPadding: 10
                    font.pixelSize: 15
                    color: "white"
                    font.family: ptMonoFont.name
                    readOnly: true
                    selectByKeyboard: true
                    selectByMouse: Qt.platform.os !== "ios"
                    clip: true

                    onTextChanged: {
                        cursorPosition = length
                    }

                    function add(text) {
                        log_viewer.insert(log_viewer.length, text);
                    }

                    background: Rectangle {
                        color: "#353637"
                        border.color: "#d5d8dc"
                        border.width: 1
                    }
                }
            }

            Layout.fillHeight: true
            Layout.fillWidth: true

            onHeightChanged: {
                log_viewer.update();
            }
        }
    }
}
