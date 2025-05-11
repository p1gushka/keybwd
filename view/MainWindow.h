#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "screens/TypingScreen.h"
#include "screens/ModeSelectionScreen.h"
#include "screens/ResultsScreen.h"
#include "screens/UserTextScreen.h"

namespace view {
class ModeSelectionScreen;
class ResultsScreen;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onModeSelected(const QString &mode);
    void onTypingFinished();
    void onRepeat();
    void onRestart();

private:
    QStackedWidget *stack;
    ModeSelectionScreen *modeSelection;
    TypingScreen *typingScreen;
    ResultsScreen *resultsScreen;
};
} // namespace view
