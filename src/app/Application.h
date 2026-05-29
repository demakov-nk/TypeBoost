#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include <memory>

class LessonManager;
class ProgressTracker;
class StatisticsEngine;
class KeyboardLayout;
class TypingSession;
class LessonModel;
class ProgressModel;
class AppSettings;        // Добавлено
class TranslationManager; // Добавлено

class Application : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Application)

public:
    explicit Application(QObject *parent = nullptr);
    ~Application() override;

    int run();

private:
    void registerQmlTypes();
    void exposeToQml();

    QQmlApplicationEngine m_engine;

    // Core services (owned)
    std::unique_ptr<AppSettings>        m_settings;         // Добавлено
    std::unique_ptr<TranslationManager> m_translation;      // Добавлено
    std::unique_ptr<KeyboardLayout>     m_keyboardLayout;
    std::unique_ptr<LessonManager>      m_lessonManager;
    std::unique_ptr<TypingSession>      m_typingSession;
    std::unique_ptr<ProgressTracker>    m_progressTracker;
    std::unique_ptr<StatisticsEngine>   m_statisticsEngine;

    // Models
    std::unique_ptr<LessonModel>   m_lessonModel;
    std::unique_ptr<ProgressModel> m_progressModel;
};