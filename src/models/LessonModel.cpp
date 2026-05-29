#include "LessonModel.h"

LessonModel::LessonModel(LessonManager *manager, QObject *parent)
    : QAbstractListModel(parent), m_manager(manager)
{
    connect(m_manager, &LessonManager::lessonsLoaded, this, [this] {
        beginResetModel();
        endResetModel();
    });
}

int LessonModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_manager->lessons().size();
}

QVariant LessonModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_manager->lessons().size())
        return {};

    const Lesson &l = m_manager->lessons().at(index.row());
    switch (role) {
    case IdRole:          return l.id;
    case TitleRole:       return l.title;
    case DescriptionRole: return l.description;
    case LanguageRole:    return l.language;
    case LayoutHintRole:  return l.layoutHint;
    case DifficultyRole:  return l.difficulty;
    case PassageCountRole:return l.passages.size();
    default:              return {};
    }
}

QHash<int, QByteArray> LessonModel::roleNames() const
{
    return {
        { IdRole,           "lessonId"     },
        { TitleRole,        "title"        },
        { DescriptionRole,  "description"  },
        { LanguageRole,     "language"     },
        { LayoutHintRole,   "layoutHint"   },
        { DifficultyRole,   "difficulty"   },
        { PassageCountRole, "passageCount" },
    };
}
