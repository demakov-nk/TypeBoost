import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import TypeBoost 1.0

RowLayout {
    id: root
    spacing: Theme.spacingL
    anchors.fill: parent

    signal lessonStarted(string lessonId, var passages)

    property int selectedIndex: 0
    property string selectedLessonId: ""
    property string selectedTitle: ""
    property string selectedDesc: ""
    property int selectedDifficulty: 1
    property var selectedPassages: []

    // Функция обновления детальной информации о выбранном уроке
    function updateDetails() {
        if (lessonModel.rowCount() > root.selectedIndex) {
            var idx = lessonModel.index(root.selectedIndex, 0);
            root.selectedLessonId = lessonModel.data(idx, Qt.UserRole + 1); // lessonId
            root.selectedTitle = lessonModel.data(idx, Qt.UserRole + 2);    // title
            root.selectedDesc = lessonModel.data(idx, Qt.UserRole + 3);     // description
            root.selectedDifficulty = lessonModel.data(idx, Qt.UserRole + 6); // difficulty
            
            // Запрашиваем текст отрывков через менеджер
            root.selectedPassages = lessonManager.passagesForLesson(root.selectedLessonId);
        }
    }

    Component.onCompleted: updateDetails()

    Connections {
        target: lessonModel
        function onModelReset() {
            root.selectedIndex = 0;
            root.updateDetails();
        }
    }

    // ЛЕВАЯ ЧАСТЬ: Список уроков
    Rectangle {
        Layout.preferredWidth: 320
        Layout.fillHeight: true
        color: Theme.surface
        border.color: Theme.divider
        radius: Theme.radiusM

        ListView {
            id: listView
            anchors.fill: parent
            anchors.margins: Theme.spacingS
            model: lessonModel
            clip: true
            spacing: 4

            delegate: ItemDelegate {
                width: listView.width
                height: 64
                
                background: Rectangle {
                    color: listView.currentIndex === index ? Qt.alpha(Theme.primary, 0.08) : "transparent"
                    border.color: listView.currentIndex === index ? Theme.primary : "transparent"
                    border.width: 1
                    radius: Theme.radiusS
                }

                contentItem: ColumnLayout {
                    spacing: 2
                    Text {
                        text: model.title
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        font.bold: true
                        color: Theme.textPrimary
                    }
                    Row {
                        spacing: Theme.spacingM
                        Text {
                            text: qsTr("Сложность: ") + "★".repeat(model.difficulty)
                            font.family: Theme.fontFamily
                            font.pixelSize: 11
                            color: Theme.warning
                        }
                        Text {
                            text: model.passageCount + " " + qsTr("текст(ов)")
                            font.family: Theme.fontFamily
                            font.pixelSize: 11
                            color: Theme.textSecondary
                        }
                    }
                }

                onClicked: {
                    listView.currentIndex = index;
                    root.selectedIndex = index;
                    root.updateDetails();
                }
            }
        }
    }

    // ПРАВАЯ ЧАСТЬ: Детализация и запуск
    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        color: Theme.surface
        border.color: Theme.divider
        radius: Theme.radiusM

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingXL
            spacing: Theme.spacingL

            Text {
                text: root.selectedTitle
                font.family: Theme.fontFamily
                font.pixelSize: 22
                font.bold: true
                color: Theme.textPrimary
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: Theme.divider
            }

            Text {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: root.selectedDesc
                font.family: Theme.fontFamily
                font.pixelSize: 14
                color: Theme.textSecondary
                wrapMode: Text.WordWrap
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.spacingM

                ColumnLayout {
                    spacing: 2
                    Text {
                        text: qsTr("Поддерживаемая раскладка:")
                        font.pixelSize: 11
                        color: Theme.textSecondary
                    }
                    Text {
                        // Раскладка-подсказка из модели
                        property var idx: lessonModel.index(root.selectedIndex, 0)
                        text: lessonModel.rowCount() > root.selectedIndex ? lessonModel.data(idx, Qt.UserRole + 5).toUpperCase() : ""
                        font.bold: true
                        color: Theme.primary
                    }
                }

                Item { Layout.fillWidth: true }

                ActionButton {
                    text: qsTr("Начать обучение")
                    onClicked: {
                        if (root.selectedLessonId !== "") {
                            root.lessonStarted(root.selectedLessonId, root.selectedPassages);
                        }
                    }
                }
            }
        }
    }
}