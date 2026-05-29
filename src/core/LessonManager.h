#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <QJsonObject>
#include <QRandomGenerator>

/**
 * @brief One lesson / exercise loaded from a JSON file.
 */
struct Lesson {
    QString id;
    QString title;
    QString description;
    QString language;   ///< "ru" | "en"
    QString layoutHint; ///< suggested layout, e.g. "qwerty"
    int     difficulty; ///< 1 = beginner … 5 = expert
    QStringList passages; ///< ordered list of text passages
};

/**
 * @brief Loads, caches and provides lessons from JSON files.
 *
 * JSON schema (per file):
 * {
 *   "language": "en",
 *   "lessons": [
 *     {
 *       "id": "en_home_row",
 *       "title": "Home row",
 *       "description": "...",
 *       "layoutHint": "qwerty",
 *       "difficulty": 1,
 *       "passages": ["asdf jkl;", "asdfjkl; asdfjkl;", ...]
 *     }
 *   ]
 * }
 */
class LessonManager : public QObject
{
    Q_OBJECT

public:
    explicit LessonManager(QObject *parent = nullptr);

    /** Load all lessons for the given language from the embedded resource. */
    Q_INVOKABLE bool loadForLanguage(const QString &lang);
    Q_INVOKABLE QStringList passagesForLesson(const QString &id) const;
    Q_INVOKABLE QString randomPassage(const QString &lang,
                                   const QString &layoutHint = QString{},
                                   const QString &exclude    = QString{}) const;
    Q_INVOKABLE QString layoutHintForLesson(const QString &lessonId) const;

    const QList<Lesson> &lessons() const;
    const Lesson *lessonById(const QString &id) const;

signals:
    void lessonsLoaded();

private:
    Lesson lessonFromJson(const QJsonObject &obj, const QString &lang) const;

    QList<Lesson> m_lessons;
};
// добавь внутрь класса перед signals:
