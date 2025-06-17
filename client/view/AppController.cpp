#include "AppController.h"
#include "view/components/LoginDialog.h"
#include "view/MainWindow.h"

#include <QMessageBox>
#include <QInputDialog>

AppController::AppController(QApplication &app)
    : QObject(&app), m_app(app), m_db("localhost", "textdb", "textuser", "secure_password", "5432")
{
}

AppController::~AppController()
{
    delete m_loginDlg;
    delete m_mainWnd;
}

void AppController::run()
{
    showLogin();
}

void AppController::showLogin()
{
    m_loginDlg = new view::LoginDialog;
    while (true)
    {
        if (m_loginDlg->exec() != QDialog::Accepted)
        {
            m_app.quit();
            return;
        }

        const QString login = m_loginDlg->getLogin();
        const QString password = m_loginDlg->getPassword();
        auto action = m_loginDlg->getAction();

        if (login.isEmpty() || password.isEmpty())
        {
            QMessageBox::warning(nullptr, "Ошибка", "Заполните все поля");
            continue;
        }

        if (action == view::LoginDialog::Action::Login)
        {
            auto optId = m_db.authenticate_player(login.toStdString(), password.toStdString());
            if (!optId)
            {
                QMessageBox::warning(nullptr, "Ошибка входа", "Неверный логин или пароль");
                continue;
            }
            std::string storedUsername = m_db.get_username_by_login(login.toStdString());
            int playerId = *optId;
            showMainWindow(QString::fromStdString(storedUsername), *optId);
            return;
        }
        else
        { // Register
            bool ok = false;
            QString username = QInputDialog::getText(
                nullptr, "Регистрация",
                "Введите имя пользователя:",
                QLineEdit::Normal, QString(), &ok);
            if (!ok || username.isEmpty())
            {
                QMessageBox::warning(nullptr, "Ошибка регистрации", "Имя пользователя не может быть пустым");
                continue;
            }
            bool registered = m_db.register_player(
                login.toStdString(),
                username.toStdString(),
                password.toStdString());
            if (!registered)
            {
                QMessageBox::warning(nullptr, "Ошибка регистрации", "Логин уже занят");
                continue;
            }
            QMessageBox::information(nullptr, "Успех", "Регистрация завершена. Войдите.");
            continue;
        }
    }
}

void AppController::showMainWindow(const QString &username, int playerId)
{
    m_mainWnd = new view::MainWindow(username);
    m_mainWnd->setPlayerId(playerId);
    m_mainWnd->show();
}