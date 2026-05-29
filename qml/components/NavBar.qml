import QtQuick
import QtQuick.Layouts
import TypeBoost

Rectangle {
    id: root
    color: Theme.surface

    signal navigate(string page)
    property string currentPage: "home"

    readonly property var navItems: [
        { page: "home",     icon: "⌂",  label: qsTr("Главная")   },
        { page: "practice", icon: "⌨",  label: qsTr("Практика")  },
        { page: "progress", icon: "◢", label: qsTr("Прогресс")  },
        { page: "settings", icon: "⚙",  label: qsTr("Настройки") },
    ]

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            height: 72
            Text {
                anchors.centerIn: parent
                text: "TypeBoost"
                font.pixelSize: Theme.fontSizeLarge
                font.bold: true
                color: Theme.primary
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

        Repeater {
            model: root.navItems
            delegate: NavItem {
                Layout.fillWidth: true
                icon:    modelData.icon
                label:   modelData.label
                active:  root.currentPage === modelData.page
                onClicked: root.navigate(modelData.page)
            }
        }

        Item { Layout.fillHeight: true }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

        Text {
            Layout.fillWidth: true
            text: "v0.1.0"
            color: Theme.textSecondary
            font.pixelSize: Theme.fontSizeSmall
            opacity: 0.5
            horizontalAlignment: Text.AlignHCenter
            padding: Theme.spacingM
        }
    }
}
