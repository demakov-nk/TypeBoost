import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import TypeBoost

ApplicationWindow {
    id: root
    visible: true
    width: 1280
    height: 800
    minimumWidth: 900
    minimumHeight: 620
    title: qsTr("TypeBoost")
    color: Theme.background

    // ── Navigation state ──────────────────────────────────────────────────────
    property string currentPage: "home"  // "home" | "lesson" | "practice" | "progress" | "settings"

    function navigate(page) {
        root.currentPage = page
        if (page === "practice") {
            // typingSession.resume()
            practicePageItem.focusInput()
        }
    }



    // ── Layout ────────────────────────────────────────────────────────────────
    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Sidebar navigation
        NavBar {
            Layout.fillHeight: true
            Layout.preferredWidth: 220
            currentPage: root.currentPage
            onNavigate: page => root.navigate(page)
        }

        // Page content area
        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: {
                switch (root.currentPage) {
                case "home":     return 0
                case "practice": return 1
                case "progress": return 2
                case "settings": return 3
                default:         return 0
                }
            }

            HomePage     { onNavigate: page => root.navigate(page) }
            PracticePage { id: practicePageItem }
            ProgressPage {}
            SettingsPage {}
        }

    }
}
