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

Frame {
    GridLayout {
        id: grid
        objectName: "RCButton"
        columns: 3

        signal connectClicked(string msg1, string msg2)

        Item {
            width: 35
        }

        RCButton {
            text: qsTr("Power")
            Layout.preferredWidth: 75
            Layout.columnSpan: 2
        }

        Repeater {
            id: numbers
            model: 12

            RCButton {
                text: index + 1
            }
        }

        RCButton {
            text: qsTr("Gr")

            contentItem: Rectangle {
                color: parent.down ? "#08D508" : "#058B05"
                opacity: enabled ? 1 : 0.3
                border.width: 0
                radius: 4
            }
        }

        RCButton {
            text: qsTr("Re")

            contentItem: Rectangle {
                color: parent.down ? "#ff0000" : "#9F0404"
                opacity: enabled ? 1 : 0.3
                border.width: 0
                radius: 4
            }
        }

        RCButton {
            text: qsTr("Ye")

            contentItem: Rectangle {
                color: parent.down ? "#F8E849" : "#ECD709"
                opacity: enabled ? 1 : 0.3
                border.width: 0
                radius: 4
            }
        }

        RCButton {
            text: qsTr("Bl")

            contentItem: Rectangle {
                color: parent.down ? "#0078FF" : "#0000CE"
                opacity: enabled ? 1 : 0.3
                border.width: 0
                radius: 4
            }
        }

        RCButton {
            text: qsTr("I/II")
        }

        RCButton {
            text: qsTr("TV")
        }

        Repeater {
            model: 3

            Item {
                width: 35
                height: 25
            }
        }

        Item {
            width: 35
        }

        RCButton {
            text: qsTr("^")
        }

        Item {
            width: 35
        }

        RCButton {
            text: qsTr("<<")
        }

        RCButton {
            text: qsTr(">")
        }

        RCButton {
            text: qsTr(">>")
        }

        Item {
            width: 35
        }

        RCButton {
            text: qsTr("v")
        }

        Item {
            width: 35
        }

        Repeater {
            model: 3

            Item {
                width: 35
                height: 25
            }
        }

        RCButton {
            text: qsTr("*")
        }

        RCButton {
            text: qsTr("CH*P/P")
            Layout.preferredWidth: 75
            Layout.columnSpan: 2
        }

        Repeater {
            model: 3

            Item {
                width: 35
                height: 10
            }
        }

        RCButton {
            text: qsTr("V+")
        }

        RCButton {
            text: qsTr("M")
        }

        RCButton {
            text: qsTr("C+")
        }

        RCButton {
            text: qsTr("V-")
        }

        Item {
            width: 35
        }

        RCButton {
            text: qsTr("C-")
        }
    }
}
