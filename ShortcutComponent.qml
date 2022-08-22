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

Item {
    id: tabview
    property int currentIndex

    function showIndex(index) {
        tabview.currentIndex = index;
        layout.currentIndex = tabview.currentIndex; 
    }

    Shortcut {
        sequence: "Ctrl+1"
        onActivated: layout.currentIndex = 0
    }

    Shortcut {
        sequence: "Ctrl+2"
        onActivated: layout.currentIndex = 1
    }

    Shortcut {
        sequence: "Ctrl+3"
        enabled: main_viewer.v1.checked
        onActivated: layout.currentIndex = 2
    }

    Shortcut {
        sequence: "Ctrl+4"
        enabled: main_viewer.v1.checked
        onActivated: layout.currentIndex = 3
    }

    Shortcut {
        sequence: "Ctrl+5"
        enabled: main_viewer.v1.checked
        onActivated: layout.currentIndex = 4
    }

    Shortcut {
        sequence: "Ctrl+6"
        enabled: main_viewer.v2.checked
        onActivated: layout.currentIndex = 5
    }

    Shortcut {
        sequence: "Ctrl+right"
        enabled: layout.currentIndex < (main_viewer.v1.checked ? 4 : 5)
        onActivated: {
            layout.currentIndex++;
            
            if (main_viewer.v2.checked && layout.currentIndex == 2) {
                layout.currentIndex = 5;
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+left"
        enabled: layout.currentIndex > 0
        onActivated: {
            layout.currentIndex--;
            
            if (main_viewer.v2.checked && layout.currentIndex == 4) {
                layout.currentIndex = 1;
            }
        }
    }
}