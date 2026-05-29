#pragma once

#include <QObject>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QVariantList>

/**
 * @brief Finger assignment and key metadata for a given keyboard layout.
 *
 * Finger numbering matches the diagram:
 *   1 = thumb, 2 = index, 3 = middle, 4 = ring, 5 = pinky
 * Hand: 0 = left, 1 = right.
 */
class KeyboardLayout : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString  currentLayoutId READ currentLayoutId WRITE setCurrentLayoutId
               NOTIFY layoutChanged)
    Q_PROPERTY(QStringList availableLayouts READ availableLayouts CONSTANT)

public:
    struct KeyInfo {
        QString keyId;      ///< Unique key identifier, e.g. "Q", "Backspace"
        int     finger = 2; ///< 1–5
        int     hand   = 0; ///< 0 = left, 1 = right
        int     row    = 0; ///< 0 = number row … 3 = space row
        int     col    = 0; ///< Column index within row
    };

    explicit KeyboardLayout(QObject *parent = nullptr);

    QString     currentLayoutId() const;
    void        setCurrentLayoutId(const QString &id);
    QStringList availableLayouts() const;

    /** Returns finger [1..5] for the character, -1 if unknown. */
    Q_INVOKABLE int fingerForChar(const QString &ch) const;

    /** Returns hand [0=left, 1=right] for the character, -1 if unknown. */
    Q_INVOKABLE int handForChar(const QString &ch) const;

    /** Full key info list for QML (used to render the keyboard). */
    Q_INVOKABLE QVariantList keyInfoList() const;

    Q_INVOKABLE int shiftHandForChar(const QString &ch) const;

    Q_INVOKABLE QString baseKeyForChar(const QString &ch) const;

signals:
    void layoutChanged();

private:
    void loadLayout(const QString &id);
    void buildQwertyLayout();
    void buildYcukenLayout();
    void buildColemakLayout();
    void buildWorkmanLayout();
    void buildDvorakLayout();
    void buildRuMachLayout();
    void buildRuMacLayout();
    void buildYavertyLayout();

    // Вспомогательные методы, которые мы вызывали из .cpp
    void buildQwertyRow0();
    void buildStandardRow4();
    void addKey(const QString &keyId, int row, int col, int finger, int hand);

    void addShiftChar(const QString &shiftChar, const QString &baseKeyId);
    void buildQwertyShiftMap();
    void buildYcukenShiftMap();

    QString              m_currentLayoutId;
    QList<KeyInfo>       m_keys;
    QHash<QString, int>  m_charToKeyIndex; ///< char → index in m_keys
    QHash<QString, QString> m_shiftCharToBaseKey; // '!' -> "1", '@' -> "2" etc.
};
