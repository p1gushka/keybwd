#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QElapsedTimer>
#include <QLabel>
#include <QVBoxLayout>
#include "model/SessionStats.h"

class TypingScreen : public QWidget
{
    Q_OBJECT

public:
    explicit TypingScreen(QWidget *parent = nullptr);
    void setTargetText(const QString &text);
    const SessionStats &getStats() const;
    void setText(const QString &text);
    SessionStats getSessionStats() const;
    void setTimeLimit(int seconds); // Для режима времени
    void setWordCount(int count);   // Для режима слов
    void reset();

signals:
    void typingFinished();

private slots:
    void onTextChanged();
    void onFinishClicked();
    void updateTimer(); // Обновление таймера

private:
    QTextEdit *inputField;
    QTextEdit *textDisplay;
    QPushButton *finishButton;
    QLabel *timerLabel;   // Отображение таймера
    QLabel *counterLabel; // Отображение счетчика слов

    QString targetText;
    SessionStats stats;
    QElapsedTimer timerTyping;
    QElapsedTimer timerRaw;
    QTimer *countdownTimer; // Таймер обратного отсчета

    bool rawActive = true;
    int timeLimit = 0; // Ограничение времени в секундах
    int wordCount = 0; // Целевое количество слов
    int initialWordCount = 0;
    bool isFirstKeyPress;

    void updateStats(const QString &input);
    void applyHighlighting(const QString &input);
    void startCountdown();                        // Запуск обратного отсчета
    void updateWordCounter(const QString &input); // Обновление счетчика слов
};
