#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QElapsedTimer>
#include "model/SessionStats.h"

class TypingScreen : public QWidget {
    Q_OBJECT

public:
    explicit TypingScreen(QWidget *parent = nullptr);
    void setTargetText(const QString &text);
    const SessionStats& getStats() const;
    void setText(const QString &text);
    SessionStats getSessionStats() const; 

signals:
    void typingFinished(); 

private slots:
    void onTextChanged();
    void onFinishClicked();

private:
    QTextEdit *inputField;
    QTextEdit *textDisplay;
    QPushButton *finishButton;

    QString targetText;
    SessionStats stats;
    QElapsedTimer timerTyping;
    QElapsedTimer timerRaw;
    bool rawActive = true;

    void updateStats(const QString& input);
    void applyHighlighting(const QString& input);
};
