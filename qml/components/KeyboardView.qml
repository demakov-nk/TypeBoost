import QtQuick
import QtQuick.Layouts
import TypeBoost 1.0
import "../styles"

Column {
    id: root
    spacing: 4
    anchors.horizontalCenter: parent.horizontalCenter

    visible: appSettings.showKeyboard
    property bool colorByFinger:    appSettings.colorByFinger
    property bool highlightNextKey: appSettings.highlightNextKey

    property string nextChar:    typingSession.nextChar
    property string prevChar:    typingSession.prevChar
    property bool   needsShift:  typingSession.needsShift
    property string activeKeyId: ""

    // Какой Shift подсвечивать — противоположная рука от символа
    property string shiftKeyId: {
        if (!needsShift) return "";
        if (nextChar === " ") return "";
        var hand = keyboardLayout.shiftHandForChar(nextChar);
        return hand === 0 ? "shiftl" : "shiftr";
    }

    property var layoutKeys: []

    Component.onCompleted: {
        // Не перезаписываем currentLayoutId — он уже установлен
        root.layoutKeys = keyboardLayout.keyInfoList()
    }

    Connections {
        target: appSettings
        function onLayoutIdChanged() {
            // Не трогаем currentLayoutId — он управляется страницами
            root.layoutKeys = keyboardLayout.keyInfoList()
        }
    }


    Connections {
        target: keyboardLayout
        function onLayoutChanged() {
            root.layoutKeys = keyboardLayout.keyInfoList()
        }
    }

    function getWidth(keyId) {
        switch(keyId.toLowerCase()) {
            case "space":     return 290;
            case "backspace": return 82;
            case "tab":       return 62;
            case "caps":      return 76;
            case "enter":     return 84;
            case "shiftl":    return 94;
            case "shiftr":    return 112;
            case "ctrll":
            case "ctrlr":     return 60;
            case "altl":
            case "altr":      return 54;
            case "winl":
            case "winr":
            case "menur":     return 50;
            default:          return 44;
        }
    }

    function getLabel(keyId) {
        switch(keyId.toLowerCase()) {
            case "space":     return qsTr("Пробел");
            case "shiftl":
            case "shiftr":    return "Shift";
            case "ctrll":
            case "ctrlr":     return "Ctrl";
            case "altl":
            case "altr":      return "Alt";
            case "caps":      return "Caps";
            case "tab":       return "Tab";
            case "enter":     return "Enter";
            case "backspace": return "⌫";
            case "winl":
            case "winr":      return "⊞";
            case "menur":     return "☰";
            default:          return keyId.toUpperCase();
        }
    }

    Repeater {
        model: 5

        delegate: Row {
            id: rowLayout
            property int rowIndex: index
            spacing: 4
            anchors.horizontalCenter: parent.horizontalCenter

            Repeater {
                model: root.layoutKeys.filter(function(k) { return k.row === rowLayout.rowIndex; })
                delegate: KeyCap {
                    keyId:         modelData.keyId
                    displayLabel:  root.getLabel(modelData.keyId)
                    finger:        modelData.finger
                    hand:          modelData.hand
                    colorByFinger: root.colorByFinger
                    keyWidth:      root.getWidth(modelData.keyId)

                    isNext: {
                        var base = keyboardLayout.baseKeyForChar(root.nextChar);
                        if (modelData.keyId.toLowerCase() === ";")
                            console.log("key=';' nextChar='" + root.nextChar + "' base='" + base + "' match=" + (modelData.keyId.toLowerCase() === base));
                        return root.highlightNextKey && root.nextChar !== "" && (
                            modelData.keyId.toLowerCase() === root.nextChar.toLowerCase() ||
                            (root.nextChar === " " && modelData.keyId.toLowerCase() === "space") ||
                            (modelData.keyId.toLowerCase() === base) ||
                            (root.shiftKeyId !== "" && modelData.keyId.toLowerCase() === root.shiftKeyId)
                        );
                    }

                    isActive: root.activeKeyId !== "" && modelData.keyId === root.activeKeyId
                }
            }
        }
    }
}