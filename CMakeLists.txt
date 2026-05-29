cmake_minimum_required(VERSION 3.21)

project(TypeBoost
    VERSION 0.1.0
    DESCRIPTION "Keyboard trainer application"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ── Qt ────────────────────────────────────────────────────────────────────────
find_package(Qt6 6.4 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    LinguistTools
)

qt_standard_project_setup()

# ── Sources ───────────────────────────────────────────────────────────────────
set(APP_SOURCES
    src/main.cpp
    src/app/Application.cpp
    src/app/Application.h
    src/core/AppSettings.cpp
    src/core/AppSettings.h
    src/core/KeyboardLayout.cpp
    src/core/KeyboardLayout.h
    src/core/LessonManager.cpp
    src/core/LessonManager.h
    src/core/TypingSession.cpp
    src/core/TypingSession.h
    src/core/TranslationManager.h
    src/core/ProgressTracker.cpp
    src/core/ProgressTracker.h
    src/core/StatisticsEngine.cpp
    src/core/StatisticsEngine.h
    src/models/LessonModel.cpp
    src/models/LessonModel.h
    src/models/ProgressModel.cpp
    src/models/ProgressModel.h
    src/models/SessionResult.h
)

# ── Executable ────────────────────────────────────────────────────────────────
qt_add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
)

# qt_add_qml_module creates a "TypeBoost/" subdirectory in the build tree,
# which conflicts with an executable also named "TypeBoost" on Linux.
# Use a lowercase output name to avoid the clash.
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "typeboost"
)

# Mark Theme.qml as a QML singleton
set_source_files_properties(qml/styles/Theme.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

# ── QML module ────────────────────────────────────────────────────────────────
set_source_files_properties(qml/styles/Theme.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(${PROJECT_NAME}
    URI TypeBoost
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/styles/Theme.qml
        qml/components/KeyCap.qml
        qml/components/KeyboardView.qml
        qml/components/TypingArea.qml
        qml/components/LessonSelector.qml
        qml/components/StatisticsView.qml
        qml/components/ProgressChart.qml
        qml/components/FingerHint.qml
        qml/components/NavBar.qml
        qml/components/NavItem.qml
        qml/components/ActionButton.qml
        qml/components/StatTile.qml
        qml/components/SettingsSwitch.qml
        qml/components/DatePickerButton.qml
        qml/pages/HomePage.qml
        qml/pages/LessonPage.qml
        qml/pages/PracticePage.qml
        qml/pages/ProgressPage.qml
        qml/pages/SettingsPage.qml
    RESOURCES
        resources/lessons/ru_lessons.json
        resources/lessons/en_lessons.json
)

# ── Translations (must come AFTER qt_add_executable) ─────────────────────────
set(TS_FILES
    src/i18n/app_ru.ts
    src/i18n/app_en.ts
)

qt_add_translations(${PROJECT_NAME}
    TS_FILES ${TS_FILES}
    RESOURCE_PREFIX "/i18n"
)

# ── Link libraries ────────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ── Data directory (runtime) ──────────────────────────────────────────────────
# Copy data directory next to the executable for development convenience
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data"
    COMMENT "Creating runtime data directory"
)

# ── Install ───────────────────────────────────────────────────────────────────
install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
