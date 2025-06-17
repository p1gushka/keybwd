#include "TypingScreen.h"
#include <QVBoxLayout>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QDebug>
#include <QTimer>
#include <QHBoxLayout>
#include <QRegularExpression>

TypingScreen::TypingScreen(QWidget *parent) : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);

    // Панель информации (таймер/счетчик)
    auto infoLayout = new QHBoxLayout();
    timerLabel = new QLabel("00:00", this);
    counterLabel = new QLabel("", this);
    infoLayout->addWidget(timerLabel);
    infoLayout->addStretch();
    infoLayout->addWidget(counterLabel);
    mainLayout->addLayout(infoLayout);

    textDisplay = new QTextEdit(this);
    textDisplay->setReadOnly(true);
    textDisplay->setFont(QFont("Courier New", 18)); // Моноширинный шрифт

    inputField = new QTextEdit(this);
    inputField->setFont(QFont("Courier New", 18)); // Моноширинный шрифт

    finishButton = new QPushButton("Завершить", this);

    mainLayout->addWidget(textDisplay);
    mainLayout->addWidget(inputField);
    mainLayout->addWidget(finishButton);

    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &TypingScreen::updateTimer);

    connect(inputField, &QTextEdit::textChanged, this, &TypingScreen::onTextChanged);
    connect(finishButton, &QPushButton::clicked, this, &TypingScreen::onFinishClicked);
}

void TypingScreen::setTimeLimit(int seconds)
{
    timeLimit = seconds;
    if (seconds > 0)
    {
        timerLabel->setText(QString("%1:%2")
                                .arg(timeLimit / 60, 2, 10, QLatin1Char('0'))
                                .arg(timeLimit % 60, 2, 10, QLatin1Char('0')));
        timerLabel->show();
    }
    else
    {
        timerLabel->hide();
    }
}

void TypingScreen::setWordCount(int count)
{
    wordCount = count;
    initialWordCount = count;
    if (count > 0)
    {
        counterLabel->setText(QString("Осталось слов: %1").arg(wordCount));
        counterLabel->show();
    }
    else
    {
        counterLabel->hide();
    }
}

void TypingScreen::setTargetText(const QString &text)
{
    targetText = text;
    textDisplay->setPlainText(text);
    inputField->clear();

    stats = SessionStats();
    timerTyping.start();
    timerRaw.start();
    rawActive = true;

    // Запускаем таймер если установлен лимит времени
    if (timeLimit > 0)
    {
        startCountdown();
    }
}

void TypingScreen::startCountdown()
{
    countdownTimer->start(1000); // Обновление каждую секунду
}

void TypingScreen::updateTimer()
{
    if (timeLimit <= 0)
    {
        countdownTimer->stop();
        onFinishClicked();
        return;
    }

    timeLimit--;
    timerLabel->setText(QString("%1:%2")
                            .arg(timeLimit / 60, 2, 10, QLatin1Char('0'))
                            .arg(timeLimit % 60, 2, 10, QLatin1Char('0')));
}

void TypingScreen::updateWordCounter(const QString &input)
{
    if (wordCount <= 0)
        return;

    int currentWords = input.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).size();
    int remaining = initialWordCount - currentWords;

    if (remaining <= 0)
    {
        wordCount = 0;
        counterLabel->setText("Все слова набраны!");
        QTimer::singleShot(500, this, &TypingScreen::onFinishClicked);
    }
    else
    {
        wordCount = remaining;
        counterLabel->setText(QString("Осталось слов: %1").arg(remaining));
    }
}

const SessionStats &TypingScreen::getStats() const
{
    return stats;
}

void TypingScreen::onTextChanged()
{
    QString input = inputField->toPlainText();

    // Запускаем таймер при первом нажатии
    if (isFirstKeyPress && !input.isEmpty())
    {
        isFirstKeyPress = false;
        if (timeLimit > 0)
        {
            startCountdown();
        }
    }

    updateStats(input);
    applyHighlighting(input);

    if (initialWordCount > 0)
    {
        updateWordCounter(input);
    }

    if (input == targetText)
    {
        QTimer::singleShot(500, this, &TypingScreen::onFinishClicked);
    }
}

void TypingScreen::onFinishClicked()
{
    stats.typingTimeMs = timerTyping.elapsed();
    stats.rawTimeMs = rawActive ? timerRaw.elapsed() : timerTyping.elapsed();

    // Сбрасываем таймеры и лейблы
    countdownTimer->stop();
    timerLabel->setText("");
    counterLabel->setText("");

    emit typingFinished();
}

void TypingScreen::updateStats(const QString &input)
{
    stats.correctChars = 0;
    stats.incorrectChars = 0;
    stats.extraChars = 0;
    stats.missedChars = 0;

    rawActive = true;

    for (int i = 0; i < input.length(); ++i)
    {
        if (i < targetText.length())
        {
            if (input[i] == targetText[i])
            {
                stats.correctChars++;
            }
            else
            {
                stats.incorrectChars++;
                rawActive = false;
            }
        }
        else
        {
            stats.extraChars++;
            rawActive = false;
        }
    }

    if (input.length() < targetText.length())
    {
        stats.missedChars = targetText.length() - input.length();
    }

    stats.typingTimeMs = timerTyping.elapsed();
    if (rawActive)
        stats.rawTimeMs = timerRaw.elapsed();
}

void TypingScreen::applyHighlighting(const QString &input)
{
    QTextDocument *doc = textDisplay->document();
    QTextCursor cursor(doc);
    cursor.select(QTextCursor::Document);
    cursor.setCharFormat(QTextCharFormat()); // reset format

    for (int i = 0; i < targetText.length(); ++i)
    {
        QTextCharFormat fmt;
        if (i < input.length())
        {
            if (input[i] == targetText[i])
            {
                fmt.setForeground(Qt::darkGreen);
            }
            else
            {
                fmt.setForeground(Qt::red);
            }
        }
        else
        {
            fmt.setForeground(Qt::black);
        }

        cursor.setPosition(i);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        cursor.setCharFormat(fmt);
    }
}

void TypingScreen::setText(const QString &text)
{
    targetText = text;
    textDisplay->setPlainText(text);
    inputField->clear();

    stats = SessionStats();
    timerTyping.start();
    timerRaw.start();
    rawActive = true;

    connect(inputField, &QTextEdit::textChanged, this, &TypingScreen::onTextChanged);

    isFirstKeyPress = true;
    inputField->setFocus();
}

SessionStats TypingScreen::getSessionStats() const
{
    SessionStats result;
    result.correctChars = stats.correctChars;     // Используем stats
    result.incorrectChars = stats.incorrectChars; // Используем stats
    result.extraChars = stats.extraChars;         // Используем stats
    result.missedChars = stats.missedChars;       // Используем stats
    result.typingTimeMs = timerTyping.elapsed();  // Используем timerTyping
    result.rawTimeMs = rawActive ? timerRaw.elapsed() : timerTyping.elapsed();
    return result;
}

void TypingScreen::reset()
{
    inputField->clear();
    textDisplay->clear();

    timeLimit = 0;
    wordCount = 0;
    initialWordCount = 0;

    countdownTimer->stop();
    isFirstKeyPress = true;
    rawActive = true;

    stats = SessionStats();

    if (!targetText.isEmpty())
    {
        setText(targetText);
    }
}
