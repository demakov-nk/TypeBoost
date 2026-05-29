#include "ProgressTracker.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

static constexpr int kFileVersion = 1;

ProgressTracker::ProgressTracker(QObject *parent)
    : QObject(parent)
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    m_filePath = dir + QStringLiteral("/progress.json");
    load();
}

void ProgressTracker::record(const SessionResult &result)
{
    m_sessions.append(result);
    save();
    emit newResultRecorded(result);
}

const QList<SessionResult> &ProgressTracker::allSessions() const
{
    return m_sessions;
}

QList<SessionResult> ProgressTracker::sessionsInRange(const QDateTime &from,
                                                       const QDateTime &to) const
{
    QList<SessionResult> result;
    for (const auto &s : m_sessions) {
        if (s.timestamp >= from && s.timestamp <= to)
            result.append(s);
    }
    return result;
}

// ── I/O ───────────────────────────────────────────────────────────────────────

bool ProgressTracker::load()
{
    QFile file(m_filePath);
    if (!file.exists()) return true; // first run — nothing to load

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "ProgressTracker: cannot open" << m_filePath;
        return false;
    }

    QJsonParseError err;
    const auto doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "ProgressTracker: JSON error:" << err.errorString();
        return false;
    }

    const QJsonObject root = doc.object();
    const QJsonArray  arr  = root.value(QStringLiteral("sessions")).toArray();

    m_sessions.clear();
    m_sessions.reserve(arr.size());
    for (const auto &v : arr)
        m_sessions.append(fromJson(v.toObject()));

    return true;
}

bool ProgressTracker::save() const
{
    QJsonArray arr;
    for (const auto &s : m_sessions)
        arr.append(toJson(s));

    QJsonObject root;
    root[QStringLiteral("version")]  = kFileVersion;
    root[QStringLiteral("sessions")] = arr;

    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "ProgressTracker: cannot write" << m_filePath;
        return false;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

SessionResult ProgressTracker::fromJson(const QJsonObject &obj)
{
    SessionResult r;
    r.timestamp    = QDateTime::fromString(obj.value(QStringLiteral("timestamp")).toString(),
                                           Qt::ISODate);
    r.lessonId     = obj.value(QStringLiteral("lessonId")).toString();
    r.language     = obj.value(QStringLiteral("language")).toString();
    r.layoutId     = obj.value(QStringLiteral("layoutId")).toString();
    r.totalChars   = obj.value(QStringLiteral("totalChars")).toInt();
    r.correctChars = obj.value(QStringLiteral("correctChars")).toInt();
    r.errorCount   = obj.value(QStringLiteral("errorCount")).toInt();
    r.durationSec  = obj.value(QStringLiteral("durationSec")).toDouble();
    return r;
}

QJsonObject ProgressTracker::toJson(const SessionResult &r)
{
    QJsonObject obj;
    obj[QStringLiteral("timestamp")]    = r.timestamp.toString(Qt::ISODate);
    obj[QStringLiteral("lessonId")]     = r.lessonId;
    obj[QStringLiteral("language")]     = r.language;
    obj[QStringLiteral("layoutId")]     = r.layoutId;
    obj[QStringLiteral("totalChars")]   = r.totalChars;
    obj[QStringLiteral("correctChars")] = r.correctChars;
    obj[QStringLiteral("errorCount")]   = r.errorCount;
    obj[QStringLiteral("durationSec")]  = r.durationSec;
    return obj;
}
