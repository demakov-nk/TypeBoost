import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import TypeBoost 1.0
import "../styles"

ScrollView {
    id: root
    anchors.fill: parent
    contentWidth: availableWidth
    clip: true

    property var availableLayouts: appSettings.layoutsForLanguage(appSettings.lessonLanguage)

    Connections {
        target: appSettings
        function onLessonLanguageChanged() {
            var layouts = appSettings.layoutsForLanguage(appSettings.lessonLanguage)
            layoutCombo.model = layouts
            layoutCombo.currentIndex = 0
        }
        function onLayoutIdChanged() {
            var layouts = appSettings.layoutsForLanguage(appSettings.lessonLanguage)
            var idx = layouts.indexOf(appSettings.layoutId)
            if (idx >= 0) layoutCombo.currentIndex = idx
        }
    }

    ColumnLayout {
        width: parent.width
        spacing: Theme.spacingXl
        anchors.top: parent.top
        anchors.margins: Theme.spacingL

        Text {
            text: qsTr("Настройки тренажёра")
            font.family: Theme.fontFamily
            font.pixelSize: 20
            font.bold: true
            color: Theme.textPrimary
        }

        // ── Группа: Интерактивная клавиатура ─────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            color: Theme.surface
            radius: Theme.radiusM
            height: keyboardGroup.implicitHeight + Theme.spacingL * 2

            ColumnLayout {
                id: keyboardGroup
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    margins: Theme.spacingL
                }
                spacing: Theme.spacingM

                Text {
                    text: qsTr("Интерактивная клавиатура")
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeLarge
                    font.bold: true
                    color: Theme.primary
                }

                SettingsSwitch {
                    text: qsTr("Отображать виртуальную клавиатуру")
                    checked: appSettings.showKeyboard
                    onToggled: {
                        appSettings.showKeyboard = checked
                        if (!checked) {
                            appSettings.colorByFinger    = false
                            appSettings.highlightNextKey = false
                        }
                    }
                }
                SettingsSwitch {
                    text: qsTr("Цветовое разделение зон под пальцы")
                    checked: appSettings.colorByFinger
                    enabled: appSettings.showKeyboard
                    opacity: appSettings.showKeyboard ? 1.0 : 0.4
                    onToggled: appSettings.colorByFinger = checked
                }
                SettingsSwitch {
                    text: qsTr("Подсвечивать следующую клавишу")
                    checked: appSettings.highlightNextKey
                    enabled: appSettings.showKeyboard
                    opacity: appSettings.showKeyboard ? 1.0 : 0.4
                    onToggled: appSettings.highlightNextKey = checked
                }
                SettingsSwitch {
                    text: qsTr("Показывать подсказку активного пальца")
                    checked: appSettings.showFingerHint
                    onToggled: appSettings.showFingerHint = checked
                }
            }
        }

        // ── Группа: Раскладки и язык ──────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            color: Theme.surface
            radius: Theme.radiusM
            height: layoutGroup.implicitHeight + Theme.spacingL * 2

            ColumnLayout {
                id: layoutGroup
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    margins: Theme.spacingL
                }
                spacing: Theme.spacingM

                Text {
                    text: qsTr("Раскладки и язык обучения")
                    font.family: Theme.fontFamily
                    font.pixelSize: Theme.fontSizeLarge
                    font.bold: true
                    color: Theme.primary
                }

                RowLayout {
                    spacing: Theme.spacingL
                    Text {
                        text: qsTr("Текущая раскладка:")
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontSizeBody
                        color: Theme.textPrimary
                        Layout.preferredWidth: 150
                    }
                    ComboBox {
                        id: layoutCombo
                        model: availableLayouts
                        currentIndex: availableLayouts.indexOf(appSettings.layoutId)
                        onActivated: {
                            appSettings.layoutId = availableLayouts[currentIndex]
                            keyboardLayout.currentLayoutId = availableLayouts[currentIndex]
                        }
                    }
                }

                RowLayout {
                    spacing: Theme.spacingL
                    Text {
                        text: qsTr("Язык текстов:")
                        font.family: Theme.fontFamily
                        font.pixelSize: Theme.fontSizeBody
                        color: Theme.textPrimary
                        Layout.preferredWidth: 150
                    }
                    ComboBox {
                        id: languageCombo
                        model: ["ru", "en"]
                        currentIndex: model.indexOf(appSettings.lessonLanguage)
                        onActivated: {
                            appSettings.lessonLanguage = model[currentIndex]
                            // layoutCombo.currentIndex уже сбросится в 0 через onLanguageChanged
                            // явно применяем первую раскладку нового языка
                            var layouts = appSettings.layoutsForLanguage(model[currentIndex])
                            if (layouts.length > 0) {
                                appSettings.layoutId = layouts[0]
                                keyboardLayout.currentLayoutId = layouts[0]
                            }
                        }
                    }
                }
            }
        }
    }
}