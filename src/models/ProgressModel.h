#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include "models/SessionResult.h"

class StatisticsEngine;
class ProgressTracker;

/**
 * @brief List model of daily aggregated stats for the ProgressPage chart.
 *
 * Roles: date, minutes, avgCpm, avgWpm, avgErrorRate
 */
class ProgressModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int rangeDays READ rangeDays WRITE setRangeDays NOTIFY rangeDaysChanged)

public:
    enum Roles {
        DateRole = Qt::UserRole + 1,
        MinutesRole,
        AvgCpmRole,
        AvgWpmRole,
        AvgErrorRateRole,
    };

    explicit ProgressModel(StatisticsEngine *engine, QObject *parent = nullptr);

    int      rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int  rangeDays() const;
    void setRangeDays(int days);

    Q_INVOKABLE void refresh();

    Q_INVOKABLE QVariantMap summary() const
    {
        qreal totalMin = 0, totalWpm = 0, totalErr = 0;
        int   sessions = m_points.size();
        for (const auto &p : m_points) {
            totalMin += p.minutes;
            totalWpm += p.avgWpm;
            totalErr += p.avgErrorRate;
        }
        return {
            { "totalSessions", sessions },
            { "totalMinutes",  totalMin },
            { "avgWpm",        sessions > 0 ? totalWpm / sessions : 0.0 },
            { "avgErrorRate",  sessions > 0 ? totalErr / sessions : 0.0 },
        };
    }

signals:
    void rangeDaysChanged();

private:
    StatisticsEngine *m_engine; // non-owning
    int               m_rangeDays = 7;

    struct DayPoint {
        QString date;
        qreal   minutes      = 0;
        qreal   avgCpm       = 0;
        qreal   avgWpm       = 0;
        qreal   avgErrorRate = 0;
    };
    QList<DayPoint> m_points;
};
