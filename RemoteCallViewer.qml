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

import RemoteCalls 1.0

Frame {
    id: remotecall_viewer
    objectName: "RemoteCallViewer"

    background: Rectangle {
        color: "#353637"
        border.color: "#d5d8dc"
        border.width: 1
        radius: 2
    }

    signal remoteCallClicked(string name, string parameter)

    ListView {
        implicitHeight: contentHeight > applicationWindow.height - 240 ? applicationWindow.height - 240 : contentHeight
        implicitWidth: applicationWindow.minimumWidth - 44

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
                    font.bold: true
                    font.styleName: "Bold"
                }

                RowLayout {
                    spacing: 0

                    ColumnLayout {
                        Label {
                            id: rc_par_name_1
                            Layout.alignment: Qt.AlignLeft
                            text: model.parameter1
                        }
                        SpinBox {
                            id: rc_par_1
                            from: -32768
                            to: 32767
                            value: 0
                            editable: true
                            implicitWidth: 150
                            visible: rc_par_name_1.text !== ""

                            background: Rectangle {
                                color: "transparent"
                                border.color: "#d5d8dc"
                                border.width: 1
                                radius: 4
                            }
                        }
                    }

                    Item {
                        width: 20
                        visible: rc_par_name_1.text !== ""
                    }

                    ColumnLayout {
                        Label {
                            id: rc_par_name_2
                            Layout.alignment: Qt.AlignLeft
                            text: model.parameter2
                        }
                        SpinBox {
                            id: rc_par_2
                            from: -32768
                            to: 32767
                            value: 0
                            editable: true
                            implicitWidth: 150
                            visible: rc_par_name_2.text !== ""

                            background: Rectangle {
                                color: "transparent"
                                border.color: "#d5d8dc"
                                border.width: 1
                                radius: 4
                            }
                        }
                    }

                    Item {
                        width: 20
                        visible: rc_par_name_2.text !== ""
                    }

                    ColumnLayout {
                        Label {
                            id: rc_par_name_3
                            Layout.alignment: Qt.AlignLeft
                            text: model.parameter3
                        }

                        SpinBox {
                            id: rc_par_3
                            from: -32768
                            to: 32767
                            value: 0
                            editable: true
                            implicitWidth: 150
                            visible: rc_par_name_3.text !== ""

                            background: Rectangle {
                                color: "transparent"
                                border.color: "#d5d8dc"
                                border.width: 1
                                radius: 4
                            }
                        }
                    }

                    Item {
                        width: 20
                        visible: rc_par_name_3.text !== ""
                    }

                    ColumnLayout {
                        Label {}

                        Button {
                            text: "Run"
                            font.bold: true
                            font.styleName: "Bold"
                            implicitWidth: 60

                            onClicked: {
                                remotecall_viewer.remoteCallClicked(rc_name.text, rc_par_1.value + "," + rc_par_2.value + "," + rc_par_3.value);
                            }
                        }
                    }
                }

                Item {
                    height: 10
                }
            }
        }
    }
}
