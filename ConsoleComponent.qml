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
    enabled: main_viewer.v2.checked

    ColumnLayout {
        spacing: 5
        Layout.margins: 10
        Layout.alignment: Qt.AlignTop

        RowLayout {
            Label {
                font.bold: true
                font.styleName: "Bold"
                text: "Console:"
            }

            Item {
                width: 20
            }

            Button {
                text: "Clear"

                onClicked: {
                    bot_console.text = ""
                }
            }
        }

        Rectangle {
            ScrollView {
                anchors.fill: parent
                property ScrollBar hScrollBar: ScrollBar.horizontal
                property ScrollBar vScrollBar: ScrollBar.vertical
                ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                ScrollBar.vertical.width: 10
                ScrollBar.horizontal.height: 10

                TextArea {
                    id: bot_console
                    objectName: "bot_console"
                    placeholderText: qsTr("Console")
                    textMargin: 10
                    leftPadding: 10
                    rightPadding: 10
                    font.pixelSize: 12
                    color: "white"
                    font.family: ptMonoFont.name
                    textFormat: Text.RichText
                    readOnly: true
                    selectByKeyboard: true
                    selectByMouse: Qt.platform.os !== "ios"
                    clip: true

                    onTextChanged: {
                        cursorPosition = length
                    }

                    function add(text) {
                        bot_console.insert(bot_console.length, text);
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
                bot_console.update();
            }
        }

        ListModel {
            id: historyModel
        }

        

        Rectangle {
            ListView {
                id: historyList
                anchors.fill: parent
                clip: true
                model: historyModel
                topMargin: 10
                bottomMargin: 10

                ScrollBar.vertical: ScrollBar {
                    anchors.right: parent.right
                    visible: true
                    policy: ScrollBar.AlwaysOn
                    width: 10
                }

                delegate: Component {
                    Item {
                        width: ListView.view.width - 20
                        height: 22

                        Text {
                            text: command;
                            font.pixelSize: 14
                            color: "white"
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                        }

                        MouseArea {
                            anchors.fill: parent

                            onClicked: {
                                historyList.currentIndex = index;
                                cmd.text = historyModel.get(historyList.currentIndex).command;
                            }

                            onDoubleClicked: {
                                historyViewer.visible = false;
                                if (historyModel.get(historyList.count - 1).command != cmd.text) {
                                    historyModel.append({command: cmd.text});
                                    if (historyList.count > 30) {
                                        historyModel.remove(0, 1);
                                    }
                                }
                                cmd.sendClicked(cmd.text);
                                cmd.text = "";
                            }
                        }
                    }
                }
                
                highlight: Rectangle {
                    color: 'grey'
                    radius: 5
                    opacity: 0.7
                }

                // onCurrentItemChanged: console.log(historyList.currentIndex + '/' + historyList.count + ': ' + historyModel.get(historyList.currentIndex).command + ' selected')
            }

            id: historyViewer
            color: "#353637"
            border.color: "#d5d8dc"
            border.width: 1
            Layout.fillWidth: true
            height: 100
            visible: false;
        }

        FocusScope {
            property alias color: cmd.color
            id: cmd_focus
            x: cmd.x
            y: cmd.y
            height: cmd.height
            Layout.fillWidth: true

            TextField {
                id: cmd
                objectName: "Cmd"
                text: ""
                focus: layout.currentIndex == 5
                activeFocusOnPress: true
                selectByMouse: Qt.platform.os !== "ios"
                width: parent.width
                font.pixelSize: 14
                placeholderText: qsTr("Command")
                inputMethodHints: Qt.ImhPreferLowercase | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText

                onAccepted: {
                    if (historyViewer.visible) {
                        historyViewer.visible = false;
                    }
                    if (historyList.count == 0 || historyModel.get(historyList.count - 1).command != cmd.text) {
                        if (cmd.text != "") {
                            historyModel.append({command: cmd.text});
                            if (historyList.count > 30) {
                                historyModel.remove(0, 1);
                            }
                        }
                    }
                    sendClicked(cmd.text);
                    cmd.text = "";
                }

                Keys.onUpPressed: {
                    if (historyViewer.visible) {
                        historyList.decrementCurrentIndex();
                        cmd.text = historyModel.get(historyList.currentIndex).command;
                    } else if (cmd.text == "" && historyList.count != 0) {
                        historyList.currentIndex = historyList.count - 1;
                        historyList.positionViewAtIndex(historyList.count - 1, ListView.Beginning);
                        historyViewer.visible = true;
                        cmd.text = historyModel.get(historyList.currentIndex).command;
                    }
                }

                Keys.onDownPressed: {
                    if (historyViewer.visible) {
                        if (historyList.currentIndex == historyList.count - 1) {
                            historyViewer.visible = false;
                            cmd.text = "";
                        } else {
                            historyList.incrementCurrentIndex();
                            cmd.text = historyModel.get(historyList.currentIndex).command;
                        }
                    }
                }

                signal sendClicked(string cmd)
            }
        }
    }
}
