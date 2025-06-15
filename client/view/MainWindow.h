#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QString>
#include "screens/TypingScreen.h"
#include "screens/ModeSelectionScreen.h"
#include "screens/ResultsScreen.h"
#include "../../database/text_database.hpp"
#include "common_types.h"

namespace view
{
    class ModeSelectionScreen;
    class ResultsScreen;

    class MainWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        explicit MainWindow(QWidget *parent = nullptr);

    private slots:
        void onModeSelected(const QString &mode, const ModeParams &params);
        void onTypingFinished();
        void onRepeat();
        void onRestart();

    private:
        server::TextDatabase db;
        QStackedWidget *stack;
        ModeSelectionScreen *modeSelection;
        TypingScreen *typingScreen;
        ResultsScreen *resultsScreen;
        ModeParams currentParams;
    };
} // namespace view
