pragma Singleton
import QtQuick

QtObject {
    // Palette
    readonly property color background:     "#1C1C2E"
    readonly property color surface:        "#2A2A40"
    readonly property color surfaceVariant: "#33334D"
    readonly property color primary:        "#7C6AF7"
    readonly property color primaryVariant: "#5A4FCC"
    readonly property color secondary:      "#4FC3F7"
    readonly property color success:        "#66BB6A"
    readonly property color errorColor:     "#EF5350"
    readonly property color warning:        "#FFA726"
    readonly property color textPrimary:    "#E8E8F0"
    readonly property color textSecondary:  "#C8C8DC"
    readonly property color textOnPrimary:  "#FFFFFF"
    readonly property color divider:        "#3A3A55"

    // Finger colours: index 0 unused; 1=thumb 2=index 3=middle 4=ring 5=pinky
    // index 0 = служебные, 1-5 = левая рука, 6-10 = правая рука
    readonly property var fingerColors: [
        "#33334D",  // 0  — служебные
        "#FFFFFF",  // 1  — левый большой
        "#E07050",  // 2  — левый указательный
        "#6699CC",  // 3  — левый средний
        "#88BB66",  // 4  — левый безымянный
        "#DDCC55",  // 5  — левый мизинец
        "#DDCC55",  // 6  — правый мизинец
        "#88BB66",  // 7  — правый безымянный
        "#6699CC",  // 8  — правый средний
        "#E07050",  // 9  — правый указательный
        "#FFFFFF",  // 10 — правый большой
    ]

    // Typography
    readonly property string fontFamily:     "Segoe UI, sans-serif"
    readonly property string monoFontFamily: "Consolas, monospace"

    readonly property int fontSizeSmall:   11
    readonly property int fontSizeBody:    14
    readonly property int fontSizeLarge:   18
    readonly property int fontSizeTitle:   24
    readonly property int fontSizeDisplay: 36

    // Spacing
    readonly property int spacingXs:  4
    readonly property int spacingS:   8
    readonly property int spacingM:  16
    readonly property int spacingL:  24
    readonly property int spacingXl: 40

    // Shape
    readonly property int radiusS:  6
    readonly property int radiusM: 12
    readonly property int radiusL: 20

    // Animation
    readonly property int durationFast:   120
    readonly property int durationNormal: 200
    readonly property int durationSlow:   350
}
