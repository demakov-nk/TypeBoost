#include "TypingSession.h"
#include "ProgressTracker.h"

#include <QDateTime>

TypingSession::TypingSession(ProgressTracker *tracker, QObject *parent)
    : QObject(parent), m_tracker(tracker)
{}

void TypingSession::setKeyboardLayout(KeyboardLayout *layout)
{
    m_keyboardLayout = layout;
}

// ── Getters ───────────────────────────────────────────────────────────────────

QString  TypingSession::targetText()  const { return m_targetText; }
QString  TypingSession::inputText()   const { return m_inputText; }
bool     TypingSession::isActive()    const { return m_active; }
bool     TypingSession::isPaused() const { return m_paused; }
qreal    TypingSession::currentCpm()  const { return m_cpm; }
qreal    TypingSession::currentWpm()  const { return m_cpm / 5.0; }
qreal    TypingSession::errorRate()   const { return m_errorRate; }
int      TypingSession::errorCount()  const { return m_errorCount; }
QString   TypingSession::ownerTag() const { return m_ownerTag; }

QString TypingSession::prevChar() const
{
    const int pos = m_inputText.length();
    if (pos > 0)
        return QString(m_targetText.at(pos - 1));
    return QString();
}

bool TypingSession::needsShift() const
{
    const int pos = m_inputText.length();
    if (pos >= m_targetText.length()) return false;
    const QChar ch = m_targetText.at(pos);
    if (ch.isLetter() && ch.isUpper()) return true;

    if (m_keyboardLayout) {
        const QString chStr(ch);
        const QString base = m_keyboardLayout->baseKeyForChar(chStr);
        if (base != chStr.toLower())   // есть в shiftMap → требует Shift
            return true;
    }

    static const QString shiftChars = QStringLiteral("~!@#$%^&*()_+{}|:\"<>?№");
    return shiftChars.contains(ch);
}

int TypingSession::cursorPos() const
{
    return m_inputText.length();
}

QString TypingSession::nextChar() const
{
    const int pos = m_inputText.length();
    if (pos < m_targetText.length())
        return QString(m_targetText.at(pos));
    return {};
}

// ── Public API ────────────────────────────────────────────────────────────────

void TypingSession::start(const QString &text,
                          const QString &lessonId,
                          const QString &language,
                          const QString &layoutId,
                          const QString &ownerTag)
{
    qDebug() << "[TypingSession] start() ownerTag=" << ownerTag
             << "text=" << text.left(30);

    m_targetText    = text;
    m_inputText     = "";
    m_errorPositions.clear();
    m_lessonId      = lessonId;
    m_language      = language;
    m_layoutId      = layoutId;
    m_ownerTag      = ownerTag;
    m_pausedElapsed = 0;
    m_paused        = false;
    m_active        = true;
    m_timerStarted  = false;
    m_cpm           = 0.0;
    m_errorRate     = 0.0;

    m_timer.invalidate();

    emit ownerTagChanged();
    emit targetTextChanged();
    emit inputTextChanged();
    emit cursorPosChanged();
    emit activeChanged();
    emit pausedChanged();
    emit statsUpdated();
}

void TypingSession::pause()
{
    if (!m_active || m_paused) return;
    m_paused = true;
    if (m_timerStarted) {
        m_pausedElapsed += m_timer.elapsed();
        m_timer.invalidate();
    }
    qDebug() << "[TypingSession] pause() ownerTag=" << m_ownerTag
             << "elapsed=" << m_pausedElapsed;
    emit pausedChanged();
}

void TypingSession::resume()
{
    if (!m_active || !m_paused) return;
    m_paused = false;
    m_timer.restart();   // ← перезапускаем таймер
    qDebug() << "[TypingSession] resume() ownerTag=" << m_ownerTag;
    emit pausedChanged();
}

void TypingSession::abort()
{
    if (!m_active) return;
    m_active = false;
    m_inputText.clear();
    emit activeChanged();
    emit inputTextChanged();
}

// ── Input handling ────────────────────────────────────────────────────────────

void TypingSession::setInputText(const QString &text)
{
    if (!m_active || m_paused) return;

    // ── Ограничиваем длину ввода длиной эталона ──────────────────────────────
    const QString limited = text.left(m_targetText.length());
    if (m_inputText == limited) return;  // ← сравниваем с обрезанной версией

    if (!m_timerStarted && !limited.isEmpty()) {
        m_timerStarted = true;
        m_timer.restart();
        qDebug() << "[TypingSession] timer started ownerTag=" << m_ownerTag;
    }
    if (limited.length() > m_inputText.length()) {
        const int newPos = limited.length() - 1;
        if (newPos < m_targetText.length()) {
            if (limited.at(newPos) != m_targetText.at(newPos)) {
                m_errorPositions.insert(newPos);  // ← добавляем позицию, не счётчик
            }
        }
    }


    m_inputText = limited;  // ← сохраняем обрезанную версию
    emit inputTextChanged();
    emit cursorPosChanged();

    updateStats();
    checkCompletion();
}

// ── Private ───────────────────────────────────────────────────────────────────

void TypingSession::updateStats()
{
    if (!m_timerStarted) return;
    if (!m_timer.isValid()) return;

    const qreal elapsed = (m_pausedElapsed + m_timer.elapsed()) / 1000.0;
    if (elapsed <= 0.0) return;

    // Считаем верно введённые символы по текущему состоянию (для CPM)
    int correct = 0;
    const int len = qMin(m_inputText.length(), m_targetText.length());
    for (int i = 0; i < len; ++i) {
        if (m_inputText.at(i) == m_targetText.at(i))
            ++correct;
    }

    m_cpm = (correct / elapsed) * 60.0;

    // Процент ошибок — накопленный, не падает при исправлении
    const int total = m_targetText.length();
    m_errorRate = (total > 0)
        ? (static_cast<qreal>(m_errorPositions.size()) / total * 100.0)
        : 0.0;

    emit statsUpdated();
}


void TypingSession::checkCompletion()
{
    if (m_inputText.length() < m_targetText.length()) return;
    if (m_inputText != m_targetText) return;

    // Session complete
    m_active = false;
    const qreal elapsed = m_timerStarted
        ? ((m_pausedElapsed + m_timer.elapsed()) / 1000.0)
        : 0.0;

    SessionResult result;
    result.timestamp    = QDateTime::currentDateTimeUtc();
    result.lessonId     = m_lessonId;
    result.language     = m_language;
    result.layoutId     = m_layoutId;
    result.totalChars   = m_targetText.length();
    result.correctChars = m_targetText.length() - m_errorPositions.size();
    result.errorCount   = m_errorPositions.size();  // уникальные позиции с ошибкой
    result.durationSec  = elapsed;


    if (m_tracker)
        m_tracker->record(result);

    emit sessionFinished(result);
    emit activeChanged();
}
