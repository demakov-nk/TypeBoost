import QtQuick
import TypeBoost

Rectangle {
    id: root
    property string text
    property bool   primary: false
    signal clicked

    width: 200
    height: 48
    radius: Theme.radiusM
    color: primary ? Theme.primary : Theme.surface
    border.color: primary ? "transparent" : Theme.primary
    border.width: 1

    property bool hovered: false
    opacity: hovered ? 0.85 : 1.0
    Behavior on opacity { NumberAnimation { duration: Theme.durationFast } }

    Text {
        anchors.centerIn: parent
        text: root.text
        color: root.primary ? Theme.textOnPrimary : Theme.primary
        font.pixelSize: Theme.fontSizeBody
        font.bold: true
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
