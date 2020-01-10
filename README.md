# c't-Bot remote viewer

## Summary

This is a very basic remote viewer for the [c't-Bot][ctBot] based on the [Qt widget toolkit][qt]. It is licensed under the terms of the [GPLv3 license](LICENSE.md).

### Notice

Consider this as experimental code. **If it breaks, you get to keep both pieces.**

## Setup using pre-build binaries

1. Download release for Linux or macOS [here][release]
1. Prepare and launch...
    * for Linux:
        1. Create a directory: `mkdir ctbot-viewer`
        1. Change to directory: `cd ctbot-viewer`
        1. Extract the archive there: `tar xjf ../ctbot-viwer.tbz2`
        1. Launch the application: `./ctbot-viewer`
    * for macOS:
        1. Open `ctbot-viewer.dmg`
        1. Launch the application `ctbot-viewer` from there by right-clicking on the icon, then select **open** and confirm with **open** (you might need to do this twice, because the app is currently built as an unsigned one).

## Build from source

TBD

[ctBot]: https://www.ct-bot.de
[release]: https://github.com/tsandmann/ctbot-viewer/releases
[qt]: https://en.wikipedia.org/wiki/Qt_(software)
