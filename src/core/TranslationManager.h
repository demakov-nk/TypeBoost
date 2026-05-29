#pragma once
#include <QObject>
#include <QTranslator>
#include <QQmlEngine>
#include <QCoreApplication>

class TranslationManager : public QObject {
    Q_OBJECT
public:
    explicit TranslationManager(QQmlEngine *engine, QObject *parent = nullptr)
        : QObject(parent), m_engine(engine) {}

    Q_INVOKABLE void setLanguage(const QString &lang) {
        QCoreApplication::removeTranslator(&m_translator);
        
        // Загружаем файл перевода из ресурсов (например, :/i18n/app_ru.qm)
        if (m_translator.load(QStringLiteral(":/i18n/app_") + lang + QStringLiteral(".qm"))) {
            QCoreApplication::installTranslator(&m_translator);
        }
        
        // Инструктируем QML-движок полностью обновить все строки qsTr() в интерфейсе
        if (m_engine) {
            m_engine->retranslate();
        }
        emit languageChanged();
    }

signals:
    void languageChanged();

private:
    QTranslator m_translator;
    QQmlEngine *m_engine;
};