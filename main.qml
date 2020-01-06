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
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12


ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 615
    height: 605
    title: qsTr("ct-Bot Remote Viewer 0.1")

    menuBar: MenuBar {
        Menu {
            title: qsTr("Viewer")

            MenuItem {
                id: connectMenu
                text: qsTr("Connect")
                onTriggered: hostname.connectClicked(hostname.text);
            }

            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }

        Menu {
            title: qsTr("Components")

            MenuItem {
                text: qsTr("Main")
                onTriggered: swipeView.currentIndex = 0
            }

            MenuItem {
                text: qsTr("Remote-Calls")
                onTriggered: swipeView.currentIndex = 1
            }

            MenuItem {
                text: qsTr("Logs")
                onTriggered: swipeView.currentIndex = 2
            }

            MenuItem {
                text: qsTr("Map")
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
                    font.pointSize: 16
                    font.bold: true
                    text: "Sensors:"
                }

                SensorViewer {}

                Item {
                    height: 20
                }

                Label {
                    font.pointSize: 16
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
                    font.pointSize: 16
                    font.bold: true
                    text: "ct-Bot connection:"
                }

                TextField {
                    id: hostname
                    objectName: "Hostname"
                    focus: true
                    Layout.preferredWidth: 200
                    placeholderText: qsTr("Hostname")
                    onAccepted: connectClicked(text);

                    signal connectClicked(msg: string)

                    function connected(msg: string) {
                        enabled = false;
                        connectMenu.text = button.text = qsTr("Disconnect")
                        button.font.bold = false
                    }

                    function disconnected(msg: string) {
                        hostname.enabled = true;
                        connectMenu.text = button.text = qsTr("Connect")
                        button.font.bold = true
                    }
                }

                Item {
                    height: 5
                }

                ColumnLayout {
                    RowLayout {
                        RadioButton {
                            checked: true
                            font.pointSize: 24
                            leftPadding: -10
                            indicator.height: 32
                            indicator.width: 32
                            scale: 0.6
                            text: qsTr("v 1")
                        }

                        RadioButton {
                            id: radio_v2
                            font.pointSize: 24
                            leftPadding: -10
                            indicator.height: 32
                            indicator.width: 32
                            scale: 0.6
                            text: qsTr("v 2")
                        }
                    }

                    Button {
                        id: button
                        text: qsTr("Connect")
                        font.bold: true
                        font.pointSize: 16
                        Layout.preferredWidth: 125

                        onClicked: {
                            hostname.connectClicked(hostname.text);
                        }
                    }
                }

                Item {
                    height: 30
                }

                Label {
                    font.pointSize: 16
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
                        font.pointSize: Qt.platform.os == "ios" ? 12 : 16
                        font.family: "courier"
                        readOnly: true
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
                    font.pointSize: 16
                    font.bold: true
                    text: "Viewer:"
                }

                GridLayout {
                    columns: 2
                    objectName: "Viewer"

                    signal connectClicked(msg: string)

                    Button {
                        text: qsTr("Logs")
                        font.pointSize: 16
                        Layout.preferredWidth: 97

                        onClicked: {
                            swipeView.currentIndex = 2
                        }
                    }

                    Button {
                        text: qsTr("Map")
                        font.pointSize: 16
                        Layout.preferredWidth: 97

                        onClicked: {
                            swipeView.currentIndex = 3
                        }
                    }

                    Button {
                        text: qsTr("Remote-Calls")
                        font.pointSize: 16
                        Layout.columnSpan: 2
                        Layout.preferredWidth: 200

                        onClicked: {
                            swipeView.currentIndex = 1
                        }
                    }

                    Button {
                        id: button_console
                        text: qsTr("Console (v2 only)")
                        font.pointSize: 16
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
                    font.pointSize: 16
                    font.bold: true
                    text: "Remote Control:"
                }

                RemoteControl {}
            }
        }

        RowLayout {
            // Remote-Calls
        }

        RowLayout {
            // Logs
        }

        RowLayout {
            // Map
        }

        RowLayout {
            // Console
            enabled: radio_v2.checked
        }
    }

    footer: TabBar {
        id: tabBar
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
