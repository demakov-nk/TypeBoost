#pragma once

#include <QObject>
#include <QList>
#include "models/SessionResult.h"

/**
 * @brief Persists and retrieves typing session results using a JSON file.
 *
 * File location:
 *   QStandardPaths::AppDataLocation / "progress.json"
 *
 * Format:
 * {
 *   "version": 1,
 *   "sessions": [
 *     {
 *       "timestamp": "2024-01-15T14:23:00Z",
 *       "lessonId": "en_home_row",
 *       "language": "en",
 *       "layoutId": "qwerty",
 *       "totalChars": 40,
 *       "correctChars": 38,
 *       "errorCount": 3,
 *       "durationSec": 12.5
 *     }, ...
 *   ]
 * }
 */
class ProgressTracker : public QObject
{
    Q_OBJECT

public:
    explicit ProgressTracker(QObject *parent = nullptr);

    /** Append a result and immediately persist to disk. */
    void record(const SessionResult &result);

    /** All recorded sessions, ordered oldest→newest. */
    const QList<SessionResult> &allSessions() const;

    /** Sessions within [from, to] UTC range. */
    QList<SessionResult> sessionsInRange(const QDateTime &from,
                                         const QDateTime &to) const;

signals:
    void newResultRecorded(SessionResult result);

private:
    bool load();
    bool save() const;

    static SessionResult fromJson(const QJsonObject &obj);
    static QJsonObject   toJson(const SessionResult &r);

    QString             m_filePath;
    QList<SessionResult> m_sessions;
};
