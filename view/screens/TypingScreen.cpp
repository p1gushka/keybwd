#include "TypingScreen.h"
#include <QVBoxLayout>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QDebug>

TypingScreen::TypingScreen(QWidget *parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);

    textDisplay = new QTextEdit(this);
    textDisplay->setReadOnly(true);

    inputField = new QTextEdit(this);
    finishButton = new QPushButton("Завершить", this);

    layout->addWidget(textDisplay);
    layout->addWidget(inputField);
    layout->addWidget(finishButton);

    connect(inputField, &QTextEdit::textChanged, this, &TypingScreen::onTextChanged);
    connect(finishButton, &QPushButton::clicked, this, &TypingScreen::onFinishClicked);
}

void TypingScreen::setTargetText(const QString &text) {
    targetText = text;
    textDisplay->setPlainText(text);
    inputField->clear();

    stats = SessionStats();
    timerTyping.start();
    timerRaw.start();
    rawActive = true;
}

const SessionStats& TypingScreen::getStats() const {
    return stats;
}

void TypingScreen::onTextChanged() {
    QString input = inputField->toPlainText();
    updateStats(input);
    applyHighlighting(input);
}

void TypingScreen::onFinishClicked() {
    stats.typingTimeMs = timerTyping.elapsed();
    stats.rawTimeMs = rawActive ? timerRaw.elapsed() : timerTyping.elapsed();
    
    emit typingFinished();  // Используем единый сигнал
}

void TypingScreen::updateStats(const QString &input) {
    stats.correctChars = 0;
    stats.incorrectChars = 0;
    stats.extraChars = 0;
    stats.missedChars = 0;

    rawActive = true;

    for (int i = 0; i < input.length(); ++i) {
        if (i < targetText.length()) {
            if (input[i] == targetText[i]) {
                stats.correctChars++;
            } else {
                stats.incorrectChars++;
                rawActive = false;
            }
        } else {
            stats.extraChars++;
            rawActive = false;
        }
    }

    if (input.length() < targetText.length()) {
        stats.missedChars = targetText.length() - input.length();
    }

    stats.typingTimeMs = timerTyping.elapsed();
    if (rawActive)
        stats.rawTimeMs = timerRaw.elapsed();
}

void TypingScreen::applyHighlighting(const QString &input) {
    QTextDocument *doc = textDisplay->document();
    QTextCursor cursor(doc);
    cursor.select(QTextCursor::Document);
    cursor.setCharFormat(QTextCharFormat());  // reset format

    for (int i = 0; i < targetText.length(); ++i) {
        QTextCharFormat fmt;
        if (i < input.length()) {
            if (input[i] == targetText[i]) {
                fmt.setForeground(Qt::darkGreen);
            } else {
                fmt.setForeground(Qt::red);
            }
        } else {
            fmt.setForeground(Qt::black);
        }

        cursor.setPosition(i);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        cursor.setCharFormat(fmt);
    }
}

void TypingScreen::setText(const QString &text) {
    targetText = text;
    textDisplay->setPlainText(text);
    inputField->clear();

    stats = SessionStats();  // Используем stats вместо currentStats
    timerTyping.start();     // Используем timerTyping вместо typingTimer
    timerRaw.start();        // Используем timerRaw вместо rawTimer
    rawActive = true;

    connect(inputField, &QTextEdit::textChanged, this, &TypingScreen::onTextChanged);
}

SessionStats TypingScreen::getSessionStats() const {
    SessionStats result;
    result.correctChars = stats.correctChars;       // Используем stats
    result.incorrectChars = stats.incorrectChars;   // Используем stats
    result.extraChars = stats.extraChars;           // Используем stats
    result.missedChars = stats.missedChars;         // Используем stats
    result.typingTimeMs = timerTyping.elapsed();    // Используем timerTyping
    result.rawTimeMs = rawActive ? timerRaw.elapsed() : timerTyping.elapsed();
    return result;
}