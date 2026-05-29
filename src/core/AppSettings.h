#pragma once
#include <QObject>
#include <QSettings>

class AppSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString layoutId       READ layoutId       WRITE setLayoutId       NOTIFY layoutIdChanged)
    Q_PROPERTY(QString interfaceLanguage READ interfaceLanguage WRITE setInterfaceLanguage NOTIFY interfaceLanguageChanged)
    Q_PROPERTY(QString lessonLanguage READ lessonLanguage WRITE setLessonLanguage NOTIFY lessonLanguageChanged)
    Q_PROPERTY(bool showKeyboard      READ showKeyboard   WRITE setShowKeyboard   NOTIFY showKeyboardChanged)
    Q_PROPERTY(bool colorByFinger     READ colorByFinger  WRITE setColorByFinger  NOTIFY colorByFingerChanged)
    Q_PROPERTY(bool showFingerHint    READ showFingerHint WRITE setShowFingerHint NOTIFY showFingerHintChanged)
    Q_PROPERTY(bool highlightNextKey  READ highlightNextKey WRITE setHighlightNextKey NOTIFY highlightNextKeyChanged)

public:
    explicit AppSettings(QObject *parent = nullptr);

    Q_INVOKABLE QStringList layoutsForLanguage(const QString &language) const;

    QString layoutId()        const;
    QString interfaceLanguage()  const;
    QString lessonLanguage() const;
    bool    showKeyboard()    const;
    bool    colorByFinger()   const;
    bool    showFingerHint()  const;
    bool    highlightNextKey() const;

    void setLayoutId(const QString &v);
    void setInterfaceLanguage(const QString &v);
    void setLessonLanguage(const QString &v);
    void setShowKeyboard(bool v);
    void setColorByFinger(bool v);
    void setShowFingerHint(bool v);
    void setHighlightNextKey(bool v);

signals:
    void layoutIdChanged();
    void interfaceLanguageChanged();
    void lessonLanguageChanged();
    void showKeyboardChanged();
    void colorByFingerChanged();
    void showFingerHintChanged();
    void highlightNextKeyChanged();

private:
    QSettings m_s;
};