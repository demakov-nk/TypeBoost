#include "Application.h"

#include "core/KeyboardLayout.h"
#include "core/LessonManager.h"
#include "core/TypingSession.h"
#include "core/ProgressTracker.h"
#include "core/StatisticsEngine.h"
#include "models/LessonModel.h"
#include "models/ProgressModel.h"
#include "core/AppSettings.h"         // Добавлено
#include "core/TranslationManager.h" // Добавлено (Убедитесь в точном пути к файлу TranslationManager.h)

#include <QQmlContext>
#include <QUrl>
#include <QGuiApplication>
#include <QQuickStyle>

Application::Application(QObject *parent)
    : QObject(parent)
{
    // Инициализируем настройки в первую очередь
    m_settings         = std::make_unique<AppSettings>(this);
    m_translation      = std::make_unique<TranslationManager>(&m_engine, this);
    
    m_keyboardLayout   = std::make_unique<KeyboardLayout>(this);
    m_lessonManager    = std::make_unique<LessonManager>(this);
    m_progressTracker  = std::make_unique<ProgressTracker>(this);
    m_statisticsEngine = std::make_unique<StatisticsEngine>(m_progressTracker.get(), this);
    m_typingSession    = std::make_unique<TypingSession>(m_progressTracker.get(), this);

    m_lessonModel   = std::make_unique<LessonModel>(m_lessonManager.get(), this);
    m_progressModel = std::make_unique<ProgressModel>(m_statisticsEngine.get(), this);

    // Применяем стартовый язык интерфейса из сохраненных настроек
    m_translation->setLanguage(m_settings->interfaceLanguage());
    
    // Синхронизируем раскладку клавиатуры с настройками
    m_keyboardLayout->setCurrentLayoutId(m_settings->layoutId());

    m_typingSession->setKeyboardLayout(m_keyboardLayout.get());
}

Application::~Application() = default;

int Application::run()
{
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    registerQmlTypes();
    exposeToQml();

    m_engine.load(QUrl(QStringLiteral("qrc:/TypeBoost/qml/main.qml")));

    if (m_engine.rootObjects().isEmpty())
        return EXIT_FAILURE;

    return QGuiApplication::exec();
}

void Application::registerQmlTypes() {}

void Application::exposeToQml()
{
    QQmlContext *ctx = m_engine.rootContext();

    // Экспонируем ВСЕ объекты бэкенда для связи с QML интерфейсом
    ctx->setContextProperty(QStringLiteral("appSettings"),       m_settings.get());       // ДОБАВЛЕНО!
    ctx->setContextProperty(QStringLiteral("translationManager"), m_translation.get());    // ДОБАВЛЕНО!
    ctx->setContextProperty(QStringLiteral("keyboardLayout"),     m_keyboardLayout.get());
    ctx->setContextProperty(QStringLiteral("lessonManager"),      m_lessonManager.get());
    ctx->setContextProperty(QStringLiteral("typingSession"),      m_typingSession.get());
    ctx->setContextProperty(QStringLiteral("progressTracker"),    m_progressTracker.get());
    ctx->setContextProperty(QStringLiteral("statisticsEngine"),   m_statisticsEngine.get());
    ctx->setContextProperty(QStringLiteral("lessonModel"),        m_lessonModel.get());
    ctx->setContextProperty(QStringLiteral("progressModel"),      m_progressModel.get());
}
