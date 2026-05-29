#include "StatisticsEngine.h"
#include "ProgressTracker.h"
#include "models/SessionResult.h"

#include <QMap>

StatisticsEngine::StatisticsEngine(ProgressTracker *tracker, QObject *parent)
    : QObject(parent), m_tracker(tracker)
{}

// ── Helpers ───────────────────────────────────────────────────────────────────

static QVariantMap buildSummary(const QList<SessionResult> &sessions)
{
    QVariantMap m;

    qreal totalSec = 0.0;
    qreal sumCpm   = 0.0;
    qreal sumErr   = 0.0;
    qreal bestCpm  = 0.0;

    for (const auto &s : sessions) {
        totalSec += s.durationSec;
        sumCpm   += s.cpm();
        sumErr   += s.errorRate();
        if (s.cpm() > bestCpm) bestCpm = s.cpm();
    }

    const int n = sessions.size();
    m[QStringLiteral("totalSessions")] = n;
    m[QStringLiteral("totalSeconds")]  = totalSec;
    m[QStringLiteral("totalMinutes")]  = totalSec / 60.0;
    m[QStringLiteral("avgCpm")]        = n > 0 ? sumCpm / n : 0.0;
    m[QStringLiteral("avgWpm")]        = n > 0 ? (sumCpm / n) / 5.0 : 0.0;
    m[QStringLiteral("avgErrorRate")]  = n > 0 ? sumErr / n : 0.0;
    m[QStringLiteral("bestCpm")]       = bestCpm;
    m[QStringLiteral("bestWpm")]       = bestCpm / 5.0;
    return m;
}


// ── Public API ────────────────────────────────────────────────────────────────

QVariantMap StatisticsEngine::summaryForRange(const QDateTime &from,
                                               const QDateTime &to) const
{
    return buildSummary(m_tracker->sessionsInRange(from, to));
}

QVariantList StatisticsEngine::dailyDataForRange(const QDateTime &from,
                                                  const QDateTime &to) const
{
    const auto sessions = m_tracker->sessionsInRange(from, to);

    // Group by local date string
    QMap<QString, QList<SessionResult>> byDate;
    for (const auto &s : sessions) {
        const QString dateKey = s.timestamp.toLocalTime().date().toString(Qt::ISODate);
        byDate[dateKey].append(s);
    }

    QVariantList result;
    for (auto it = byDate.cbegin(); it != byDate.cend(); ++it) {
        const auto &list = it.value();
        QVariantMap point;
        point[QStringLiteral("date")] = it.key();

        qreal totalSec = 0.0, sumCpm = 0.0, sumErr = 0.0;
        for (const auto &s : list) {
            totalSec += s.durationSec;
            sumCpm   += s.cpm();
            sumErr   += s.errorRate();
        }
        const int n = list.size();
        point[QStringLiteral("minutes")]      = totalSec / 60.0;
        point[QStringLiteral("avgCpm")]       = sumCpm / n;
        point[QStringLiteral("avgWpm")]       = (sumCpm / n) / 5.0;
        point[QStringLiteral("avgErrorRate")] = sumErr / n;
        result.append(point);
    }
    return result;
}

QVariantMap StatisticsEngine::summaryLastDays(int days) const
{
    const QDateTime to   = QDateTime::currentDateTimeUtc();
    const QDateTime from = to.addDays(-days);
    return summaryForRange(from, to);
}

QVariantList StatisticsEngine::dailyDataLastDays(int days) const
{
    const QDateTime to   = QDateTime::currentDateTimeUtc();
    const QDateTime from = to.addDays(-days);
    return dailyDataForRange(from, to);
}
