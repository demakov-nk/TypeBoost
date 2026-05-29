import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import TypeBoost 1.0

Item {
    id: root

    property string currentText: ""
    property string lessonId:    "practice"
    property string language:    appSettings.lessonLanguage
    property string layoutId:    appSettings.layoutId
    property string localNextChar: ""
    property bool sessionInitialized: false
    property string savedInputText: ""
    property bool   practiceWasActive: false
    property int savedElapsedSeconds: 0
    property bool needsReset: false 

    function focusInput() {
        typingArea.forceInputFocus()
    }

    function startNewSession() {
        var text = lessonManager.randomPassage(
            appSettings.lessonLanguage,
            appSettings.layoutId,        // ← добавить
            typingArea.currentText
        )
        if (text.length === 0) return
        typingArea.currentText = text
        typingArea.clear()
    }

    onVisibleChanged: {
        if (visible) {
            // Восстанавливаем раскладку практики из настроек
            // (могла быть перебита уроком)
            keyboardLayout.currentLayoutId = appSettings.layoutId
        }
        if (!visible && typingSession.ownerTag === "practice") {
            practiceWasActive   = typingSession.active
            savedInputText      = typingSession.inputText
            savedElapsedSeconds = statsView.elapsedSeconds
            typingSession.pause()
        }
        if (visible) {
            // ← ДОБАВЬ ЭТОТ БЛОК ПЕРВЫМ
            if (root.needsReset) {
                root.needsReset = false
                root.sessionInitialized = false
                root.practiceWasActive  = false
                root.savedInputText     = ""
                // ← явно обновить раскладку
                keyboardLayout.currentLayoutId = appSettings.layoutId
                root.startNewSession()
                typingArea.forceInputFocus()
                return
            }


            if (typingSession.ownerTag === "practice") {
                if (typingSession.active && typingSession.paused)
                    typingSession.resume()
                statsView.syncStats()
                typingArea.syncCache()
            } else if (!sessionInitialized) {
                sessionInitialized = true
                startNewSession()
            } else if (practiceWasActive && typingArea.currentText !== "") {
                statsView.initialSeconds = savedElapsedSeconds
                typingSession.start(typingArea.currentText,
                                    "", appSettings.lessonLanguage,
                                    appSettings.layoutId, "practice")
                if (savedInputText !== "")
                    typingSession.setInputText(savedInputText)
                typingArea.syncCache()
            } else {
                startNewSession()
            }
            typingArea.forceInputFocus()
        }
    }

    Component.onCompleted: {
        console.log("[Practice] lessonLanguage =", appSettings.lessonLanguage)
        console.log("[Practice] appSettings =", appSettings)
        lessonManager.loadForLanguage(appSettings.lessonLanguage)
        // startNewSession()
    }


    Connections {
        target: typingSession
        function onSessionFinished(result) {
            if (typingSession.ownerTag !== "practice") return
            Qt.callLater(root.startNewSession);
        }
        function onNextCharChanged() {
            if (typingSession.ownerTag === "practice")
                root.localNextChar = typingSession.nextChar
        }
    }

    Connections {
        target: appSettings
        function onLayoutIdChanged() {
            root.layoutId = appSettings.layoutId
        }
        function onLessonLanguageChanged() {
            lessonManager.loadForLanguage(appSettings.lessonLanguage)
            root.needsReset = true
        }
    }


    Connections {
        target: lessonManager
        function onLessonsLoaded() {
            if (root.visible) {
                root.sessionInitialized = false
                root.startNewSession()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingL
        spacing: Theme.spacingL

        Text {
            text: qsTr("Свободная практика")
            font.family: Theme.fontFamily
            font.pixelSize: 20
            font.bold: true
            color: Theme.textPrimary
        }

        StatisticsView {
            id: statsView
            ownerTag: "practice"
            Layout.fillWidth: true
        }

        TypingArea {
            id: typingArea
            ownerTag: "practice"
            Layout.fillWidth: true
            Layout.fillHeight: true
            lessonId:    root.lessonId
            language:    root.language
            layoutId:    root.layoutId
        }

        KeyboardView {
            Layout.alignment: Qt.AlignHCenter
            nextChar: root.localNextChar
        }

        FingerHint {
            Layout.alignment: Qt.AlignHCenter
            nextChar: root.localNextChar
        }


        RowLayout {
            Layout.fillWidth: true

            Item { Layout.fillWidth: true }

            ActionButton {
                text: qsTr("Новый текст")
                onClicked: root.startNewSession()
            }
        }
    }
}
