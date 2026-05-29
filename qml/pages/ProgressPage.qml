import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import TypeBoost 1.0
import "../styles"

Item {
    id: root

    // ── Период ────────────────────────────────────────────────────────────────
    property int  selectedPeriod: 7   // дней, 0 = кастомный
    property date customFrom: new Date(new Date().setDate(new Date().getDate() - 30))
    property date customTo:   new Date()

    property var from: {
        if (selectedPeriod === 0) {
            var d = new Date(customFrom); d.setHours(0,0,0,0); return d;
        }
        var t = new Date(); t.setDate(t.getDate() - selectedPeriod); return t;
    }
    property var to: selectedPeriod === 0 ? customTo : new Date()

    property var summary: statisticsEngine.summaryForRange(
        from.toISOString ? from : new Date(from),
        to.toISOString   ? to   : new Date(to)
    )

    function refreshSummary() {
        summary = statisticsEngine.summaryForRange(from, to)
    }


    onFromChanged: refreshSummary()
    onToChanged:   refreshSummary()

    // ── Форматирование секунд ─────────────────────────────────────────────────
    function formatSeconds(sec) {
        var s = Math.round(sec)
        if (s < 60)   return s + " " + qsTr("сек")
        if (s < 3600) return Math.floor(s/60) + " " + qsTr("мин") + " " + (s%60) + " " + qsTr("сек")
        return Math.floor(s/3600) + " " + qsTr("ч") + " " + Math.floor((s%3600)/60) + " " + qsTr("мин")
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingL
        spacing: Theme.spacingM

        // ── Заголовок + выбор периода ─────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            Text {
                text: qsTr("Прогресс")
                font.pixelSize: Theme.fontSizeTitle
                font.bold: true
                color: Theme.textPrimary
            }

            Item { Layout.fillWidth: true }

            // Кнопки быстрого периода
            Repeater {
                model: [
                    { label: qsTr("1 д"),   days: 1  },
                    { label: qsTr("7 д"),   days: 7  },
                    { label: qsTr("30 д"),  days: 30 },
                    { label: qsTr("90 д"),  days: 90 },
                    { label: qsTr("Своё"),  days: 0  },
                ]
                delegate: Rectangle {
                    width: 52; height: 30
                    radius: Theme.radiusS
                    color: root.selectedPeriod === modelData.days
                           ? Theme.primary : Theme.surface
                    border.color: Theme.divider
                    border.width: 1
                    Text {
                        anchors.centerIn: parent
                        text: modelData.label
                        color: root.selectedPeriod === modelData.days
                               ? Theme.textOnPrimary : Theme.textSecondary
                        font.pixelSize: Theme.fontSizeSmall
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.selectedPeriod = modelData.days
                    }
                }
            }
        }

        // ── Кастомный период ──────────────────────────────────────────────────
        RowLayout {
            visible: root.selectedPeriod === 0
            spacing: Theme.spacingM

            Text {
                text: qsTr("С:")
                color: Theme.textSecondary
                font.pixelSize: Theme.fontSizeBody
            }

            DatePickerButton {
                selectedDate: root.customFrom
                onDateSelected: function(d) { root.customFrom = d }
            }

            Text {
                text: qsTr("По:")
                color: Theme.textSecondary
                font.pixelSize: Theme.fontSizeBody
            }

            DatePickerButton {
                selectedDate: root.customTo
                onDateSelected: function(d) { root.customTo = d }
            }
        }

        // ── Плитки со статистикой ─────────────────────────────────────────────
        GridLayout {
            Layout.fillWidth: true
            columns: 4
            columnSpacing: Theme.spacingM
            rowSpacing: Theme.spacingM

            Repeater {
                model: [
                    { label: qsTr("Сессий"),        value: (root.summary.totalSessions || 0) + "" },
                    { label: qsTr("Время"),          value: root.formatSeconds(root.summary.totalSeconds || 0) },
                    { label: qsTr("Ср. скорость"),   value: Math.round(root.summary.avgWpm || 0) + " WPM" },
                    { label: qsTr("Лучшая скорость"),value: Math.round(root.summary.bestWpm || 0) + " WPM" },
                    { label: qsTr("Ср. CPM"),        value: Math.round(root.summary.avgCpm || 0) + "" },
                    { label: qsTr("Лучший CPM"),     value: Math.round(root.summary.bestCpm || 0) + "" },
                    { label: qsTr("Ср. ошибки"),     value: Math.round((root.summary.avgErrorRate || 0) * 10) / 10 + "%" },
                ]
                delegate: Rectangle {
                    Layout.fillWidth: true
                    height: 72
                    color: Theme.surface
                    radius: Theme.radiusM

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 4
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: modelData.value
                            font.pixelSize: Theme.fontSizeLarge
                            font.bold: true
                            color: Theme.primary
                        }
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: modelData.label
                            font.pixelSize: Theme.fontSizeSmall
                            color: Theme.textSecondary
                        }
                    }
                }
            }
        }

        // ── График ────────────────────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.surface
            radius: Theme.radiusM

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingM
                spacing: Theme.spacingS

                // Переключатель метрики
                RowLayout {
                    spacing: Theme.spacingS
                    Repeater {
                        model: [
                            { label: qsTr("WPM"),     metric: "wpm"       },
                            { label: qsTr("CPM"),     metric: "cpm"       },
                            { label: qsTr("Ошибки"),  metric: "errorRate" },
                            { label: qsTr("Время"),   metric: "minutes"   },
                        ]
                        delegate: Rectangle {
                            width: 70; height: 26
                            radius: Theme.radiusS
                            color: chart.activeMetric === modelData.metric
                                   ? Theme.primary : Theme.surfaceVariant
                            Text {
                                anchors.centerIn: parent
                                text: modelData.label
                                color: chart.activeMetric === modelData.metric
                                       ? Theme.textOnPrimary : Theme.textSecondary
                                font.pixelSize: Theme.fontSizeSmall
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: chart.activeMetric = modelData.metric
                            }
                        }
                    }
                }

                ProgressChart {
                    id: chart
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }

    Component.onCompleted: refreshSummary()
}