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
import QtQuick.Dialogs 1.3

RowLayout {
    Item {
        width: 10
    }

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
            selectExisting: true
            defaultSuffix: "txt"
            nameFilters: [ "Script files (*.txt)", "All files (*)" ]

            onAccepted: {
                script_viewer.scriptLoad(scriptLoadDialog.fileUrl);
            }
        }

        FileDialog {
            id: scriptSaveDialog
            selectExisting: false
            defaultSuffix: "txt"
            nameFilters: [ "Script files (*.txt)", "All files (*)" ]

            onAccepted: {
                script_viewer.scriptSave(scriptSaveDialog.fileUrl);
            }
        }

        RowLayout {

            Label {
                font.pointSize: fontsize(12)
                font.bold: true
                text: "ct-Bot Script"
                Layout.preferredWidth: 90
            }

            Item {
                width: 40
            }

            TextField {
                id: script_remote_filename
                objectName: "script_remote_filename"
                text: "prog1.txt"
                focus: true
                Layout.preferredHeight: 25
                Layout.preferredWidth: 205
                placeholderText: qsTr("Remote filename")
                font.pointSize: fontsize(10)
                onAccepted: {
                    script_viewer.scriptSend();
                }
            }

            Button {
                text: "Send to bot"
                font.pointSize: applicationWindow.fontsize(12)
                implicitHeight: 25

                onClicked: {
                    script_viewer.scriptSend();
                }
            }

            CheckBox {
                id: script_execute
                objectName: "script_execute"
                checked: true
                font.pointSize: fontsize(16)
                leftPadding: -10
                indicator.height: 32
                indicator.width: 32
                scale: 0.7
                text: qsTr("Execute")
            }
        }

        RowLayout {
            RadioButton {
                id: scripts_abl
                objectName: "scripts_abl"
                checked: true
                font.pointSize: fontsize(16)
                leftPadding: -15
                rightPadding: -10
                indicator.height: 32
                indicator.width: 32
                scale: 0.6
                text: qsTr("ABL")
            }

            RadioButton {
                id: scripts_basic
                font.pointSize: fontsize(16)
                leftPadding: -10
                rightPadding: -3
                indicator.height: 32
                indicator.width: 32
                scale: 0.6
                text: qsTr("Basic")
            }

            Button {
                text: "Load"
                font.pointSize: applicationWindow.fontsize(12)
                implicitHeight: 25

                onClicked: {
                    scriptLoadDialog.open();
                }
            }

            Button {
                text: "Save"
                font.pointSize: applicationWindow.fontsize(12)
                implicitHeight: 25

                onClicked: {
                    scriptSaveDialog.open();
                }
            }

            Button {
                text: "Abort"
                font.pointSize: applicationWindow.fontsize(12)
                implicitHeight: 25

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
                    textMargin: 4
                    font.pointSize: fontsize(10)
                    font.family: "courier"
                    readOnly: false
                    selectByMouse: true
                    clip: true
                    //onTextChanged: cursorPosition = length
                }
            }

            width: 570
            implicitHeight: applicationWindow.height - 185
            border.color: "gray"
            border.width: 2
            Layout.alignment: Qt.AlignTop
        }
    }
}
