// ModeSelectionScreen.cpp
#include "ModeSelectionScreen.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>

using namespace view;

ModeSelectionScreen::ModeSelectionScreen(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Выберите режим:", this));

    layout->addWidget(createModeButton("Печать на время", "time"));
    layout->addWidget(createModeButton("По количеству слов", "words"));
    
    // Пользовательский режим с полем для ввода текста
    customTextContainer = new QWidget(this);
    auto *customLayout = new QVBoxLayout(customTextContainer);
    customTextEdit = new QTextEdit(this);
    customTextEdit->setPlaceholderText("Введите текст для тренировки...");
    customTextEdit->setMaximumHeight(100);
    
    auto *customModeButton = new QPushButton("Пользовательский режим", this);
    connect(customModeButton, &QPushButton::clicked, [this]() {
        emit modeSelected("custom", customTextEdit->toPlainText());
    });
    
    customLayout->addWidget(customTextEdit);
    customLayout->addWidget(customModeButton);
    layout->addWidget(customTextContainer);
}

QPushButton* ModeSelectionScreen::createModeButton(const QString &label, const QString &mode) {
    auto *btn = new QPushButton(label, this);
    connect(btn, &QPushButton::clicked, [this, mode]() {
        emit modeSelected(mode);
    });
    return btn;
}