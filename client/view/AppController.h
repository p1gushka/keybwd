#pragma once

#include <QObject>
#include <QString>
#include <QApplication>
#include "../../database/text_database.hpp"

namespace view
{
    class LoginDialog;
    class MainWindow;
}

class AppController : public QObject
{
    Q_OBJECT

public:
    explicit AppController(QApplication &app);
    ~AppController() override;

    void run();

private:
    QApplication &m_app;
    server::TextDatabase m_db;
    view::LoginDialog *m_loginDlg = nullptr;
    view::MainWindow *m_mainWnd = nullptr;

    void showLogin();
    void showMainWindow(const QString &username, int playerId);
};
