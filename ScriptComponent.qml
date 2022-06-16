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
import QtQuick.Dialogs

RowLayout {
    ColumnLayout {
        spacing: 5
        Layout.margins: 10
        Layout.alignment: Qt.AlignTop

        id: script_viewer
        objectName: "script_viewer"

        signal scriptLoad(string filename)
        signal scriptSave(string filename)
        signal scriptAbort()
        signal scriptSend()

        FileDialog {
            id: scriptLoadDialog
            fileMode: FileDialog.OpenFile
            defaultSuffix: "txt"
            nameFilters: [ "Script files (*.txt)", "All files (*)" ]

            onAccepted: {
                script_viewer.scriptLoad(scriptLoadDialog.selectedFile);
            }
        }

        FileDialog {
            id: scriptSaveDialog
            fileMode: FileDialog.SaveFile
            defaultSuffix: "txt"
            nameFilters: [ "Script files (*.txt)", "All files (*)" ]

            onAccepted: {
                script_viewer.scriptSave(scriptSaveDialog.selectedFile);
            }
        }

        RowLayout {

            Label {
                font.bold: true
                font.styleName: "Bold"
                text: "ct-Bot Script:"
                Layout.preferredWidth: 90
            }

            Item {
                width: 20
            }

            TextField {
                id: script_remote_filename
                objectName: "script_remote_filename"
                text: "prog1.txt"
                focus: true
                selectByMouse: Qt.platform.os !== "ios"
                Layout.preferredWidth: 205
                placeholderText: qsTr("Remote filename")

                onAccepted: {
                    script_viewer.scriptSend();
                }
            }

            Button {
                text: "Send to bot"

                onClicked: {
                    script_viewer.scriptSend();
                }
            }

            CheckBox {
                id: script_execute
                objectName: "script_execute"
                checked: true
                text: qsTr("Execute")
            }
        }

        RowLayout {
            RadioButton {
                id: scripts_abl
                objectName: "scripts_abl"
                checked: true
                text: qsTr("ABL")
            }

            RadioButton {
                id: scripts_basic
                text: qsTr("Basic")
            }

            Button {
                text: "Load"

                onClicked: {
                    scriptLoadDialog.open();
                }
            }

            Button {
                text: "Save"

                onClicked: {
                    scriptSaveDialog.open();
                }
            }

            Button {
                text: "Abort"

                onClicked: {
                    script_viewer.scriptAbort();
                }
            }
        }

        Rectangle {
            ScrollView {
                anchors.fill: parent
                id: script_scrollview
                property ScrollBar hScrollBar: ScrollBar.horizontal
                property ScrollBar vScrollBar: ScrollBar.vertical

                TextArea {
                    id: script_editor
                    objectName: "script_editor"
                    placeholderText: qsTr("Enter your ct-Bot script here")
                    textMargin: 10
                    leftPadding: 10
                    rightPadding: 10
                    font.pixelSize: 15
                    font.family: ptMonoFont.name
                    color: "white"
                    readOnly: false
                    selectByMouse: true
                    clip: true
                    tabStopDistance: 16

                    background: Rectangle {
                        color: "#353637"
                        border.color: "#d5d8dc"
                        border.width: 1
                    }

                }
            }

            border.color: "#d5d8dc"
            border.width: 1
            Layout.alignment: Qt.AlignTop
            Layout.fillHeight: true
            Layout.fillWidth: true

            onHeightChanged: {
                script_editor.update();
            }
        }
    }
}
