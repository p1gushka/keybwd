#include "ButtonsPanel.h"
#include <QHBoxLayout>
#include <QPushButton>

using namespace view;

ButtonsPanel::ButtonsPanel(QWidget *parent) : QWidget(parent) {
    auto *layout = new QHBoxLayout(this);

    repeatButton = new QPushButton("Повторить", this);
    restartButton = new QPushButton("Начать сначала", this);
    errorsButton = new QPushButton("Работа над ошибками", this);

    layout->addWidget(repeatButton);
    layout->addWidget(restartButton);
    layout->addWidget(errorsButton);

    connect(repeatButton, &QPushButton::clicked, this, &ButtonsPanel::repeatClicked);
    connect(restartButton, &QPushButton::clicked, this, &ButtonsPanel::restartClicked);
    connect(errorsButton, &QPushButton::clicked, this, &ButtonsPanel::errorsClicked);
}
