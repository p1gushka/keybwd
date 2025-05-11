#include "ResultsScreen.h"
#include "view/components/ButtonsPanel.h"
#include <QVBoxLayout>
#include <QLabel>
#include "model/SessionStats.h"

using namespace view;

ResultsScreen::ResultsScreen(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    statsLabel = new QLabel(this);
    layout->addWidget(statsLabel);

    buttons = new ButtonsPanel(this);
    layout->addWidget(buttons);

    connect(buttons, &ButtonsPanel::repeatClicked, this, &ResultsScreen::repeatClicked);
    connect(buttons, &ButtonsPanel::restartClicked, this, &ResultsScreen::restartClicked);
    connect(buttons, &ButtonsPanel::errorsClicked, this, &ResultsScreen::errorsClicked);
}

void ResultsScreen::setStats(const SessionStats &stats) {
    QString statsText;
    statsText += QString("Скорость (WPM): %1\n").arg(stats.getWPM(), 0, 'f', 1);
    statsText += QString("Сырые WPM (RAW): %1\n").arg(stats.getRawWPM(), 0, 'f', 1);
    statsText += QString("Точность: %1%\n").arg(stats.getAccuracy(), 0, 'f', 1);
    statsText += QString("Правильных символов: %1\n").arg(stats.correctChars);
    statsText += QString("Ошибочных символов: %1\n").arg(stats.incorrectChars);
    statsText += QString("Пропущенных символов: %1\n").arg(stats.missedChars);
    statsText += QString("Лишних символов: %1\n").arg(stats.extraChars);

    // например, отображение в QLabel
    statsLabel->setText(statsText);
}

