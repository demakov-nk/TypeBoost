#include "ProgressModel.h"
#include "core/StatisticsEngine.h"

ProgressModel::ProgressModel(StatisticsEngine *engine, QObject *parent)
    : QAbstractListModel(parent), m_engine(engine)
{
    refresh();
}

int ProgressModel::rangeDays() const { return m_rangeDays; }

void ProgressModel::setRangeDays(int days)
{
    if (m_rangeDays == days) return;
    m_rangeDays = days;
    emit rangeDaysChanged();
    refresh();
}

void ProgressModel::refresh()
{
    beginResetModel();
    m_points.clear();

    // ИСПРАВЛЕНО: Считаем диапазон дат от "N дней назад" до текущего момента
    QDateTime to = QDateTime::currentDateTimeUtc();
    QDateTime from = to.addDays(-m_rangeDays);

    const QVariantList raw = m_engine->dailyDataForRange(from, to);
    m_points.reserve(raw.size());
    for (const auto &v : raw) {
        const QVariantMap map = v.toMap();
        DayPoint p;
        p.date          = map.value(QStringLiteral("date")).toString();
        p.minutes       = map.value(QStringLiteral("minutes")).toDouble();
        p.avgCpm        = map.value(QStringLiteral("avgCpm")).toDouble();
        p.avgWpm        = map.value(QStringLiteral("avgWpm")).toDouble();
        p.avgErrorRate  = map.value(QStringLiteral("avgErrorRate")).toDouble();
        m_points.append(p);
    }

    endResetModel();
}

int ProgressModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_points.size();
}

QVariant ProgressModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_points.size()) return {};
    const DayPoint &p = m_points.at(index.row());
    switch (role) {
    case DateRole:         return p.date;
    case MinutesRole:      return p.minutes;
    case AvgCpmRole:       return p.avgCpm;
    case AvgWpmRole:       return p.avgWpm;
    case AvgErrorRateRole: return p.avgErrorRate;
    default:               return {};
    }
}

QHash<int, QByteArray> ProgressModel::roleNames() const
{
    return {
        { DateRole,         "date"         },
        { MinutesRole,      "minutes"      },
        { AvgCpmRole,       "avgCpm"       },
        { AvgWpmRole,       "avgWpm"       },
        { AvgErrorRateRole, "avgErrorRate" },
    };
}