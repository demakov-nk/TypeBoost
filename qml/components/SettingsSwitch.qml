import QtQuick
import QtQuick.Controls
import TypeBoost

Switch {
    id: control
    Behavior on opacity { NumberAnimation { duration: Theme.durationFast } }

    indicator: Rectangle {
        width: 44
        height: 24
        radius: 12
        color: control.checked ? Theme.primary : Theme.surfaceVariant
        anchors.verticalCenter: parent.verticalCenter

        Behavior on color { ColorAnimation { duration: Theme.durationFast } }

        Rectangle {
            width: 18
            height: 18
            radius: 9
            anchors.verticalCenter: parent.verticalCenter
            x: control.checked ? parent.width - width - 3 : 3
            color: "#FFFFFF"

            Behavior on x { NumberAnimation { duration: Theme.durationFast } }
        }
    }

    contentItem: Text {
        leftPadding: control.indicator.width + control.spacing
        text: control.text
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSizeBody
        color: Theme.textPrimary
        verticalAlignment: Text.AlignVCenter
    }
}
