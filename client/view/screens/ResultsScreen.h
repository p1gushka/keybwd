#pragma once
#include <QWidget>
#include "model/SessionStats.h"


class QLabel;
namespace view {
class ButtonsPanel;

class ResultsScreen : public QWidget {
    Q_OBJECT
public:
    explicit ResultsScreen(QWidget *parent = nullptr);
    void setStats(const SessionStats &stats);

signals:
    void repeatClicked();
    void restartClicked();
    void errorsClicked();

private:
    QLabel *statsLabel;
    ButtonsPanel *buttons;
};

} // namespace view
