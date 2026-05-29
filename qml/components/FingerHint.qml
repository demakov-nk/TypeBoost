import QtQuick
import QtQuick.Layouts
import TypeBoost 1.0
import "../styles"

Item {
    id: root
    width: 520
    height: 110
    anchors.horizontalCenter: parent.horizontalCenter

    visible: appSettings.showFingerHint && typingSession.active

    property string nextChar:   typingSession.nextChar
    property string prevChar:   typingSession.prevChar
    property bool   needsShift: typingSession.needsShift

    // Основной палец для nextChar
    property int currentFinger: {
        if (nextChar === " ")
            return spaceThumb();
        return keyboardLayout.fingerForChar(nextChar);
    }

    // Палец Shift (противоположная рука, мизинец)
    property int shiftFinger: {
        if (!needsShift) return -1;
        if (nextChar === " ") return -1;
        var hand = keyboardLayout.shiftHandForChar(nextChar);
        return hand === 0 ? 5 : 6; // левый мизинец=5, правый мизинец=6
    }

    // Большой палец для пробела — инверсия по предыдущему символу
    function spaceThumb() {
        if (prevChar === "") return 1; // по умолчанию левый
        var prevHand = keyboardLayout.handForChar(prevChar);
        return prevHand === 0 ? 10 : 1; // инверсия: была левая → правый большой и наоборот
    }

    function getFingerName(f) {
        switch(f) {
            case 1:  case 10: return qsTr("Большой");
            case 2:  case 9:  return qsTr("Указательный");
            case 3:  case 8:  return qsTr("Средний");
            case 4:  case 7:  return qsTr("Безымянный");
            case 5:  case 6:  return qsTr("Мизинец");
            default: return "";
        }
    }

    function getHandName(f) {
        if (f >= 1 && f <= 5)  return qsTr("Левая рука");
        if (f >= 6 && f <= 10) return qsTr("Правая рука");
        return "";
    }

    // Цвет овала: активный основной, активный Shift, или серый
    function fingerColor(f) {
        if (root.currentFinger === f) return Theme.fingerColors[f] || Theme.surfaceVariant;
        if (root.shiftFinger   === f) return Theme.fingerColors[f] || Theme.surfaceVariant;
        return Theme.surfaceVariant;
    }

    RowLayout {
        anchors.fill: parent
        spacing: Theme.spacingL

        // ЛЕВАЯ РУКА
        Row {
            spacing: 5
            Layout.alignment: Qt.AlignBottom

            // Мизинец (5)
            Rectangle { width: 14; height: 40; radius: 7; color: root.fingerColor(5); anchors.bottom: parent.bottom }
            // Безымянный (4)
            Rectangle { width: 14; height: 52; radius: 7; color: root.fingerColor(4); anchors.bottom: parent.bottom }
            // Средний (3)
            Rectangle { width: 14; height: 58; radius: 7; color: root.fingerColor(3); anchors.bottom: parent.bottom }
            // Указательный (2)
            Rectangle { width: 14; height: 50; radius: 7; color: root.fingerColor(2); anchors.bottom: parent.bottom }
            // Большой (1)
            Rectangle { width: 22; height: 12; radius: 6; color: root.fingerColor(1); anchors.bottom: parent.bottom; anchors.bottomMargin: 2 }
        }

        // ПРАВАЯ РУКА
        Row {
            spacing: 5
            Layout.alignment: Qt.AlignBottom
            // Большой (10)
            Rectangle { width: 22; height: 12; radius: 6; color: root.fingerColor(10); anchors.bottom: parent.bottom; anchors.bottomMargin: 2 }
            // Указательный (9)
            Rectangle { width: 14; height: 50; radius: 7; color: root.fingerColor(9); anchors.bottom: parent.bottom }
            // Средний (8)
            Rectangle { width: 14; height: 58; radius: 7; color: root.fingerColor(8); anchors.bottom: parent.bottom }
            // Безымянный (7)
            Rectangle { width: 14; height: 52; radius: 7; color: root.fingerColor(7); anchors.bottom: parent.bottom }
            // Мизинец (6)
            Rectangle { width: 14; height: 40; radius: 7; color: root.fingerColor(6); anchors.bottom: parent.bottom }
        }
    }
}