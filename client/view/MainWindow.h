#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QString>
#include <QPushButton>
#include "screens/TypingScreen.h"
#include "screens/ModeSelectionScreen.h"
#include "screens/ResultsScreen.h"
#include "StatsDialog.h"
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
        explicit MainWindow(const QString &username = QString(), QWidget *parent = nullptr);
        explicit MainWindow(const QString &username, int playerId, QWidget *parent = nullptr);
        void setPlayerId(int playerId);

    private slots:
        void onModeSelected(const QString &mode, const ModeParams &params);
        void onTypingFinished();
        void onRepeat();
        void onRestart();
        void showStats();

    private:
        int m_playerId = 0;
        QString m_username;

        std::shared_ptr<server::TextDatabase> db;
        QStackedWidget *stack;
        ModeSelectionScreen *modeSelection;
        TypingScreen *typingScreen;
        ResultsScreen *resultsScreen;
        QPushButton *statsButton;
        ModeParams currentParams;
    };
} // namespace view
