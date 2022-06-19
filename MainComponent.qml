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

GridLayout {
    columns: 3
    rows: 3

    property alias v1 : radio_v1
    property alias v2 : radio_v2
    property alias hostname : hostname
    property alias shutdown_button : shutdown_button
    property alias port : port

    ColumnLayout {
        spacing: 2
        Layout.margins: 10
        Layout.bottomMargin: 0
        Layout.alignment: Qt.AlignTop

        Label {
            font.bold: true
            font.styleName: "Bold"
            text: "Sensors:"
        }

        SensorViewer {}
    }

    ColumnLayout {
        spacing: 2
        Layout.margins: 10
        Layout.alignment: Qt.AlignTop

        ColumnLayout {
            id: actuator_viewer
            visible: false
            spacing: 2

            Label {
                id: actuator_viewer_headline
                font.bold: true
                font.styleName: "Bold"
                text: "Actuators:"
            }

            ActuatorViewer {}

            Item {
                height: 24
            }
        }

        ColumnLayout {
            id: connection_box
            visible: true

            Label {
                font.bold: true
                font.styleName: "Bold"
                text: "ct-Bot connection:"
            }

            RowLayout {
                spacing: 2
                Layout.margins: 2

                TextField {
                    id: hostname
                    objectName: "Hostname"
                    text: "ctbot"
                    focus: true
                    selectByMouse: Qt.platform.os !== "ios"
                    Layout.preferredWidth: 150
                    font.pixelSize: 14
                    placeholderText: qsTr("Hostname")
                    onAccepted: connectClicked(hostname.text, port.text);

                    signal connectClicked(string hostname, string port)
                    property int version: radio_v1.checked ? 1 : 2

                    function connected(msg) {
                        hostname.enabled = false;
                        port.enabled = false;
                        radio_v1.enabled = false;
                        radio_v2.enabled = false;
                        shutdown_button.enabled = true;
                        menubar.connect_menu.text = button.text = qsTr("Disconnect");
                        menubar.shutdown_menu.enabled = true;
                        button.font.bold = false;
                        button.font.styleName = "Regular";
                        actuator_viewer.visible = true;
                        connection_box.visible = false;
                    }

                    function disconnected(msg) {
                        hostname.enabled = true;
                        port.enabled = true;
                        radio_v1.enabled = true;
                        radio_v2.enabled = true;
                        shutdown_button.enabled = false;
                        menubar.connect_menu.text = button.text = qsTr("Connect");
                        menubar.shutdown_menu.enabled = false;
                        button.font.bold = true;
                        button.font.styleName = "Bold";
                        actuator_viewer.visible = false;
                        connection_box.visible = true;
                    }
                }

                Item {
                    width: 20
                }

                TextField {
                    id: port
                    objectName: "Port"
                    text: radio_v1.checked ? "10002" : "23"
                    selectByMouse: Qt.platform.os !== "ios"
                    focus: true
                    Layout.preferredWidth: 65
                    font.pixelSize: 14
                    placeholderText: qsTr("Port")
                    inputMethodHints: Qt.ImhDigitsOnly

                    onAccepted: hostname.connectClicked(hostname.text, port.text);
                }
            }

            RowLayout {
                RadioButton {
                    id: radio_v1
                    checked: true
                    text: qsTr("ct-Bot")
                }

                RadioButton {
                    id: radio_v2
                    text: qsTr("ct-Bot v2")
                }
            }
        }

        ColumnLayout {
            RowLayout {
                Button {
                    id: button
                    text: qsTr("Connect")
                    font.bold: true
                    font.styleName: "Bold"
                    Layout.preferredWidth: 110

                    onClicked: {
                        hostname.connectClicked(hostname.text, port.text);
                    }
                }

                Item {
                    width: 10
                }

                Button {
                    id: shutdown_button
                    objectName: "ShutdownButton"
                    text: qsTr("Shutdown")
                    font.bold: false
                    font.styleName: "Regular"
                    Layout.preferredWidth: 110
                    enabled: false

                    signal shutdownClicked()

                    onClicked: {
                        shutdownClicked();
                    }
                }
            }
        }

        Item {
            width: 250
            height: 17
        }

        Label {
            visible: radio_v1.checked ? true : false
            font.bold: true
            font.styleName: "Bold"
            text: "LCD:"
        }

        Rectangle {
            visible: radio_v1.checked ? true : false
            color: "#cdff25"

            TextEdit {
                id: lcd
                objectName: "LCD"
                text: ""
                textMargin: 10
                font.pixelSize: 19
                font.family: ptMonoFont.name
                readOnly: true
                selectByMouse: true
            }

            width: 250
            height: 110
            border.color: "#d5d8dc"
            border.width: 1
        }
    }

    ColumnLayout {
        spacing: 2
        Layout.margins: 10
        Layout.bottomMargin: 0
        Layout.alignment: Qt.AlignTop
        Layout.rowSpan: 3

        Label {
            font.bold: true
            font.styleName: "Bold"
            text: "Remote Control:"
        }

        RemoteControl {}
    }

    Item {
        Layout.margins: 0
        height: 1
    }

    ColumnLayout {
        spacing: 2
        Layout.margins: 10
        Layout.bottomMargin: 0
        Layout.alignment: Qt.AlignBottom
        Layout.columnSpan: 2

        MiniLogComponent {}
    }
}
