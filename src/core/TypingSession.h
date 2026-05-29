#pragma once

#include <QObject>
#include <QString>
#include <QElapsedTimer>
#include <QSet>
#include "models/SessionResult.h"
#include "KeyboardLayout.h"

class ProgressTracker;

/**
 * @brief Manages the state of one active typing exercise.
 *
 * Lifecycle:
 *   start(text)  →  user types  →  sessionFinished() emitted  →  start() again
 *
 * Exposed to QML as "typingSession" context property.
 */
class TypingSession : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString  targetText    READ targetText    NOTIFY targetTextChanged)
    Q_PROPERTY(QString  inputText     READ inputText     WRITE  setInputText    NOTIFY inputTextChanged)
    Q_PROPERTY(int      cursorPos     READ cursorPos     NOTIFY cursorPosChanged)
    Q_PROPERTY(bool     active        READ isActive      NOTIFY activeChanged)
    Q_PROPERTY(qreal    currentCpm    READ currentCpm    NOTIFY statsUpdated)
    Q_PROPERTY(qreal    currentWpm    READ currentWpm    NOTIFY statsUpdated)
    Q_PROPERTY(qreal    errorRate     READ errorRate     NOTIFY statsUpdated)
    Q_PROPERTY(QString  nextChar      READ nextChar      NOTIFY cursorPosChanged)
    Q_PROPERTY(int      errorCount    READ errorCount    NOTIFY statsUpdated)
    Q_PROPERTY(bool     paused        READ isPaused      NOTIFY pausedChanged)
    Q_PROPERTY(QString  ownerTag      READ ownerTag      NOTIFY ownerTagChanged)
    Q_PROPERTY(QString prevChar    READ prevChar    NOTIFY cursorPosChanged)
    Q_PROPERTY(bool    needsShift  READ needsShift  NOTIFY cursorPosChanged)

public:
    explicit TypingSession(ProgressTracker *tracker, QObject *parent = nullptr);
    void setKeyboardLayout(KeyboardLayout *layout);

    // ── Getters ───────────────────────────────────────────────────────────────
    QString  targetText()  const;
    QString  inputText()   const;
    int      cursorPos()   const;
    bool     isActive()    const;
    bool     isPaused() const;
    qreal    currentCpm()  const;
    qreal    currentWpm()  const;
    qreal    errorRate()   const;
    QString  nextChar()    const;
    int      errorCount()  const;
    QString  ownerTag() const;
    QString prevChar()   const;
    bool    needsShift() const;

public slots:
    /** Begin a new session with the provided target text. */
    Q_INVOKABLE void start(const QString &text, const QString &lessonId,
                           const QString &language, const QString &layoutId,
                           const QString &ownerTag = QString());

    void pause();
    void resume();

    /** Abort the current session without saving a result. */
    void abort();

    Q_INVOKABLE void setInputText(const QString &text);

signals:
    void targetTextChanged();
    void inputTextChanged();
    void cursorPosChanged();
    void activeChanged();
    void pausedChanged();
    void statsUpdated();
    void sessionFinished(SessionResult result);
    void ownerTagChanged();

private:
    void checkCompletion();
    void updateStats();

    ProgressTracker *m_tracker;   // non-owning
    KeyboardLayout *m_keyboardLayout = nullptr;

    QString m_targetText;
    QString m_inputText;
    QString m_lessonId;
    QString m_language;
    QString m_layoutId;
    QString m_ownerTag;

    int  m_errorCount  = 0;
    bool m_active      = false;
    bool m_timerStarted = false;
    bool m_paused = false;
    qint64 m_pausedElapsed = 0; // сколько прошло до паузы

    QSet<int> m_errorPositions;  // позиции где пользователь хоть раз ошибся


    QElapsedTimer m_timer;

    // Cached stats
    qreal m_cpm       = 0.0;
    qreal m_errorRate = 0.0;
};
