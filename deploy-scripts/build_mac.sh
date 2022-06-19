#!/bin/sh

BUILDDIR=$(greadlink -f $1)
/Users/ts/usr/qt/6.3.1/macos/bin/macdeployqt $BUILDDIR/ctbot-viewer.app -qmldir=$BUILDDIR/../ -always-overwrite -verbose=1 -dmg

hdiutil convert -format UDRW -ov -o $BUILDDIR/tmp.dmg $BUILDDIR/ctbot-viewer.dmg
hdiutil mount $BUILDDIR/tmp.dmg
diskutil rename /Volumes/*ctbot-viewer* ctbot-viewer
hdiutil detach /Volumes/*ctbot-viewer*
hdiutil convert -format UDZO -ov -o $BUILDDIR/ctbot-viewer.dmg $BUILDDIR/tmp.dmg
rm $BUILDDIR/tmp.dmg

