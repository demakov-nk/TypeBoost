import QtQuick
import QtQuick.Layouts
import TypeBoost 1.0

RowLayout {
    id: root
    spacing: Theme.spacingXL
    Layout.fillWidth: true

    // ── Владелец этого виджета статистики ────────────────────────────────────
    property string ownerTag: ""

    // ── Локальный кэш — обновляется только если мы владелец ──────────────────
    property real   cachedCpm:     0
    property real   cachedWpm:     0
    property real   cachedErrors:  0
    property int    elapsedSeconds: 0
    property int    initialSeconds: 0
    property bool inputStarted: false

    // ── Принудительная синхронизация при возврате на вкладку ─────────────────
    function syncStats() {
        if (typingSession.ownerTag !== root.ownerTag) return
        root.cachedCpm    = typingSession.currentCpm
        root.cachedWpm    = typingSession.currentWpm
        root.cachedErrors = typingSession.errorRate
    }

    // ── Таймер — работает только если мы владелец ─────────────────────────────
    Timer {
        id: sessionTimer
        interval: 1000
        running: typingSession.active
                 && !typingSession.paused
                 && typingSession.ownerTag === root.ownerTag
                 && root.inputStarted  
        repeat: true
        onTriggered: root.elapsedSeconds++
    }

    // ── Слушаем сигналы сессии — реагируем только если мы владелец ───────────
    Connections {
        target: typingSession

        function onTargetTextChanged() {
            if (typingSession.ownerTag !== root.ownerTag) return
            root.elapsedSeconds = root.initialSeconds  // ← берём из переданного
            root.initialSeconds = 0                    // ← сбрасываем после использования
            root.cachedCpm    = 0
            root.cachedWpm    = 0
            root.cachedErrors = 0
            root.inputStarted = false
        }

        function onCurrentCpmChanged() {
            if (typingSession.ownerTag !== root.ownerTag) return
            root.cachedCpm = typingSession.currentCpm
        }

        function onCurrentWpmChanged() {
            if (typingSession.ownerTag !== root.ownerTag) return
            root.cachedWpm = typingSession.currentWpm
        }

        function onErrorRateChanged() {
            if (typingSession.ownerTag !== root.ownerTag) return
            root.cachedErrors = typingSession.errorRate
        }

        function onInputTextChanged() {
            if (typingSession.ownerTag !== root.ownerTag) return
            if (!root.inputStarted && typingSession.inputText.length > 0) {
                root.inputStarted = true   // ← первый символ — запускаем таймер
            }
            if (typingSession.inputText === typingSession.targetText) {
                root.inputStarted = false  // ← вся строка верно — стоп
            }
        }

        function onActiveChanged() {
            if (typingSession.ownerTag !== root.ownerTag) return
            if (!typingSession.active) {
                root.elapsedSeconds = 0
                root.cachedCpm      = 0
                root.cachedWpm      = 0
                root.cachedErrors   = 0
                root.inputStarted = false
            }
        }
    }

    function formatTime(totalSeconds) {
        var m = Math.floor(totalSeconds / 60)
        var s = totalSeconds % 60
        return String(m).padStart(2, '0') + ":" + String(s).padStart(2, '0')
    }

    // ── UI ───────────────────────────────────────────────────────────────────

    StatTile {
        Layout.fillWidth: true
        label: "CPM (симв/мин)"
        value: String(Math.round(root.cachedCpm))
    }

    StatTile {
        Layout.fillWidth: true
        label: "WPM (слов/мин)"
        value: String(Math.round(root.cachedWpm))
    }

    ColumnLayout {
        Layout.fillWidth: true
        spacing: Theme.spacingXs

        Text {
            text: qsTr("Ошибки")
            font.family: Theme.fontFamily
            font.pixelSize: 12
            color: Theme.textSecondary
        }
        Text {
            text: root.cachedErrors.toFixed(1) + "%"
            font.family: Theme.fontFamily
            font.pixelSize: 28
            font.bold: true
            color: root.cachedErrors > 5.0 ? Theme.errorColor : Theme.success

            Behavior on color { ColorAnimation { duration: 200 } }
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        spacing: Theme.spacingXs

        Text {
            text: qsTr("Время")
            font.family: Theme.fontFamily
            font.pixelSize: 12
            color: Theme.textSecondary
        }

        Text {
            text: root.formatTime(root.elapsedSeconds)
            font.family: Theme.monoFontFamily
            font.pixelSize: 28
            font.bold: true
            color: Theme.textPrimary
        }
    }
}