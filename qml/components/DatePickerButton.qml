import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import TypeBoost 1.0
import "../styles"

Item {
    id: root

    property date selectedDate: new Date()
    signal dateSelected(date d)

    implicitWidth: 140
    implicitHeight: 36

    // ── Кнопка ────────────────────────────────────────────────────────────────
    Rectangle {
        anchors.fill: parent
        radius: Theme.radiusS
        color: Theme.surface
        border.color: popup.opened ? Theme.primary : Theme.divider
        border.width: 1

        RowLayout {
            anchors { fill: parent; margins: 8 }
            spacing: 6

            Text {
                text: ""
                font.pixelSize: 14
            }
            Text {
                Layout.fillWidth: true
                text: Qt.formatDate(root.selectedDate, "dd.MM.yyyy")
                color: Theme.textPrimary
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeBody
                verticalAlignment: Text.AlignVCenter
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: popup.opened ? popup.close() : popup.open()
        }
    }

    // ── Попап с календарём ────────────────────────────────────────────────────
    Popup {
        id: popup
        y: root.height + 4
        width: 280
        padding: 0
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            color: Theme.surfaceVariant
            radius: Theme.radiusM
            border.color: Theme.divider
            border.width: 1
        }

        ColumnLayout {
            width: parent.width
            spacing: 0

            // Шапка — месяц + навигация
            RowLayout {
                Layout.fillWidth: true
                Layout.margins: 8
                spacing: 4

                RoundButton {
                    text: "‹"
                    flat: true
                    onClicked: { if (calendar.month > 0) calendar.month--; else { calendar.year--; calendar.month = 11 } }
                    contentItem: Text {
                        text: parent.text
                        color: Theme.textPrimary
                        font.pixelSize: 18
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle { color: "transparent" }
                }

                Text {
                    Layout.fillWidth: true
                    text: Qt.locale("ru_RU").standaloneMonthName(calendar.month) +
                        " " + calendar.year
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeBody
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                }

                RoundButton {
                    text: "›"
                    flat: true
                    onClicked: { if (calendar.month < 11) calendar.month++; else { calendar.year++; calendar.month = 0 } }
                    contentItem: Text {
                        text: parent.text
                        color: Theme.textPrimary
                        font.pixelSize: 18
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle { color: "transparent" }
                }
            }

            // Дни недели
            Row {
                Layout.fillWidth: true
                Layout.leftMargin: 8
                Layout.rightMargin: 8
                Repeater {
                    model: ["Пн","Вт","Ср","Чт","Пт","Сб","Вс"]
                    delegate: Text {
                        width: (280 - 16) / 7
                        text: modelData
                        color: Theme.textSecondary
                        font.pixelSize: Theme.fontSizeSmall
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

            // Сетка дат
            MonthGrid {
                id: calendar
                Layout.fillWidth: true
                Layout.margins: 8
                locale: Qt.locale("ru_RU")

                delegate: Rectangle {
                    width: (280 - 16) / 7
                    height: 34
                    radius: Theme.radiusS
                    color: {
                        if (model.day === root.selectedDate.getDate() &&
                            model.month === root.selectedDate.getMonth() &&
                            model.year === root.selectedDate.getFullYear())
                            return Theme.primary
                        if (model.month !== calendar.month) return "transparent"
                        return "transparent"
                    }


                    Text {
                        anchors.centerIn: parent
                        text: model.day
                        color: {
                            if (model.day === root.selectedDate.getDate() &&
                                model.month === root.selectedDate.getMonth() &&
                                model.year === root.selectedDate.getFullYear())
                                return Theme.textOnPrimary
                            if (model.month !== calendar.month)
                                return Theme.divider
                            return Theme.textPrimary
                        }
                        font.pixelSize: Theme.fontSizeBody
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (model.month !== calendar.month) return
                            root.selectedDate = new Date(model.year, model.month, model.day)
                            root.dateSelected(root.selectedDate)
                            popup.close()
                        }
                    }
                }
            }

            // Кнопка "Сегодня"
            Rectangle {
                Layout.fillWidth: true
                height: 36
                color: Theme.surface
                radius: Theme.radiusM

                Text {
                    anchors.centerIn: parent
                    text: qsTr("Сегодня")
                    color: Theme.primary
                    font.pixelSize: Theme.fontSizeBody
                    font.bold: true
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        root.selectedDate = new Date()
                        root.dateSelected(root.selectedDate)
                        popup.close()
                    }
                }
            }
        }
    }
}