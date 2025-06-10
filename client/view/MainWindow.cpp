#include "MainWindow.h"
#include "screens/ModeSelectionScreen.h"
#include "screens/TypingScreen.h"
#include "screens/ResultsScreen.h"
#include "model/SessionStats.h"
#include "../../database/text_database.hpp"
#include <pqxx/pqxx>

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

void MainWindow::onModeSelected(const QString &mode)
{
    QString text;
    if (mode == "time")
    {
        text = QString::fromStdString(db.get_random_text());
        typingScreen->setTimeLimit(modeSelection->getTimeLimit());
    }
    else if (mode == "words")
    {
        text = QString::fromStdString(db.get_random_text());
        typingScreen->setWordCount(modeSelection->getWordCount());
    }
    else if (mode == "custom")
    {
        text = modeSelection->getCustomText();
    }
    else
    {
        text = "Текст для других режимов";
    }

    typingScreen->setText(text);
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
