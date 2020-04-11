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
                font.pointSize: fontsize(12)
                implicitHeight: 25

                onClicked: {
                    parent.remoteCallClear();
                    parent.remoteCallFetch();
                }
            }

            Button {
                text: "Clear List"
                font.pointSize: fontsize(12)
                implicitHeight: 25

                onClicked: {
                    parent.remoteCallClear();
                }
            }

            Button {
                id: remotecall_abort
                text: "Abort"
                font.pointSize: fontsize(12)
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
