#pragma once
#include <QWidget>

class QPushButton;

namespace view {

class ModeSelectionScreen : public QWidget {
    Q_OBJECT
public:
    explicit ModeSelectionScreen(QWidget *parent = nullptr);

signals:
    void modeSelected(const QString &mode);

private:
    QPushButton *createModeButton(const QString &label, const QString &mode);
};

} // namespace view
