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

import Sensors 1.0

Frame {
    ListView {
        implicitWidth: 180
        implicitHeight: contentHeight
        anchors.fill: parent
        clip: true
        model: sensorModel

        delegate: RowLayout {
            width: parent.width
            spacing: 2

            Label {
                Layout.alignment: Qt.AlignLeft
                text: model.name
                font.pointSize: fontsize(12)
            }

            Label {
                Layout.alignment: Qt.AlignRight
                text: model.value
                font.pointSize: fontsize(12)
            }
        }
    }
}
