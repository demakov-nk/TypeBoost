#pragma once

#include <QString>
#include <QDateTime>

/**
 * @brief Immutable result of one completed typing session (one text passage).
 *
 * Stored as a JSON object inside progress.json by ProgressTracker.
 */
struct SessionResult
{
    QDateTime timestamp;       ///< When the session ended (UTC)
    QString   lessonId;        ///< Lesson identifier, empty for free practice
    QString   language;        ///< "ru" | "en"
    QString   layoutId;        ///< e.g. "qwerty", "йцукен"

    int   totalChars   = 0;    ///< Characters in the target text
    int   correctChars = 0;    ///< Correctly typed characters
    int   errorCount   = 0;    ///< Cumulative incorrect key presses
    qreal durationSec  = 0.0;  ///< Elapsed seconds (start → last correct char)

    /** Characters per minute — only correct chars count. */
    qreal cpm() const
    {
        if (durationSec <= 0.0) return 0.0;
        return (correctChars / durationSec) * 60.0;
    }

    /** Words per minute (CPM / 5, standard definition). */
    qreal wpm() const { return cpm() / 5.0; }

    /** Error rate in percent [0, 100]. */
    qreal errorRate() const
    {
        if (totalChars == 0) return 0.0;
        return static_cast<qreal>(errorCount) / totalChars * 100.0;
    }
};
