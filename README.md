# c't-Bot remote viewer

## Summary

This is a very basic remote viewer for the [c't-Bot][ctBot] based on the [Qt widget toolkit][qt]. It is licensed under the terms of the [GPLv3 license](LICENSE.md).

### Notice

Consider this as experimental code. **If it breaks, you get to keep both pieces.**

## Setup using pre-build binaries

1. Download release for Linux, macOS or Windows [here][release]
1. Prepare and launch...
    * for Linux as [AppImage](AppImage):
        1. Create a local application directory, if not existing: `mkdir ~/Applications`
        1. Move downloaded application to app directory: `mv ~/Downloads/ct-Bot_Viewer_*-x86_64.AppImage ~/Applications/`
        1. Launch the application: `ct-Bot_Viewer_X-x86_64.AppImage` by double-clicking.
    * for Linux:
        1. Create a directory: `mkdir ctbot-viewer`
        1. Change to directory: `cd ctbot-viewer`
        1. Extract the archive there: `tar xjf ../ctbot-viewer.tbz2`
        1. Launch the application: `./ctbot-viewer`
    * for macOS:
        1. Open `ctbot-viewer.dmg`
        1. Launch the application `ctbot-viewer` from there by right-clicking on the icon, then select **open** and confirm with **open** (you might need to do this twice, because the app is currently built as an unsigned one).
    * for Windows:
        1. Extract the archive `ctbot-viewer.zip` to a folder.
        1. Launch the application `ctbot-viewer.exe` by double-clicking.

## Build from source

### Using Qt Creator IDE

Open `ctbot-viewer.pro` with [Qt Creator][qt-creator] and build the project. You will need at least Qt version 5.12.

### Using make on command-line

1. Create directory for build: `mkdir build`
1. Change to directory for build: `cd build`
1. Create/update makefile: `qmake ..`
1. Build with makefile: `make`

[ctBot]: https://www.ct-bot.de
[release]: https://github.com/tsandmann/ctbot-viewer/releases
[qt]: https://en.wikipedia.org/wiki/Qt_(software)
[AppImage]: https://appimage.org
[qt-creator]: https://de.wikipedia.org/wiki/Qt_Creator
