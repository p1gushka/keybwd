#include "MainWindow.h"
#include "screens/ModeSelectionScreen.h"
#include "screens/TypingScreen.h"
#include "screens/ResultsScreen.h"
#include "model/SessionStats.h"
#include "view/components/LoginDialog.h"
#include "../../database/text_database.hpp"
#include <pqxx/pqxx>
#include <cassert>
#include <string>
#include <QMessageBox>
#include <iostream>

using namespace view;

void MainWindow::setPlayerId(int playerId)
{
    m_playerId = playerId;
}

view::MainWindow::MainWindow(const QString &username, QWidget *parent)
    : QMainWindow(parent),
      db(std::make_shared<server::TextDatabase>("localhost", "textdb", "textuser", "secure_password", "5432")),
      m_playerId(-1),
      m_username(username)
{
    resize(1000, 700);
    setWindowTitle(QString("Клавиатурный Тренажёр — %1").arg(m_username));

    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    modeSelection = new ModeSelectionScreen(this);
    typingScreen = new TypingScreen(this);
    resultsScreen = new ResultsScreen(this);

    stack->addWidget(modeSelection);
    stack->addWidget(typingScreen);
    stack->addWidget(resultsScreen);

    connect(modeSelection, &view::ModeSelectionScreen::statsRequested, this, &MainWindow::showStats);
    connect(modeSelection, &ModeSelectionScreen::modeSelected, this, &MainWindow::onModeSelected);
    connect(typingScreen, &TypingScreen::typingFinished, this, &MainWindow::onTypingFinished);
    connect(resultsScreen, &ResultsScreen::repeatClicked, this, &MainWindow::onRepeat);
    connect(resultsScreen, &ResultsScreen::restartClicked, this, &MainWindow::onRestart);

    stack->setCurrentWidget(modeSelection);
}

void MainWindow::onModeSelected(const QString &mode, const ModeParams &params)
{
    currentParams = params;
    std::string text;
    if (mode == "time")
    {
        text = db->get_random_text(1);
        typingScreen->setTimeLimit(params.timeLimit);
    }
    else if (mode == "words")
    {
        std::vector<std::string> vec_words = db->get_random_words(params.wordCount);

        // std::cout << vec_words.size() << std::endl;
        // std::cout << params.wordCount << std::endl;

        for (std::string word : vec_words)
        {
            text += word + ' ';
        }
        text.pop_back();

        typingScreen->setWordCount(params.wordCount);
    }
    else if (mode == "quotes")
    {
        std::string length_cat;
        if (params.quoteLength == "Короткие")
        {
            length_cat = "short";
        }
        else if (params.quoteLength == "Средние")
        {
            length_cat = "medium";
        }
        else if (params.quoteLength == "Длинные")
        {
            length_cat = "long";
        }
        else
        {
            assert(false);
        }

        text = db->get_random_quote(length_cat);
    }
    else if (mode == "code")
    {
        std::string lang;
        if (params.language == "C++")
        {
            lang = "cpp";
        }
        else if (params.language == "Python")
        {
            lang = "python";
        }
        else if (params.language == "JavaScript")
        {
            lang = "javascript";
        }
        else if (params.language == "Java")
        {
            lang = "java";
        }
        else
        {
            assert(false);
        }
        text = db->get_random_code(lang);
    }
    else
    {
        text = "Текст для других режимов";
    }

    typingScreen->setText(QString::fromStdString(text));
    stack->setCurrentWidget(typingScreen);
}

void MainWindow::onTypingFinished()
{
    SessionStats stats = typingScreen->getSessionStats();

    if (m_playerId > 0)
    {
        try
        {
            db->record_game(
                m_playerId,
                currentParams.mode.toStdString(),
                stats.getWPM(),
                stats.getRawWPM(),
                stats.getAccuracy(),
                stats.correctChars,
                stats.incorrectChars,
                stats.missedChars,
                stats.extraChars);
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Ошибка сохранения статистики: " << ex.what() << std::endl;
        }
    }
    else
    {
        std::cerr << "Игрок не авторизован, статистика не сохранена." << std::endl;
    }

    resultsScreen->setStats(stats);
    stack->setCurrentWidget(resultsScreen);
}

void MainWindow::onRepeat()
{
    if (currentParams.mode == "time")
    {
        typingScreen->setTimeLimit(currentParams.timeLimit);
    }
    else if (currentParams.mode == "words")
    {
        typingScreen->setWordCount(currentParams.wordCount);
    }
    else
    {
        typingScreen->setTimeLimit(0);
        typingScreen->setWordCount(0);
    }

    typingScreen->reset();
    stack->setCurrentWidget(typingScreen);
}

void MainWindow::onRestart()
{
    typingScreen->reset();
    stack->setCurrentWidget(modeSelection);
}

void MainWindow::showStats()
{
    StatsDialog dialog(this, db, m_playerId);
    dialog.exec();
}
