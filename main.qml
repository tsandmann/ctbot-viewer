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
import mapimage 1.0


ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 615
    height: 605
    title: qsTr("ct-Bot Remote Viewer 0.3")

    function fontsize(size) {
        if (Qt.platform.os == "osx" || Qt.platform.os == "ios") {
            return size * 4 / 3
        }

        return size * 0.8
    }

    menuBar: MenuBar {
        font.pointSize: fontsize(12)
        width: parent.width

        AutoSizingMenu {
            title: qsTr("Viewer")

            MenuItem {
                id: connectMenu
                text: qsTr("Connect")
                font.pointSize: fontsize(12)
                horizontalPadding: 10
                onTriggered: hostname.connectClicked(hostname.text);
            }

            MenuItem {
                text: qsTr("Exit")
                font.pointSize: fontsize(12)
                horizontalPadding: 10
                onTriggered: Qt.quit();
            }
        }

        AutoSizingMenu {
            title: qsTr("Components")

            MenuItem {
                text: qsTr("Main")
                font.pointSize: fontsize(12)
                horizontalPadding: 10
                onTriggered: layout.currentIndex = 0
            }

            MenuItem {
                text: qsTr("Remote Calls")
                font.pointSize: fontsize(12)
                horizontalPadding: 10
                onTriggered: layout.currentIndex = 1
            }

            MenuItem {
                text: qsTr("Logs")
                font.pointSize: fontsize(12)
                horizontalPadding: 10
                onTriggered: layout.currentIndex = 2
            }

            MenuItem {
                text: qsTr("Map")
                font.pointSize: fontsize(12)
                horizontalPadding: 10
                onTriggered: layout.currentIndex = 3
            }
        }
    }

    StackLayout {
        id: layout
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        focus: true

        RowLayout {
            // Main

            ColumnLayout {
                spacing: 2
                Layout.margins: 10
                Layout.alignment: Qt.AlignTop

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
                Layout.alignment: Qt.AlignTop

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

                        onClicked: layout.currentIndex = 2
                    }

                    Button {
                        text: qsTr("Map")
                        font.pointSize: fontsize(12)
                        Layout.preferredWidth: 97

                        onClicked: layout.currentIndex = 3
                    }

                    Button {
                        text: qsTr("Remote Calls")
                        font.pointSize: fontsize(12)
                        Layout.columnSpan: 2
                        Layout.preferredWidth: 200

                        onClicked: layout.currentIndex = 1
                    }

                    Button {
                        id: button_console
                        text: qsTr("Console (v2 only)")
                        font.pointSize: fontsize(12)
                        Layout.columnSpan: 2
                        Layout.preferredWidth: 200
                        enabled: radio_v2.checked

                        onClicked: layout.currentIndex = 4
                    }
                }
            }

            ColumnLayout {
                spacing: 2
                Layout.margins: 10
                Layout.alignment: Qt.AlignTop

                Label {
                    font.pointSize: fontsize(12)
                    font.bold: true
                    text: "Remote Control:"
                }

                RemoteControl {}
            }
        }

        RowLayout {
            // Remote Calls

            ColumnLayout {
                spacing: 5
                Layout.margins: 10
                Layout.alignment: Qt.AlignTop

                RowLayout {
                    objectName: "RemoteCallActions"

                    signal remoteCallFetch()
                    signal remoteCallClear()
                    signal remoteCallAbort()

                    Label {
                        font.pointSize: fontsize(12)
                        font.bold: true
                        text: "Remote Calls:"
                    }

                    Item {
                        width: 40
                    }

                    Button {
                        text: "Fetch List"
                        font.pointSize: applicationWindow.fontsize(12)
                        implicitHeight: 25

                        onClicked: {
                            parent.remoteCallClear();
                            parent.remoteCallFetch();
                        }
                    }

                    Button {
                        text: "Clear List"
                        font.pointSize: applicationWindow.fontsize(12)
                        implicitHeight: 25

                        onClicked: {
                            parent.remoteCallClear();
                        }
                    }

                    Button {
                        id: remotecall_abort
                        text: "Abort"
                        font.pointSize: applicationWindow.fontsize(12)
                        implicitHeight: 25

                        onClicked: {
                            parent.remoteCallAbort();
                        }
                    }
                }

                RemoteCallViewer {
                    id: remotecall_viewer
                }

                Label {
                    font.pointSize: fontsize(12)
                    text: "No remote call list received."
                    visible: remotecall_viewer.implicitHeight < 100 ? true : false
                }

                Label {
                    objectName: "CurrentRemoteCallLabel"
                    font.pointSize: fontsize(12)
                    font.bold: true
                    text: "Active Remote Call: none"
                    visible: remotecall_viewer.implicitHeight < 100 ? false : true
                }
            }
        }

        RowLayout {
            Item {
                width: 10
            }

            ColumnLayout {
                // Logs
                spacing: 5
                Layout.margins: 10
                Layout.alignment: Qt.AlignTop

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
                            clip: true
                            onTextChanged: cursorPosition = length
                        }
                    }

                    width: 570
                    implicitHeight: applicationWindow.height - 165
                    border.color: "gray"
                    border.width: 2
                    Layout.alignment: Qt.AlignTop
                }
            }
        }

        RowLayout {
            Item {
                width: 10
            }

            ColumnLayout {
                // Map
                spacing: 5
                Layout.margins: 10
                Layout.alignment: Qt.AlignTop

                Label {
                    font.pointSize: fontsize(12)
                    font.bold: true
                    text: "Map:"
                }

                Rectangle {
                    Flickable {
                        id: map_flickable
                        implicitWidth: applicationWindow.width - 60
                        implicitHeight: applicationWindow.height - 175
                        contentWidth: map.width
                        contentHeight: map.height
                        clip: true
                        anchors.centerIn: parent

                        ScrollBar.horizontal: ScrollBar {
                             policy: ScrollBar.AsNeeded
                             height: 8
                             horizontalPadding: 0
                             verticalPadding: 0
                        }

                        ScrollBar.vertical: ScrollBar {
                             policy: ScrollBar.AsNeeded
                             width: 8
                             horizontalPadding: 0
                             verticalPadding: 0
                        }

                        MapImageItem {
                            id: map
                            objectName: "Map"
                            width: 1536
                            height: 1536
                            rotation: 180

                            function scroll_to(x, y) {
                                map_flickable.contentX = x - map_flickable.implicitWidth / 2;
                                map_flickable.contentY = y - map_flickable.implicitHeight / 2;
                            }
                        }
                    }


                    width: applicationWindow.width - 50
                    height: applicationWindow.height - 165
                    border.color: "gray"
                    border.width: 2
                    Layout.alignment: Qt.AlignCenter
                    color: "gray"
                }
            }
        }

        RowLayout {
            // Console
            enabled: radio_v2.checked

            Label {
                text: "not implemented."
                font.pointSize: fontsize(16)
                padding: 20
                leftPadding: 100
            }
        }
    }

    footer: TabBar {
        id: tabBar
        font.pointSize: fontsize(12)
        width: 615
        currentIndex: layout.currentIndex

        TabButton {
            text: qsTr("Main")
            onClicked: layout.currentIndex = 0
        }

        TabButton {
            text: qsTr("Remote Calls")
            onClicked: layout.currentIndex = 1
        }

        TabButton {
            text: qsTr("Logs")
            onClicked: layout.currentIndex = 2
        }

        TabButton {
            text: qsTr("Map")
            onClicked: layout.currentIndex = 3
        }

        TabButton {
            text: qsTr("Console")
            onClicked: layout.currentIndex = 4
            enabled: radio_v2.checked
        }
    }
}
