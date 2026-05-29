import QtQuick
import QtQuick.Layouts
import TypeBoost

Item {
    signal navigate(string page)

    Item {
        anchors.centerIn: parent
        width: 560
        height: contentCol.implicitHeight

        ColumnLayout {
            id: contentCol
            width: parent.width
            spacing: Theme.spacingL

            Text {
                Layout.fillWidth: true
                text: qsTr("Добро пожаловать в TypeBoost")
                font.pixelSize: Theme.fontSizeDisplay
                font.bold: true
                color: Theme.textPrimary
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Text {
                Layout.fillWidth: true
                text: qsTr("Улучшите скорость печати и научитесь работать\nбез взгляда на клавиатуру")
                font.pixelSize: Theme.fontSizeLarge
                color: Theme.textSecondary
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Row {
                Layout.alignment: Qt.AlignHCenter
                spacing: Theme.spacingM

                ActionButton {
                    text: qsTr("Практиковаться")
                    onClicked: navigate("practice")
                }
            }

            // Quick stats strip
            Rectangle {
                Layout.fillWidth: true
                height: 90
                color: Theme.surface
                radius: Theme.radiusM

                Row {
                    anchors.fill: parent

                    StatTile {
                        width: parent.width / 3
                        height: parent.height
                        label: qsTr("Сегодня")
                        value: {
                            var s = statisticsEngine.summaryLastDays(1)
                            return Math.round(s.totalMinutes) + " " + qsTr("мин")
                        }
                    }

                    Rectangle { width: 1; height: parent.height - 20; anchors.verticalCenter: parent.verticalCenter; color: Theme.divider }

                    StatTile {
                        width: parent.width / 3 - 2
                        height: parent.height
                        label: qsTr("Ср. скорость (7 дн)")
                        value: {
                            var s = statisticsEngine.summaryLastDays(7)
                            return Math.round(s.avgWpm) + " WPM"
                        }
                    }

                    Rectangle { width: 1; height: parent.height - 20; anchors.verticalCenter: parent.verticalCenter; color: Theme.divider }

                    StatTile {
                        width: parent.width / 3 - 2
                        height: parent.height
                        label: qsTr("Ошибки (7 дн)")
                        value: {
                            var s = statisticsEngine.summaryLastDays(7)
                            return Math.round(s.avgErrorRate * 10) / 10 + "%"
                        }
                    }
                }
            }
        }
    }
}
