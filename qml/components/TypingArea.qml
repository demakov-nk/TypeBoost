import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import TypeBoost 1.0

ColumnLayout {
    id: root
    spacing: Theme.spacingM
    Layout.fillWidth: true

    property string currentText: ""
    property string lessonId: ""
    property string language: ""
    property string layoutId: ""
    property string ownerTag: ""

    property string cachedInputText: ""
    property int    cachedCursorPos: 0

    property var chars: currentText.split("")

    // ── Публичные функции ─────────────────────────────────────────────────────

    function syncCache() {
        // Не синхронизируем если сессия чужая
        if (typingSession.ownerTag !== root.ownerTag) {
            console.log("[TypingArea] syncCache SKIP ownerTag=", root.ownerTag,
                        "sessionOwner=", typingSession.ownerTag)
            return
        }
        root.cachedInputText = typingSession.inputText
        root.cachedCursorPos = typingSession.cursorPos
        console.log("[TypingArea] syncCache ownerTag=", root.ownerTag,
                    "sessionOwner=", typingSession.ownerTag,
                    "inputText=", typingSession.inputText.substring(0,20),
                    "cursorPos=", typingSession.cursorPos)
    }

    function forceInputFocus() {
        syncCache()
        inputField.enabled = true
        inputField.forceActiveFocus()
    }

    function clear() {
        finishOverlay.visible = false
        root.cachedInputText = ""
        root.cachedCursorPos = 0
        inputField.enabled = true
        inputField.text = ""              // ← сначала текст
        Qt.callLater(function() {         // ← фокус чуть позже
            inputField.forceActiveFocus()
        })
    }


    // ── Реакция на смену текста ───────────────────────────────────────────────

    onCurrentTextChanged: {
        console.log("[TypingArea] currentTextChanged ownerTag=", root.ownerTag,
            "text=", currentText.substring(0, 30))
        if (currentText !== "") {
            root.cachedInputText = ""
            root.cachedCursorPos = 0
            inputField.text = ""          // ← СНАЧАЛА чистим поле
            typingSession.start(currentText, lessonId, language, layoutId, ownerTag)
            inputField.forceActiveFocus() // ← возвращаем фокус
        }
    }

    // ── Сигналы сессии ────────────────────────────────────────────────────────

    Connections {
        target: typingSession

        function onInputTextChanged() {
            if (typingSession.ownerTag !== root.ownerTag) return
            root.cachedInputText = typingSession.inputText
        }

        function onCursorPosChanged() {
            if (typingSession.ownerTag !== root.ownerTag) return
            root.cachedCursorPos = typingSession.cursorPos
        }

        function onSessionFinished(result) {
            if (typingSession.ownerTag !== root.ownerTag) return
            inputField.text = ""
            inputField.enabled = false
            finishOverlay.visible = true
        }

        function onActiveChanged() {
            if (typingSession.ownerTag !== root.ownerTag) return
            if (!typingSession.active) {
                root.cachedInputText = ""
                root.cachedCursorPos = 0
            }
        }
    }

    // ── Поле ввода ────────────────────────────────────────────────────────────

    TextField {
        id: inputField
        Layout.fillWidth: true
        font.family: Theme.monoFontFamily
        font.pixelSize: 20
        color: Theme.textPrimary
        placeholderText: qsTr("Начните печатать здесь для старта тренировки...")

        background: Rectangle {
            color: Theme.surface
            border.color: inputField.activeFocus ? Theme.primary : Theme.divider
            border.width: inputField.activeFocus ? 2 : 1
            radius: Theme.radiusM
        }

        onTextChanged: {
            console.log("[TypingArea] text=", text,
                        "ownerTag=", root.ownerTag,
                        "sessionOwner=", typingSession.ownerTag,
                        "active=", typingSession.active,
                        "paused=", typingSession.paused,
                        "focus=", inputField.activeFocus)

            if (!inputField.activeFocus)                          return
            if (!typingSession.active)                            return
            if (typingSession.paused)                             return
            if (typingSession.ownerTag !== root.ownerTag)         return
            if (typingSession.targetText === "")                  return

            // ── Ограничение длины ввода ───────────────────────────────────────────
            const maxLen = typingSession.targetText.length
            if (text.length > maxLen) {
                const cursorPos = inputField.cursorPosition  // ← запомнить позицию
                inputField.text = text.substring(0, maxLen)
                inputField.cursorPosition = Math.min(cursorPos, maxLen)  // ← восстановить
                return
            }
            // ─────────────────────────────────────────────────────────────────────

            typingSession.setInputText(text)
        }

        Component.onCompleted: inputField.forceActiveFocus()
    }

    // ── Отображение текста ────────────────────────────────────────────────────

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: Math.max(120, flowLayout.implicitHeight + Theme.spacingL * 2)
        color: Theme.surface
        border.color: Theme.divider
        border.width: 1
        radius: Theme.radiusM
        clip: true

        Flow {
            id: flowLayout
            anchors.fill: parent
            anchors.margins: Theme.spacingL
            spacing: 0

            Repeater {
                model: root.chars.length
                delegate: Text {
                    id: charText
                    property string ch: root.chars[index]
                    property string typed: index < root.cachedInputText.length
                                        ? root.cachedInputText[index] : ""   // ← кэш
                    property bool isCorrect: typed !== "" && typed === ch
                    property bool isError:   typed !== "" && typed !== ch
                    property bool isCurrent: index === root.cachedCursorPos    // ← кэш

                    text: ch === " " ? "␣" : ch
                    font.family: Theme.monoFontFamily
                    font.pixelSize: 22

                    color: isError   ? Theme.errorColor   :
                           isCorrect ? Theme.success       :
                           isCurrent ? Theme.primary       :
                                       Theme.textSecondary

                    // Фон для ошибки на пробеле
                    Rectangle {
                        anchors.fill: parent
                        color: Theme.errorColor
                        opacity: 0.15
                        visible: charText.isError && charText.ch === " "
                        z: -1
                    }

                    // Курсор-подчёркивание
                    Rectangle {
                        id: cursorRect
                        visible: charText.isCurrent
                                 && typingSession.active
                                 && typingSession.ownerTag === root.ownerTag
                        width: parent.width
                        height: 3
                        anchors.bottom: parent.bottom
                        color: Theme.primary

                        SequentialAnimation on opacity {
                            running: cursorRect.visible
                            loops: Animation.Infinite
                            NumberAnimation { to: 0; duration: 400; easing.type: Easing.InOutQuad }
                            NumberAnimation { to: 1; duration: 400; easing.type: Easing.InOutQuad }
                        }
                    }
                }
            }
        }

        // Оверлей завершения — поверх текстового блока
        Rectangle {
            id: finishOverlay
            anchors.fill: parent
            color: Theme.surface
            opacity: 0.92
            radius: Theme.radiusM
            visible: false
            z: 10

            ColumnLayout {
                anchors.centerIn: parent
                spacing: Theme.spacingM

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "✓"
                    font.pixelSize: 48
                    color: Theme.success
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Отлично! Текст завершён")
                    font.pixelSize: 20
                    font.bold: true
                    color: Theme.textPrimary
                }
            }
        }
    }
}