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
                font.bold: true
                font.styleName: "Bold"
                text: "Remote Calls:"
            }

            Item {
                width: 40
            }

            Button {
                text: "Fetch List"

                onClicked: {
                    parent.remoteCallClear();
                    parent.remoteCallFetch();
                }
            }

            Button {
                text: "Clear List"

                onClicked: {
                    parent.remoteCallClear();
                }
            }

            Button {
                id: remotecall_abort
                text: "Abort"

                onClicked: {
                    parent.remoteCallAbort();
                }
            }
        }

        RemoteCallViewer {
            id: remotecall_viewer
            visible: remotecall_viewer.implicitHeight < 100 ? false : true
        }

        Label {
            text: "No remote call list received."
            visible: remotecall_viewer.implicitHeight < 100 ? true : false
        }

        Item {
            height: 10
        }

        Label {
            objectName: "CurrentRemoteCallLabel"
            font.bold: true
            font.styleName: "Bold"
            text: "Active Remote Call: none"
            visible: remotecall_viewer.implicitHeight < 100 ? false : true
        }
    }
}
