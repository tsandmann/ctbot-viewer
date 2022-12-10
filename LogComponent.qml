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

// FIXME: check box for enable autoscrolling

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

            Item {
                width: 20
            }

            CheckBox {
                id: autoscroll
                checked: true
                text: qsTr("Auto scroll")
            }
        }

        Rectangle {
            ScrollView {
                id: scrollview
                anchors.fill: parent
                property ScrollBar hScrollBar: ScrollBar.horizontal
                property ScrollBar vScrollBar: ScrollBar.vertical
                ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                ScrollBar.vertical.width: 10
                ScrollBar.horizontal.height: 10

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

                    function add(text) {
                        var old_pos = scrollview.contentItem.contentY;
                        log_viewer.insert(log_viewer.length, text);
                        if (!autoscroll.checked) {
                            scrollview.contentItem.contentY = old_pos;
                        }
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
