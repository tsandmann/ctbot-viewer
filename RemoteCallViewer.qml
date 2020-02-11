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

import RemoteCalls 1.0

Frame {
    id: remotecall_viewer
    objectName: "RemoteCallViewer"
    signal remoteCallClicked(string name, string parameter)

    ListView {
        implicitWidth: 570
        implicitHeight: contentHeight > applicationWindow.height - 180 ? applicationWindow.height - 180 : contentHeight
        anchors.fill: parent
        clip: true
        model: remotecallModel

        delegate: RowLayout {
            width: parent.width
            Layout.margins: 10
            Layout.alignment: Qt.AlignTop

            ColumnLayout {
                Label {
                    id: rc_name
                    Layout.alignment: Qt.AlignLeft
                    text: model.name + "():"
                    font.pointSize: applicationWindow.fontsize(12)
                    font.bold: true
                }

                RowLayout {
                    spacing: 0

                    ColumnLayout {
                        Label {
                            id: rc_par_name_1
                            Layout.alignment: Qt.AlignLeft
                            text: model.parameter1
                            font.pointSize: applicationWindow.fontsize(12)
                        }
                        SpinBox {
                            id: rc_par_1
                            from: -32768
                            to: 32767
                            value: 0
                            editable: true
                            font.pointSize: applicationWindow.fontsize(12)
                            implicitWidth: 150
                            visible: rc_par_name_1.text != ""
                        }
                    }

                    Item {
                        width: 20
                        visible: rc_par_name_1.text != ""
                    }

                    ColumnLayout {
                        Label {
                            id: rc_par_name_2
                            Layout.alignment: Qt.AlignLeft
                            text: model.parameter2
                            font.pointSize: applicationWindow.fontsize(12)
                        }
                        SpinBox {
                            id: rc_par_2
                            from: -32768
                            to: 32767
                            value: 0
                            editable: true
                            font.pointSize: applicationWindow.fontsize(12)
                            implicitWidth: 150
                            visible: rc_par_name_2.text != ""
                        }
                    }

                    Item {
                        width: 20
                        visible: rc_par_name_2.text != ""
                    }

                    ColumnLayout {
                        Label {
                            id: rc_par_name_3
                            Layout.alignment: Qt.AlignLeft
                            text: model.parameter3
                            font.pointSize: applicationWindow.fontsize(12)
                        }
                        SpinBox {
                            id: rc_par_3
                            from: -32768
                            to: 32767
                            value: 0
                            editable: true
                            font.pointSize: applicationWindow.fontsize(12)
                            implicitWidth: 150
                            visible: rc_par_name_3.text != ""
                        }
                    }

                    Item {
                        width: 20
                        visible: rc_par_name_3.text != ""
                    }

                    ColumnLayout {
                        Label {
                            font.pointSize: applicationWindow.fontsize(12)
                        }

                        Button {
                            //  objectName: rc_name.text + "_run"
                            text: "Run"
                            font.pointSize: applicationWindow.fontsize(12)
                            font.bold: true
                            implicitWidth: 60

                            onClicked: {
                                remotecall_viewer.remoteCallClicked(rc_name.text, rc_par_1.value + "," + rc_par_2.value + "," + rc_par_3.value);
                            }
                        }
                    }
                }

                Item {
                    height: 20
                }
            }
        }
    }
}
