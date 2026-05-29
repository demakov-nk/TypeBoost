import QtQuick
import QtQuick.Layouts
import TypeBoost

Item {
    id: root
    property string label
    property string value

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Theme.spacingXs

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: root.value
            font.pixelSize: Theme.fontSizeLarge
            font.bold: true
            color: Theme.primary
        }
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: root.label
            font.pixelSize: Theme.fontSizeSmall
            color: Theme.textSecondary
        }
    }
}
