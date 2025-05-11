#include "MainWindow.h"
#include "screens/ModeSelectionScreen.h"
#include "screens/TypingScreen.h"
#include "screens/ResultsScreen.h"
#include "model/SessionStats.h"
#include "MainWindow.h"

using namespace view;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    resize(800, 600);
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

void MainWindow::onModeSelected(const QString &mode) {
    QString text = (mode == "time") ? "Текст для режима на время."
                  : (mode == "words") ? "Текст по количеству слов."
                  : "Пользовательский текст.";
    typingScreen->setText(text);
    stack->setCurrentWidget(typingScreen);
}

void MainWindow::onTypingFinished() {
    SessionStats stats = typingScreen->getSessionStats();
    resultsScreen->setStats(stats);
    stack->setCurrentWidget(resultsScreen);
}

void MainWindow::onRepeat() {
    stack->setCurrentWidget(typingScreen);
}

void MainWindow::onRestart() {
    stack->setCurrentWidget(modeSelection);
}
