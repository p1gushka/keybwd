#include "MainWindow.h"
#include "screens/ModeSelectionScreen.h"
#include "screens/TypingScreen.h"
#include "screens/ResultsScreen.h"
#include "model/SessionStats.h"
#include "../../database/text_database.hpp"
#include <pqxx/pqxx>
#include <cassert>
#include <string>
#include <iostream>

using namespace view;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), db("localhost", "textdb", "textuser", "secure_password", "5432")
{
    resize(1000, 700);

    setWindowTitle("Клавиатурный Тренажёр");

    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    modeSelection = new ModeSelectionScreen(this);
    typingScreen = new TypingScreen(this);
    resultsScreen = new ResultsScreen(this);

    stack->addWidget(modeSelection);
    stack->addWidget(typingScreen);
    stack->addWidget(resultsScreen);

    connect(modeSelection, &ModeSelectionScreen::modeSelected, this, &MainWindow::onModeSelected);
    connect(typingScreen, &TypingScreen::typingFinished, this, &MainWindow::onTypingFinished);
    connect(resultsScreen, &ResultsScreen::repeatClicked, this, &MainWindow::onRepeat);
    connect(resultsScreen, &ResultsScreen::restartClicked, this, &MainWindow::onRestart);

    stack->setCurrentWidget(modeSelection);
}

void MainWindow::onModeSelected(const QString &mode, const ModeParams &params) {
    // НУЖНЫ ПАРАМЕТРЫ
    currentParams = params;
    std::string text;
    if (mode == "time")
    {
        text = db.get_random_text(1);
        typingScreen->setTimeLimit(params.timeLimit);
    }
    else if (mode == "words")
    {   
        std::vector<std::string> vec_words = db.get_random_words(params.wordCount);
        
        std::cout << vec_words.size() << std::endl;
        std::cout << params.wordCount << std::endl;

        for (std::string word : vec_words) {
            text += word + ' ';
        }
        text.pop_back();

        typingScreen->setWordCount(params.wordCount);
    }
    else if (mode == "quotes")
    {
        std::string length_cat;
        if (params.quoteLength == "Короткие") {
            length_cat = "short";
        } else if (params.quoteLength == "Средние") {
            length_cat = "medium";
        } else if (params.quoteLength == "Длинные") {
            length_cat = "long";
        } else {
            assert(false);
        }

        text = db.get_random_quote(length_cat);
    }
    else if (mode == "code") {
        text = db.get_random_code((params.language).toStdString());
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
    resultsScreen->setStats(stats);
    stack->setCurrentWidget(resultsScreen);
}

void MainWindow::onRepeat()
{
    typingScreen->reset();
    stack->setCurrentWidget(typingScreen);
}

void MainWindow::onRestart()
{
    typingScreen->reset();
    stack->setCurrentWidget(modeSelection);
}
