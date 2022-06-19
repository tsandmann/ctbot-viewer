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
import QtQuick.Dialogs
import Qt.labs.platform

import MapImage 1.0

RowLayout {
    ColumnLayout {
        spacing: 5
        Layout.margins: 10
        Layout.alignment: Qt.AlignTop

        RowLayout {
            id: mapViewer
            objectName: "MapViewer"

            signal mapClear()
            signal mapFetch()
            signal mapSave(string filename)

            FileDialog {
                id: saveFileDialog
                fileMode: FileDialog.SaveFile
                defaultSuffix: "png"
                nameFilters: [ "Image files (*.png)", "All files (*)" ]

                onAccepted: {
                    mapViewer.mapSave(saveFileDialog.selectedFile);
                }
            }

            Label {
                font.bold: true
                font.styleName: "Bold"
                text: "Map:"
            }

            Item {
                width: 40
            }

            Button {
                text: "Fetch"

                onClicked: {
                    parent.mapFetch();
                }
            }

            Button {
                text: "Clear"

                onClicked: {
                    parent.mapClear();
                }
            }

            Button {
                text: "Save to file"

                onClicked: {
                    saveFileDialog.open();
                }
            }
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
                anchors.fill: parent

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

            border.color: "#d5d8dc"
            border.width: 1
            Layout.alignment: Qt.AlignCenter
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "gray"
        }
    }
}
