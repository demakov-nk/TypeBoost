import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import TypeBoost 1.0
import "../components"

Item {
    id: root
    anchors.fill: parent

    // Состояния страницы: "selecting" | "typing"
    property string pageState: "selecting"

    property string activeLessonId: ""
    property var activePassages: []
    property int passageIndex: 0

    // Данные для финального модального окна результатов
    property real finalCpm: 0
    property real finalWpm: 0
    property real finalErrorRate: 0

    property string localNextChar: ""
    property string savedInputText: ""
    property int    savedCursorPos: 0
    property bool   lessonWasActive: false
    property int savedElapsedSeconds: 0

    function focusInput() {
        typingArea.forceInputFocus()
    }

    Component.onCompleted: {
        console.log("[Lesson] lessonLanguage =", appSettings.lessonLanguage)
        lessonManager.loadForLanguage(appSettings.lessonLanguage);
    }

    onVisibleChanged: {
        if (!visible && typingSession.ownerTag === "lesson") {
            lessonWasActive        = typingSession.active
            savedInputText         = typingSession.inputText
            savedElapsedSeconds    = statsView.elapsedSeconds
            typingSession.pause()
        }
        if (visible) {
            if (typingSession.ownerTag === "lesson") {
                // Восстанавливаем раскладку урока поверх того что могли изменить в настройках
                var hint = lessonManager.layoutHintForLesson(root.activeLessonId)
                var layout = (hint && hint.length > 0) ? hint : appSettings.layoutId
                keyboardLayout.currentLayoutId = layout

                if (typingSession.active && typingSession.paused)
                    typingSession.resume()
                statsView.syncStats()
                typingArea.syncCache()
            } else if (lessonWasActive && typingArea.currentText !== "") {
                var hint2 = lessonManager.layoutHintForLesson(root.activeLessonId)
                var layout2 = (hint2 && hint2.length > 0) ? hint2 : appSettings.layoutId
                keyboardLayout.currentLayoutId = layout2

                statsView.initialSeconds = savedElapsedSeconds
                typingSession.start(typingArea.currentText,
                                    root.activeLessonId,
                                    appSettings.lessonLanguage,
                                    layout2,
                                    "lesson")
                if (savedInputText !== "")
                    typingSession.setInputText(savedInputText)
                typingArea.syncCache()
            }
            typingArea.forceInputFocus()
        }
    }


    // Отслеживаем завершение каждого упражнения сессии
    Connections {
        target: typingSession

        function onSessionFinished(result) {
            if (typingSession.ownerTag !== "lesson") return  // ← добавь

            root.finalCpm = typingSession.currentCpm;
            root.finalWpm = typingSession.currentWpm;
            root.finalErrorRate = typingSession.errorRate;

            root.passageIndex++;

            if (root.passageIndex < root.activePassages.length) {
                typingArea.currentText = root.activePassages[root.passageIndex];
                typingArea.clear();
            } else {
                resultOverlay.visible = true;
            }
        }

        function onNextCharChanged() {
            if (typingSession.ownerTag === "lesson")
                root.localNextChar = typingSession.nextChar
        }
    }


    // СОСТОЯНИЕ 1: Выбор урока
    LessonSelector {
        id: selector
        visible: root.pageState === "selecting"
        
        onLessonStarted: function(lessonId, passages) {
            if (!passages || passages.length === 0) return;
            console.log("[Lesson] lessonId=", lessonId, "passages.length=", passages.length)
            root.activeLessonId = lessonId;
            root.activePassages = passages;
            root.passageIndex = 0;
            root.pageState = "typing";

            // Берём раскладку из layoutHint урока, fallback — настройки
            var hint = lessonManager.layoutHintForLesson(lessonId);
            var layout = (hint && hint.length > 0) ? hint : appSettings.layoutId;

            console.log("[Lesson] hint=", hint, "layout=", (hint && hint.length > 0) ? hint : appSettings.layoutId)

            // Применяем раскладку немедленно
            keyboardLayout.currentLayoutId = layout;

            typingArea.lessonId = lessonId;
            typingArea.language = appSettings.lessonLanguage;
            typingArea.layoutId = layout;           // ← hint вместо appSettings
            typingArea.currentText = passages[0];
            typingArea.clear();
        }
    }

    // СОСТОЯНИЕ 2: Экран тренажёра
    ColumnLayout {
        id: typingLayout
        visible: root.pageState === "typing"
        anchors.fill: parent
        spacing: Theme.spacingL

        // Навигационная верхняя плашка
        RowLayout {
            Layout.fillWidth: true
            
            ActionButton {
                text: "← " + qsTr("К списку уроков")
                onClicked: {
                    typingSession.abort();
                    keyboardLayout.currentLayoutId = appSettings.layoutId  // ← восстановить
                    root.pageState = "selecting";
                }
            }

            Item { Layout.fillWidth: true }

            Text {
                text: qsTr("Текст ") + (root.passageIndex + 1) + " / " + root.activePassages.length
                font.family: Theme.fontFamily
                font.pixelSize: 16
                font.bold: true
                color: Theme.textPrimary
            }
        }

        // Блок реалтайм статистики
        StatisticsView {
            id: statsView
            ownerTag: "lesson"
        }

        // Рабочая зона с текстом
        TypingArea {
            id: typingArea
            ownerTag: "lesson"
            Layout.fillWidth: true
        }

        // Виртуальная клавиатура
        KeyboardView {
            id: keyboardView
            nextChar: root.localNextChar
            activeKeyId: typingArea.currentExpectedChar 
            visible: appSettings.showKeyboard 
            colorByFinger: appSettings.colorByFinger
            highlightNextKey: appSettings.highlightNextKey
        }

        // Помощник расположения пальцев
        FingerHint {
            id: fingerHint
            nextChar: root.localNextChar
        }

    }

    // ОВЕРЛЕЙ: Результаты успешно пройденного урока
    Rectangle {
        id: resultOverlay
        anchors.fill: parent
        color: Qt.rgba(Theme.background.r, Theme.background.g, Theme.background.b, 0.95)
        visible: false
        
        // Перехватываем клики мыши, имитируя модальность окна
        MouseArea { anchors.fill: parent }

        ColumnLayout {
            anchors.centerIn: parent
            spacing: Theme.spacingXL
            width: 400

            Text {
                Layout.alignment: Qt.AlignHCenter
                text: "🎉 " + qsTr("Урок успешно завершён!")
                font.family: Theme.fontFamily
                font.pixelSize: 24
                font.bold: true
                color: Theme.success
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: Theme.divider
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingL

                StatTile { Layout.fillWidth: true; label: "Итог CPM"; value: String(Math.round(root.finalCpm)) }
                StatTile { Layout.fillWidth: true; label: "Итог WPM"; value: String(Math.round(root.finalWpm)) }
            }

            StatTile {
                Layout.fillWidth: true
                label: qsTr("Процент ошибок")
                value: root.finalErrorRate.toFixed(1) + "%"
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingM
                Layout.topMargin: Theme.spacingL

                ActionButton {
                    Layout.fillWidth: true
                    text: qsTr("Повторить урок")
                    onClicked: {
                        resultOverlay.visible = false;
                        root.passageIndex = 0;
                        typingArea.currentText = root.activePassages[0];
                        typingArea.clear();
                    }
                }

                ActionButton {
                    Layout.fillWidth: true
                    text: qsTr("Завершить")
                    onClicked: {
                        resultOverlay.visible = false;
                        root.pageState = "selecting";
                    }
                }
            }
        }
    }
}