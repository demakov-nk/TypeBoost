#include "AppSettings.h"

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
    , m_s(QSettings::IniFormat, QSettings::UserScope,
          QStringLiteral("TypeBoost"), QStringLiteral("TypeBoost"))
{}

QStringList AppSettings::layoutsForLanguage(const QString &language) const
{
    if (language == "ru") {
        return { "йцукен", "русская_машинопись", "макинтош", "яверты" };
    } else {
        return { "qwerty", "colemak", "workman", "dvorak" };
    }
}

QString AppSettings::layoutId() const 
{ 
    return m_s.value(QStringLiteral("layoutId"), QStringLiteral("qwerty")).toString(); 
}

// РЕАЛИЗАЦИЯ ГЕТТЕРА:
QString AppSettings::interfaceLanguage() const 
{ 
    return m_s.value(QStringLiteral("interfaceLanguage"), QStringLiteral("ru")).toString(); 
}

QString AppSettings::lessonLanguage() const
{
    // Если lessonLanguage не задан явно — берём из interfaceLanguage
    QString def = (interfaceLanguage() == "ru") ? "ru" : "en";
    return m_s.value(QStringLiteral("lessonLanguage"), def).toString();
}

bool AppSettings::showKeyboard() const 
{ 
    return m_s.value(QStringLiteral("showKeyboard"), true).toBool(); 
}

bool AppSettings::colorByFinger() const 
{ 
    return m_s.value(QStringLiteral("colorByFinger"), true).toBool(); 
}

bool AppSettings::showFingerHint() const 
{ 
    return m_s.value(QStringLiteral("showFingerHint"), true).toBool(); 
}

bool AppSettings::highlightNextKey() const 
{ 
    return m_s.value(QStringLiteral("highlightNextKey"), true).toBool(); 
}

void AppSettings::setLayoutId(const QString &v)
{
    if (layoutId() == v) return;
    m_s.setValue(QStringLiteral("layoutId"), v);

    // Синхронизируем язык уроков с раскладкой
    QStringList ruLayouts = {"йцукен", "русская_машинопись", "макинтош", "яверты"};
    QString newLang = ruLayouts.contains(v) ? "ru" : "en";
    if (lessonLanguage() != newLang) {
        m_s.setValue(QStringLiteral("lessonLanguage"), newLang);
        emit lessonLanguageChanged();
    }

    emit layoutIdChanged();
}

// РЕАЛИЗАЦИЯ СЕТТЕНА:
void AppSettings::setInterfaceLanguage(const QString &v)
{ 
    if (interfaceLanguage() == v) return; 
    m_s.setValue(QStringLiteral("interfaceLanguage"), v); 
    emit interfaceLanguageChanged(); 
}

void AppSettings::setLessonLanguage(const QString &lang)
{
    if (lessonLanguage() == lang) return;
    m_s.setValue(QStringLiteral("lessonLanguage"), lang);
    // Раскладку НЕ сбрасываем — уроки берут её из layoutHint,
    // практика — из appSettings.layoutId который пользователь выбирает сам
    emit lessonLanguageChanged();
}


void AppSettings::setShowKeyboard(bool v)
{ 
    if (showKeyboard() == v) return; 
    m_s.setValue(QStringLiteral("showKeyboard"), v); 
    emit showKeyboardChanged(); 
}

void AppSettings::setColorByFinger(bool v)
{ 
    if (colorByFinger() == v) return; 
    m_s.setValue(QStringLiteral("colorByFinger"), v); 
    emit colorByFingerChanged(); 
}

void AppSettings::setShowFingerHint(bool v)
{ 
    if (showFingerHint() == v) return; 
    m_s.setValue(QStringLiteral("showFingerHint"), v); 
    emit showFingerHintChanged(); 
}

void AppSettings::setHighlightNextKey(bool v)
{ 
    if (highlightNextKey() == v) return; 
    m_s.setValue(QStringLiteral("highlightNextKey"), v); 
    emit highlightNextKeyChanged(); 
}