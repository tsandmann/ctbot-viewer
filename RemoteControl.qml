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

Frame {
    background: Rectangle {
        color: "transparent"
        border.color: "#d5d8dc"
        border.width: 1
    }

    GridLayout {
        id: grid
        objectName: "RCButton"
        columns: 3

        signal rcButtonClicked(string msg1, string msg2)

        Item {
            width: 35
        }

        RCButton {
            id: rc_power
            text: "Power"
            Layout.preferredWidth: 75
            Layout.columnSpan: 2

            ToolTip.visible: hovered
            ToolTip.text: "Esc/space"
        }

        Repeater {
            id: numbers
            model: 12

            RCButton {
                text: index + 1

                ToolTip.visible: index < 10 ? hovered : false
                ToolTip.text: index < 9 ? index + 1 : 0
            }
        }

        RCButton {
            text: "Gr"

            contentItem: Rectangle {
                color: parent.down ? "#08D508" : "#058B05"
                opacity: enabled ? 1 : 0.3
                border.width: 0
                radius: 4
            }
        }

        RCButton {
            text: "Re"

            contentItem: Rectangle {
                color: parent.down ? "#ff0000" : "#9F0404"
                opacity: enabled ? 1 : 0.3
                border.width: 0
                radius: 4
            }
        }

        RCButton {
            text: "Ye"

            contentItem: Rectangle {
                color: parent.down ? "#F8E849" : "#ECD709"
                opacity: enabled ? 1 : 0.3
                border.width: 0
                radius: 4
            }
        }

        RCButton {
            text: "Bl"

            contentItem: Rectangle {
                color: parent.down ? "#0078FF" : "#0000CE"
                opacity: enabled ? 1 : 0.3
                border.width: 0
                radius: 4
            }
        }

        RCButton {
            text: "I/II"
            padding: 0
        }

        RCButton {
            text: "TV"
        }

        Item {
            width: 35
        }

        RCButton {
            id: rc_up
            text: "||"

            ToolTip.visible: hovered
            ToolTip.text: "w"
        }

        Item {
            width: 35
        }

        RCButton {
            id: rc_left
            text: "<<"

            ToolTip.visible: hovered
            ToolTip.text: "a"
        }

        RCButton {
            text: "\u25B6"
        }

        RCButton {
            id: rc_right
            text: ">>"

            ToolTip.visible: hovered
            ToolTip.text: "d"
        }

        Item {
            width: 35
        }


        RCButton {
            id: rc_down
            text: "\u2B1B"

            ToolTip.visible: hovered
            ToolTip.text: "s"
        }

        Item {
            width: 35
        }

        RCButton {
            text: "\u25CF"
        }

        RCButton {
            text: "CH*P/C"
            Layout.preferredWidth: 75
            Layout.columnSpan: 2
        }

        RCButton {
            text: "V+"
        }

        RCButton {
            text: "M"
        }

        RCButton {
            text: "C+"
        }

        RCButton {
            text: "V-"
        }

        Item {
            width: 35
        }

        RCButton {
            text: "C-"
        }
    }

    Shortcut {
        sequences: ["Esc", "Space"]
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log('Esc/space pressed: ' + rc_power.text);
            grid.rcButtonClicked(rc_power.text, "");
        }
    }

    Shortcut {
        sequence: "w"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log('w pressed: ' + rc_up.text);
            grid.rcButtonClicked(rc_up.text, "");
        }
    }

    Shortcut {
        sequence: "a"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log('a pressed: ' + rc_left.text);
            grid.rcButtonClicked(rc_left.text, "");
        }
    }

    Shortcut {
        sequence: "s"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log('s pressed: ' + rc_down.text);
            grid.rcButtonClicked(rc_down.text, "");
        }
    }

    Shortcut {
        sequence: "d"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log('d pressed: ' + rc_right.text);
            grid.rcButtonClicked(rc_right.text, "");
        }
    }

    Shortcut {
        sequence: "1"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log(sequence + ' pressed');
            grid.rcButtonClicked(sequence, "");
        }
    }

    Shortcut {
        sequence: "2"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log(sequence + ' pressed');
            grid.rcButtonClicked(sequence, "");
        }
    }

    Shortcut {
        sequence: "3"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log(sequence + ' pressed');
            grid.rcButtonClicked(sequence, "");
        }
    }

    Shortcut {
        sequence: "4"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log(sequence + ' pressed');
            grid.rcButtonClicked(sequence, "");
        }
    }

    Shortcut {
        sequence: "5"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log(sequence + ' pressed');
            grid.rcButtonClicked(sequence, "");
        }
    }

    Shortcut {
        sequence: "6"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log(sequence + ' pressed');
            grid.rcButtonClicked(sequence, "");
        }
    }

    Shortcut {
        sequence: "7"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log(sequence + ' pressed');
            grid.rcButtonClicked(sequence, "");
        }
    }

    Shortcut {
        sequence: "8"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log(sequence + ' pressed');
            grid.rcButtonClicked(sequence, "");
        }
    }

    Shortcut {
        sequence: "9"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log(sequence + ' pressed');
            grid.rcButtonClicked(sequence, "");
        }
    }

    Shortcut {
        sequence: "0"
        enabled: layout.currentIndex == 0
        onActivated: {
            // console.log('10 pressed');
            grid.rcButtonClicked("10", "");
        }
    }
}
