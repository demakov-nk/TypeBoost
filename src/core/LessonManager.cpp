#include "LessonManager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

LessonManager::LessonManager(QObject *parent)
    : QObject(parent)
{
    bool ok = loadForLanguage(QStringLiteral("en"));
    qDebug() << "[LessonManager] constructor loaded en:" << ok << "lessons count:" << m_lessons.size();
}


bool LessonManager::loadForLanguage(const QString &lang)
{
    const QString path = QStringLiteral(":/TypeBoost/resources/lessons/%1_lessons.json").arg(lang);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "LessonManager: cannot open" << path;
        return false;
    }

    QJsonParseError err;
    const auto doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "LessonManager: JSON parse error:" << err.errorString();
        return false;
    }

    const QJsonArray arr = doc.object().value(QStringLiteral("lessons")).toArray();
    m_lessons.clear();
    m_lessons.reserve(arr.size());
    for (const auto &val : arr) {
        m_lessons.append(lessonFromJson(val.toObject(), lang));
    }

    emit lessonsLoaded();
    return true;
}

QStringList LessonManager::passagesForLesson(const QString &id) const
{
    const Lesson *l = lessonById(id);
    return l ? l->passages : QStringList{};
}

QString LessonManager::randomPassage(const QString &lang,
                                      const QString &layoutHint,
                                      const QString &exclude) const
{
    QStringList candidates;
    for (const auto &l : m_lessons) {
        if (l.language != lang) continue;
        // фильтр по layoutHint если указан и совпадает
        if (!layoutHint.isEmpty() && !l.layoutHint.isEmpty()
            && l.layoutHint != layoutHint) continue;
        for (const QString &p : l.passages)
            if (p != exclude)
                candidates.append(p);
    }

    // Fallback — если для раскладки нет уроков, берём любые по языку
    if (candidates.isEmpty()) {
        for (const auto &l : m_lessons) {
            if (l.language != lang) continue;
            for (const QString &p : l.passages)
                if (p != exclude)
                    candidates.append(p);
        }
    }

    if (candidates.isEmpty()) return QString{};
    return candidates[QRandomGenerator::global()->bounded(candidates.size())];
}

QString LessonManager::layoutHintForLesson(const QString &lessonId) const
{
    for (const auto &l : m_lessons) {
        if (l.id == lessonId)
            return l.layoutHint;
    }
    return QString();
}

const QList<Lesson> &LessonManager::lessons() const { return m_lessons; }

const Lesson *LessonManager::lessonById(const QString &id) const
{
    for (const auto &l : m_lessons) {
        if (l.id == id) return &l;
    }
    return nullptr;
}

Lesson LessonManager::lessonFromJson(const QJsonObject &obj, const QString &lang) const
{
    Lesson l;
    l.id          = obj.value(QStringLiteral("id")).toString();
    l.title       = obj.value(QStringLiteral("title")).toString();
    l.description = obj.value(QStringLiteral("description")).toString();
    l.language    = lang;
    l.layoutHint  = obj.value(QStringLiteral("layoutHint")).toString();
    l.difficulty  = obj.value(QStringLiteral("difficulty")).toInt(1);

    const QJsonArray pa = obj.value(QStringLiteral("passages")).toArray();
    for (const auto &v : pa)
        l.passages.append(v.toString());

    return l;
}
