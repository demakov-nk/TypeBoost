#pragma once

#include <QObject>
#include <QDateTime>
#include <QVariantList>
#include <QVariantMap>

class ProgressTracker;

/**
 * @brief Computes aggregated statistics from raw session data.
 *
 * All computations are done on-demand from ProgressTracker's in-memory list,
 * so no caching is needed for now.
 *
 * Exposed to QML as "statisticsEngine" context property.
 */
class StatisticsEngine : public QObject
{
    Q_OBJECT

public:
    explicit StatisticsEngine(ProgressTracker *tracker, QObject *parent = nullptr);

    /**
     * @brief Summary stats for a time range.
     *
     * Returns a QVariantMap with keys:
     *   totalSessions  : int
     *   totalMinutes   : qreal
     *   avgCpm         : qreal
     *   avgWpm         : qreal
     *   avgErrorRate   : qreal
     *   bestCpm        : qreal
     *   bestWpm        : qreal
     */
    Q_INVOKABLE QVariantMap summaryForRange(const QDateTime &from,
                                            const QDateTime &to) const;

    /**
     * @brief Daily data points for charting.
     *
     * Returns a QVariantList of QVariantMaps:
     *   date       : QString  (ISO date, e.g. "2024-01-15")
     *   minutes    : qreal
     *   avgCpm     : qreal
     *   avgWpm     : qreal
     *   avgErrorRate: qreal
     */
    Q_INVOKABLE QVariantList dailyDataForRange(const QDateTime &from,
                                               const QDateTime &to) const;

    /** Convenience: last N days summary. */
    Q_INVOKABLE QVariantMap summaryLastDays(int days) const;

    /** Convenience: daily data for last N days. */
    Q_INVOKABLE QVariantList dailyDataLastDays(int days) const;

private:
    ProgressTracker *m_tracker; // non-owning
};
