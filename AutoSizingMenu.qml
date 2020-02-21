import QtQuick 2.10
import QtQuick.Controls 2.3

Menu {
    width: {
        var result = 0;
        var padding = 0;
        for (var i = 0; i < count; ++i) {
            var item = itemAt(i);
            result = Math.max(item.contentItem.implicitWidth, result);
            padding = Math.max(item.horizontalPadding, padding);
        }

        return result + padding * 2;
    }
}
