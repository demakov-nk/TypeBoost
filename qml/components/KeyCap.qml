import QtQuick
import TypeBoost 1.0
import "../styles"

Rectangle {
    id: root
    
    // Входные свойства клавиши
    property string keyId: ""
    property string displayLabel: ""
    property int finger: 0       // 1-4 (левая рука), 5-8 (правая рука). 0 - спец.клавиши/пробел
    property int hand: 0         // 0 - левая, 1 - правая
    property bool isNext: false
    property bool isActive: false  // Нужна ли подсветка как следующей клавиши
    property bool colorByFinger: true

    property real keyWidth: 44
    property real keyHeight: 44

    width: keyWidth
    height: keyHeight
    radius: Theme.radiusS

    // Вычисление фонового цвета клавиши
    color: {
        if (isNext) return Theme.primary;
        if (colorByFinger) {
            return Theme.fingerColors[finger] || Theme.surfaceVariant;
        }
        return Theme.surfaceVariant;
    }

    border.width: isNext ? 2 : 1
    border.color: isNext ? Theme.primaryVariant : Theme.divider

    Component.onCompleted: {
        console.log("[KeyCap]", keyId, "finger=", finger, "color=", Theme.fingerColors[finger])
    }


    // Плавная анимация смены цвета при печати
    Behavior on color { ColorAnimation { duration: 120 } }

    Text {
        anchors.centerIn: parent
        text: {
            if (displayLabel !== "") return displayLabel;
            if (keyId === "Space") return "";
            return keyId;
        }
        font.family: Theme.fontFamily
        font.pixelSize: keyId.length > 1 ? 11 : 14
        font.bold: isNext ? true : false
        color: isNext ? Theme.textOnPrimary : Theme.textPrimary
    }
}