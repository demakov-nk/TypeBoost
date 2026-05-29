#pragma once

#include <QAbstractListModel>
#include "core/LessonManager.h"

/**
 * @brief QAbstractListModel wrapper around LessonManager's lesson list.
 *
 * Roles exposed to QML:
 *   id, title, description, language, layoutHint, difficulty, passageCount
 */
class LessonModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        DescriptionRole,
        LanguageRole,
        LayoutHintRole,
        DifficultyRole,
        PassageCountRole,
    };

    explicit LessonModel(LessonManager *manager, QObject *parent = nullptr);

    int      rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    LessonManager *m_manager; // non-owning
};
