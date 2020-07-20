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
    property alias v2 : radio_v2
    property alias hostname : hostname
    property alias port : port

    FontLoader { id: ptMonoFont; source: "qrc:/fonts/PTMono-Regular.ttf" }

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
            height: 30
        }

        Label {
            font.pointSize: fontsize(12)
            font.bold: true
            text: "Actuators:"
        }

        ActuatorViewer {}
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
                selectByMouse: Qt.platform.os != "ios"
                Layout.preferredWidth: 142
                placeholderText: qsTr("Hostname")
                font.pointSize: fontsize(12)
                onAccepted: connectClicked(hostname.text, port.text);

                signal connectClicked(string hostname, string port)

                function connected(msg) {
                    hostname.enabled = false;
                    port.enabled = false;
                    radio_v1.enabled = false;
                    radio_v2.enabled = false;
                    menubar.connect_menu.text = button.text = qsTr("Disconnect")
                    button.font.bold = false
                }

                function disconnected(msg) {
                    hostname.enabled = true;
                    port.enabled = true;
                    radio_v1.enabled = true;
                    radio_v2.enabled = true;
                    menubar.connect_menu.text = button.text = qsTr("Connect")
                    button.font.bold = true
                }
            }

            TextField {
                id: port
                objectName: "Port"
                text: radio_v1.checked ? "10002" : "23"
                selectByMouse: Qt.platform.os != "ios"
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
                    font.pointSize: fontsize(15)
                    leftPadding: -10
                    indicator.height: 32
                    indicator.width: 32
                    scale: 0.7
                    text: qsTr("ct-Bot")
                }

                RadioButton {
                    id: radio_v2
                    font.pointSize: fontsize(15)
                    leftPadding: -10
                    indicator.height: 32
                    indicator.width: 32
                    scale: 0.7
                    text: qsTr("ct-Bot v2")
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
                font.family: ptMonoFont.name
                readOnly: true
                selectByMouse: true
            }

            width: 212
            height: 80
            border.color: "gray"
            border.width: 1
        }

        Item {
            height: 25
        }

        MiniLogComponent {}
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
