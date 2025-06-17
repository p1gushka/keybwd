#include "LoginDialog.h"
#include <QLabel>
#include <QVBoxLayout>

namespace view
{

    LoginDialog::LoginDialog(QWidget *parent)
        : QDialog(parent)
    {
        setWindowTitle("Вход / Регистрация");
        setModal(true);
        resize(400, 200);
        setMinimumSize(400, 200);

        QLabel *loginLabel = new QLabel("Логин:", this);
        loginLineEdit = new QLineEdit(this);

        QLabel *passwordLabel = new QLabel("Пароль:", this);
        passwordLineEdit = new QLineEdit(this);
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        loginButton = new QPushButton("Войти", this);
        registerButton = new QPushButton("Зарегистрироваться", this);

        auto *layout = new QVBoxLayout(this);
        layout->addWidget(loginLabel);
        layout->addWidget(loginLineEdit);
        layout->addWidget(passwordLabel);
        layout->addWidget(passwordLineEdit);
        layout->addWidget(loginButton);
        layout->addWidget(registerButton);
        setLayout(layout);

        connect(loginButton, &QPushButton::clicked, this, [this]()
                {
        userAction = Action::Login;
        accept(); });
        connect(registerButton, &QPushButton::clicked, this, [this]()
                {
        userAction = Action::Register;
        accept(); });
    }

    QString LoginDialog::getLogin() const
    {
        return loginLineEdit->text();
    }

    QString LoginDialog::getPassword() const
    {
        return passwordLineEdit->text();
    }

    LoginDialog::Action LoginDialog::getAction() const
    {
        return userAction;
    }

} // namespace view
