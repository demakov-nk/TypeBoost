import QtQuick
import TypeBoost

Rectangle {
    id: root
    property string icon
    property string label
    property bool   active: false
    signal clicked

    height: 52
    color: active ? Theme.primary : (hovered ? Theme.surfaceVariant : "transparent")
    radius: Theme.radiusS

    property bool hovered: false

    Behavior on color { ColorAnimation { duration: Theme.durationFast } }

    Row {
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: Theme.spacingM
            right: parent.right
            rightMargin: Theme.spacingM
        }
        spacing: Theme.spacingS

        Text {
            text: root.icon
            font.pixelSize: 20
            color: root.active ? Theme.textOnPrimary : Theme.textSecondary
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            text: root.label
            font.pixelSize: Theme.fontSizeBody
            color: root.active ? Theme.textOnPrimary : Theme.textSecondary
            font.bold: root.active
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered:  root.hovered = true
        onExited:   root.hovered = false
        onClicked:  root.clicked()
        cursorShape: Qt.PointingHandCursor
    }
}
