#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

namespace view::screens {

class UserTextScreen : public QWidget {
    Q_OBJECT

public:
    explicit UserTextScreen(QWidget* parent = nullptr);

signals:
    void textLoaded(const QString& text);

private slots:
    void handleLoadButton();
    void handleLoadFromFile();

private:
    void setupUI();
    void setupConnections();

    QTextEdit* m_textEdit;
    QPushButton* m_loadButton;
    QPushButton* m_loadFromFileButton;
};

} // namespace view::screens