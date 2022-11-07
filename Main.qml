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
import QtQuick.Controls.Material
import QtQuick.Layouts


ApplicationWindow {
    FontLoader { id: defaultFont; source: "qrc:/fonts/Roboto-Regular.ttf" }
    FontLoader { id: defaultFontBold; source: "qrc:/fonts/Roboto-Bold.ttf" }
    FontLoader { id: defaultFontItalic; source: "qrc:/fonts/Roboto-Italic.ttf" }
    FontLoader { id: ptMonoFont; source: "qrc:/fonts/PTMono-Regular.ttf" }

    id: applicationWindow
    visible: true
    height: 670
    minimumHeight: 670
    // maximumHeight: 670
    width: 666
    minimumWidth: 666
    // maximumWidth: 666
    font: defaultFont.font
    title: "ct-Bot Remote Viewer 0.6.5"

    Material.theme: Material.Dark
    Material.accent: Material.Green

    menuBar: MenuComponent {
        id: menubar
    }

    StackLayout {
        id: layout
        anchors.fill: parent
        currentIndex: tabbar.currentIndex
        focus: true

        MainComponent {
            id: main_viewer
        }

        LogComponent {}
        
        RemoteCallComponent {}    

        MapComponent {}

        ScriptComponent {}

        ConsoleComponent {}
    }

    footer: TabComponent {
        id: tabbar
    }
}
