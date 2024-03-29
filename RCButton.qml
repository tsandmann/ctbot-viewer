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

Button {
    id: rcbutton
    Layout.preferredWidth: 35
    Layout.preferredHeight: 35

    onClicked: {
        parent.rcButtonClicked(text, "");
    }

    contentItem: Text {
        text: rcbutton.text
        font: rcbutton.font
        opacity: enabled ? 1.0 : 0.3
        color: rcbutton.down ? "#ff0000" : "#000000"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3
        border.color: rcbutton.down ? "#161674" : "#2C2DE9"
        border.width: 1
        radius: 4
    }
}
