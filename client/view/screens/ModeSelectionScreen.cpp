#include "ModeSelectionScreen.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

using namespace view;

ModeSelectionScreen::ModeSelectionScreen(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Выберите режим:", this));

    layout->addWidget(createModeButton("Печать на время", "time"));
    layout->addWidget(createModeButton("По количеству слов", "words"));
    layout->addWidget(createModeButton("Пользовательский режим", "custom"));
}

QPushButton* ModeSelectionScreen::createModeButton(const QString &label, const QString &mode) {
    auto *btn = new QPushButton(label, this);
    connect(btn, &QPushButton::clicked, [this, mode]() {
        emit modeSelected(mode);
    });
    return btn;
}
