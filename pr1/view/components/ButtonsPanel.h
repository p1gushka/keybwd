#pragma once

#include <QWidget>

class QPushButton;

namespace view {

class ButtonsPanel : public QWidget {
    Q_OBJECT
public:
    explicit ButtonsPanel(QWidget *parent = nullptr);

signals:
    void repeatClicked();
    void restartClicked();
    void errorsClicked();

private:
    QPushButton *repeatButton;
    QPushButton *restartButton;
    QPushButton *errorsButton;
};

} // namespace view
