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
    FontLoader { id: ptMonoFont; source: "qrc:/fonts/PTMono-Regular.ttf" }

    Item {
        width: 10
    }

    ColumnLayout {
        spacing: 5
        Layout.margins: 10
        Layout.alignment: Qt.AlignTop

        RowLayout {
            Label {
                font.pointSize: fontsize(12)
                font.bold: true
                text: "Log:"
            }

            Item {
                width: 40
            }

            Button {
                text: "Clear"
                font.pointSize: fontsize(12)
                implicitHeight: 25

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
                    textMargin: 4
                    font.pointSize: fontsize(10)
                    font.family: ptMonoFont.name
                    readOnly: true
                    selectByKeyboard: true
                    selectByMouse: Qt.platform.os != "ios"
                    clip: true
                    onTextChanged: {
                        cursorPosition = length
                    }
                }
            }

            width: applicationWindow.minimumWidth - 45
            border.color: "gray"
            border.width: 1
            Layout.fillHeight: true
            onHeightChanged: {
                log_viewer.update();
            }
        }

        Item {
            height: 10
        }
    }
}
