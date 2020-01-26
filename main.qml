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


ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 615
    height: 605
    title: qsTr("ct-Bot Remote Viewer 0.2")

    function fontsize(size) {
        if (Qt.platform.os == "osx") {
            return size * 4 / 3
        }

        return size * 0.8
    }

    menuBar: MenuBar {
        font.pointSize: fontsize(12)

        Menu {
            title: qsTr("Viewer")

            MenuItem {
                id: connectMenu
                text: qsTr("Connect")
                font.pointSize: fontsize(12)
                onTriggered: hostname.connectClicked(hostname.text);
            }

            MenuItem {
                text: qsTr("Exit")
                font.pointSize: fontsize(12)
                onTriggered: Qt.quit();
            }
        }

        Menu {
            title: qsTr("Components")

            MenuItem {
                text: qsTr("Main")
                font.pointSize: fontsize(12)
                onTriggered: swipeView.currentIndex = 0
            }

            MenuItem {
                text: qsTr("Remote-Calls")
                font.pointSize: fontsize(12)
                onTriggered: swipeView.currentIndex = 1
            }

            MenuItem {
                text: qsTr("Logs")
                font.pointSize: fontsize(12)
                onTriggered: swipeView.currentIndex = 2
            }

            MenuItem {
                text: qsTr("Map")
                font.pointSize: fontsize(12)
                onTriggered: swipeView.currentIndex = 3
            }
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        RowLayout {
            // Main

            ColumnLayout {
                spacing: 2
                Layout.margins: 10
                Layout.alignment: "Qt::AlignTop"

                Label {
                    font.pointSize: fontsize(12)
                    font.bold: true
                    text: "Sensors:"
                }

                SensorViewer {}

                Item {
                    height: 20
                }

                Label {
                    font.pointSize: fontsize(12)
                    font.bold: true
                    text: "Actors:"
                }

                ActorViewer {}
            }

            ColumnLayout {
                spacing: 2
                Layout.margins: 10
                Layout.alignment: "Qt::AlignTop"

                Label {
                    font.pointSize: fontsize(12)
                    font.bold: true
                    text: "ct-Bot connection:"
                }

                RowLayout {
                    spacing: 2
                    Layout.margins: 2

                    TextField {
                        id: hostname
                        objectName: "Hostname"
                        focus: true
                        Layout.preferredWidth: 135
                        placeholderText: qsTr("Hostname")
                        font.pointSize: fontsize(12)
                        onAccepted: connectClicked(hostname.text, port.text);

                        signal connectClicked(string hostname, string port)

                        function connected(msg) {
                            hostname.enabled = false;
                            port.enabled = false;
                            radio_v1.enabled = false;
                            radio_v2.enabled = false;
                            connectMenu.text = button.text = qsTr("Disconnect")
                            button.font.bold = false
                        }

                        function disconnected(msg) {
                            hostname.enabled = true;
                            port.enabled = true;
                            radio_v1.enabled = true;
                            radio_v2.enabled = true;
                            connectMenu.text = button.text = qsTr("Connect")
                            button.font.bold = true
                        }
                    }

                    TextField {
                        id: port
                        objectName: "Port"
                        text: radio_v1.checked ? "10002" : "23"
                        focus: true
                        Layout.preferredWidth: 65
                        placeholderText: qsTr("Port")
                        font.pointSize: fontsize(12)
                        onAccepted: hostname.connectClicked(hostname.text, port.text);
                    }
                }

                Item {
                    height: 5
                }

                ColumnLayout {
                    RowLayout {
                        RadioButton {
                            id: radio_v1
                            checked: true
                            font.pointSize: fontsize(16)
                            leftPadding: -10
                            indicator.height: 32
                            indicator.width: 32
                            scale: 0.7
                            text: qsTr("v 1")
                        }

                        RadioButton {
                            id: radio_v2
                            font.pointSize: fontsize(16)
                            leftPadding: -10
                            indicator.height: 32
                            indicator.width: 32
                            scale: 0.7
                            text: qsTr("v 2")
                        }
                    }

                    Button {
                        id: button
                        text: qsTr("Connect")
                        font.bold: true
                        font.pointSize: fontsize(12)
                        Layout.preferredWidth: 125

                        onClicked: {
                            hostname.connectClicked(hostname.text, port.text);
                        }
                    }
                }

                Item {
                    height: 30
                }

                Label {
                    font.pointSize: fontsize(12)
                    font.bold: true
                    text: "LCD:"
                }


                Rectangle {
                    color: "#CDFF25"

                    TextEdit {
                        id: lcd
                        objectName: "LCD"
                        text: ""
                        textMargin: 4
                        font.pointSize: fontsize(12)
                        font.family: "courier"
                        readOnly: true
                        selectByMouse: true
                    }

                    width: 200
                    height: 70
                    border.color: "gray"
                    border.width: 1
                }

                Item {
                    height: 30
                }

                Label {
                    font.pointSize: fontsize(12)
                    font.bold: true
                    text: "Viewer:"
                }

                GridLayout {
                    columns: 2
                    objectName: "Viewer"

                    signal connectClicked(string msg)

                    Button {
                        text: qsTr("Logs")
                        font.pointSize: fontsize(12)
                        Layout.preferredWidth: 97

                        onClicked: {
                            swipeView.currentIndex = 2
                        }
                    }

                    Button {
                        text: qsTr("Map")
                        font.pointSize: fontsize(12)
                        Layout.preferredWidth: 97
                        enabled: false

                        onClicked: {
                            swipeView.currentIndex = 3
                        }
                    }

                    Button {
                        text: qsTr("Remote-Calls")
                        font.pointSize: fontsize(12)
                        Layout.columnSpan: 2
                        Layout.preferredWidth: 200
                        enabled: false

                        onClicked: {
                            swipeView.currentIndex = 1
                        }
                    }

                    Button {
                        id: button_console
                        text: qsTr("Console (v2 only)")
                        font.pointSize: fontsize(12)
                        Layout.columnSpan: 2
                        Layout.preferredWidth: 200
                        enabled: radio_v2.checked

                        onClicked: {
                            swipeView.currentIndex = 4
                        }
                    }
                }
            }

            ColumnLayout {
                spacing: 2
                Layout.margins: 10
                Layout.alignment: "Qt::AlignTop"

                Label {
                    font.pointSize: fontsize(12)
                    font.bold: true
                    text: "Remote Control:"
                }

                RemoteControl {}
            }
        }

        RowLayout {
            // Remote-Calls
            Label {
                text: "not implemented."
                font.pointSize: 20
                leftPadding: 100
            }
        }

        RowLayout {
            Item {
                width: 30
            }

            ColumnLayout {
                // Logs
                spacing: 5
                Layout.margins: 10
                Layout.alignment: "Qt::AlignTop"

                Label {
                    font.pointSize: fontsize(12)
                    font.bold: true
                    text: "Log:"
                }

                Rectangle {
                    ScrollView {
                        anchors.fill: parent
                        id: text_scrollview
                        property ScrollBar hScrollBar: ScrollBar.horizontal
                        property ScrollBar vScrollBar: ScrollBar.vertical

                        TextArea {
                            id: log_viewer
                            objectName: "log_viewer"
                            placeholderText: qsTr("Log")
                            textMargin: 4
                            font.pointSize: fontsize(10)
                            font.family: "courier"
                            readOnly: true
                            selectByMouse: true
                            clip: true;
                            onTextChanged: cursorPosition = length;
                        }
                    }

                    width: 500
                    height: 450
                    border.color: "gray"
                    border.width: 2
                    Layout.alignment: "Qt::AlignTop"
                }
            }
        }

        RowLayout {
            // Map
            Label {
                text: "not implemented."
                font.pointSize: 20
                leftPadding: 100
            }
        }

        RowLayout {
            // Console
            enabled: radio_v2.checked

            Label {
                text: "not implemented."
                font.pointSize: 20
                leftPadding: 100
            }
        }
    }

    footer: TabBar {
        id: tabBar
        font.pointSize: fontsize(12)
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Main")
            onClicked: swipeView.currentIndex = 0
        }

        TabButton {
            text: qsTr("Remote-Calls")
            onClicked: swipeView.currentIndex = 1
        }

        TabButton {
            text: qsTr("Logs")
            onClicked: swipeView.currentIndex = 2
        }

        TabButton {
            text: qsTr("Map")
            onClicked: swipeView.currentIndex = 3
        }

        TabButton {
            text: qsTr("Console")
            onClicked: swipeView.currentIndex = 4
            enabled: radio_v2.checked
        }
    }
}
