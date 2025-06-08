#pragma once
#include <QWidget>

class QPushButton;
class QTextEdit;

namespace view {

class ModeSelectionScreen : public QWidget {
    Q_OBJECT
public:
    explicit ModeSelectionScreen(QWidget *parent = nullptr);

signals:
    void modeSelected(const QString &mode, const QString &customText = "");

private:
    QPushButton *createModeButton(const QString &label, const QString &mode);
    QTextEdit *customTextEdit;
    QWidget *customTextContainer;
};

} // namespace view