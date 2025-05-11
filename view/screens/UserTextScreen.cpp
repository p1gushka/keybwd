#include "UserTextScreen.h"

namespace view::screens {

UserTextScreen::UserTextScreen(QWidget* parent) : QWidget(parent) {
    setupUI();
    setupConnections();
}

void UserTextScreen::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_textEdit = new QTextEdit(this);
    m_textEdit->setPlaceholderText("Введите текст для тренировки или загрузите из файла...");
    m_textEdit->setAcceptRichText(false);

    m_loadButton = new QPushButton("Начать тренировку", this);
    m_loadFromFileButton = new QPushButton("Загрузить из файла", this);

    buttonLayout->addWidget(m_loadFromFileButton);
    buttonLayout->addWidget(m_loadButton);

    mainLayout->addWidget(m_textEdit);
    mainLayout->addLayout(buttonLayout);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    setLayout(mainLayout);
}

void UserTextScreen::setupConnections() {
    connect(m_loadButton, &QPushButton::clicked, this, &UserTextScreen::handleLoadButton);
    connect(m_loadFromFileButton, &QPushButton::clicked, this, &UserTextScreen::handleLoadFromFile);
}

void UserTextScreen::handleLoadButton() {
    QString text = m_textEdit->toPlainText().trimmed();
    
    if (text.isEmpty()) {
        QMessageBox::warning(this, "Пустой текст", 
                           "Пожалуйста, введите текст для тренировки");
        return;
    }

    emit textLoaded(text);
}

void UserTextScreen::handleLoadFromFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть текстовый файл", 
                                                  "", "Текстовые файлы (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    QTextStream in(&file);
    m_textEdit->setPlainText(in.readAll());
    file.close();
}

} // namespace view::screens