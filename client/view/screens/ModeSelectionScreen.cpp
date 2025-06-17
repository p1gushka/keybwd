#include "ModeSelectionScreen.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QComboBox>
#include <QStackedWidget>
#include <QFrame>

using namespace view;

ModeSelectionScreen::ModeSelectionScreen(QWidget *parent) : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("Выберите режим:", this));

    // Контейнер для кнопок режимов
    auto *modesLayout = new QHBoxLayout();
    modesLayout->addWidget(createModeButton("На время", "time"));
    modesLayout->addWidget(createModeButton("По словам", "words"));
    modesLayout->addWidget(createModeButton("Цитаты", "quotes"));
    modesLayout->addWidget(createModeButton("Пользовательский", "custom"));
    modesLayout->addWidget(createModeButton("Код", "code"));
    mainLayout->addLayout(modesLayout);

    // Добавляем кнопку "Моя статистика" под кнопками режимов
    auto *statsBtn = new QPushButton("Моя статистика", this);
    connect(statsBtn, &QPushButton::clicked, this, [this]()
            { emit statsRequested(); });
    mainLayout->addWidget(statsBtn);

    // Область для опций режима
    auto *optionsFrame = new QFrame(this);
    optionsFrame->setFrameShape(QFrame::StyledPanel);
    auto *optionsLayout = new QVBoxLayout(optionsFrame);

    optionsLabel = new QLabel("Настройки режима:", this);
    optionsLayout->addWidget(optionsLabel);

    currentOptionsPanel = new QWidget(this);
    optionsLayout->addWidget(currentOptionsPanel);
    mainLayout->addWidget(optionsFrame);

    // Кнопка начала
    startButton = new QPushButton("Начать", this);
    startButton->setEnabled(false);
    connect(startButton, &QPushButton::clicked, [this]()
            {
        ModeParams params;
        params.mode = currentMode;
        
        if (currentMode == "time") {
            params.timeLimit = timeComboBox->currentText().toInt();
        } else if (currentMode == "words") {
            params.wordCount = wordsComboBox->currentText().toInt();
        } else if (currentMode == "quotes") {
            params.quoteLength = quoteLengthComboBox->currentText();
        } else if (currentMode == "custom") {
            params.customText = customTextEdit->toPlainText();
        } else if (currentMode == "code") {
            params.language = languageComboBox->currentText();
        }
        
        emit modeSelected(currentMode, params); });
    mainLayout->addWidget(startButton);
}

QPushButton *ModeSelectionScreen::createModeButton(const QString &label, const QString &mode)
{

    auto *btn = new QPushButton(label, this);
    connect(btn, &QPushButton::clicked, [this, mode]()
            {
        currentMode = mode;
        startButton->setEnabled(true); // Разблокируем кнопку при выборе режима        
        
        // Удаляем предыдущие опции
        delete currentOptionsPanel;
        currentOptionsPanel = new QWidget(this);
        
        auto *parentWidget = qobject_cast<QWidget*>(currentOptionsPanel->parent());
        if (parentWidget) {
            auto *optionsLayout = qobject_cast<QVBoxLayout*>(parentWidget->layout());
            if (optionsLayout) {
                optionsLayout->insertWidget(1, currentOptionsPanel);
            }
        }
                
        // Создаем новые опции для выбранного режима
        auto *panelLayout = new QVBoxLayout(currentOptionsPanel);
        
        if (mode == "time") {
            setupTimeModeOptions(panelLayout);
        } else if (mode == "words") {
            setupWordCountModeOptions(panelLayout);
        } else if (mode == "quotes") {
            setupQuoteModeOptions(panelLayout);
        } else if (mode == "custom") {
            setupCustomTextMode(panelLayout);
        } else if (mode == "code") {
            setupCodeModeOptions(panelLayout);
        } });
    return btn;
}

void ModeSelectionScreen::setupTimeModeOptions(QVBoxLayout *layout)
{
    optionsLabel->setText("Настройки: Режим на время");

    auto *timeLayout = new QHBoxLayout();
    timeLayout->addWidget(new QLabel("Время (сек):", this));

    timeComboBox = new QComboBox(this);
    timeComboBox->addItems({"10", "15", "30", "60"});
    timeLayout->addWidget(timeComboBox);

    layout->addLayout(timeLayout);
}

void ModeSelectionScreen::setupWordCountModeOptions(QVBoxLayout *layout)
{
    optionsLabel->setText("Настройки: Режим по количеству слов");

    auto *wordsLayout = new QHBoxLayout();
    wordsLayout->addWidget(new QLabel("Количество слов:", this));

    wordsComboBox = new QComboBox(this);
    wordsComboBox->addItems({"10", "25", "50", "100"});
    wordsLayout->addWidget(wordsComboBox);

    layout->addLayout(wordsLayout);
}

void ModeSelectionScreen::setupQuoteModeOptions(QVBoxLayout *layout)
{
    optionsLabel->setText("Настройки: Режим цитат");

    auto *quoteLayout = new QHBoxLayout();
    quoteLayout->addWidget(new QLabel("Длина цитаты:", this));

    quoteLengthComboBox = new QComboBox(this);
    quoteLengthComboBox->addItems({"Короткие", "Средние", "Длинные"});
    quoteLayout->addWidget(quoteLengthComboBox);

    layout->addLayout(quoteLayout);
}

void ModeSelectionScreen::setupCustomTextMode(QVBoxLayout *layout)
{
    optionsLabel->setText("Настройки: Пользовательский текст");

    auto *label = new QLabel("Введите текст для тренировки:", this);
    layout->addWidget(label);

    customTextEdit = new QTextEdit(this);
    customTextEdit->setPlaceholderText("Введите текст для тренировки...");
    layout->addWidget(customTextEdit);
}

void ModeSelectionScreen::setupCodeModeOptions(QVBoxLayout *layout)
{
    optionsLabel->setText("Настройки: Написание кода");

    auto *langLayout = new QHBoxLayout();
    langLayout->addWidget(new QLabel("Язык программирования:", this));

    languageComboBox = new QComboBox(this);
    languageComboBox->addItems({"C++", "Python", "JavaScript", "Java"});
    langLayout->addWidget(languageComboBox);

    layout->addLayout(langLayout);
}

// Геттеры для получения параметров
QString ModeSelectionScreen::getSelectedMode() const { return currentMode; }
int ModeSelectionScreen::getTimeLimit() const { return timeComboBox->currentText().toInt(); }
int ModeSelectionScreen::getWordCount() const { return wordsComboBox->currentText().toInt(); }
QString ModeSelectionScreen::getCustomText() const { return customTextEdit->toPlainText(); }
QString ModeSelectionScreen::getQuoteLength() const { return quoteLengthComboBox->currentText(); }
QString ModeSelectionScreen::getLanguage() const { return languageComboBox->currentText(); }
