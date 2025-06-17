#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

namespace view
{

    class LoginDialog : public QDialog
    {
        Q_OBJECT

    public:
        enum class Action
        {
            None,
            Login,
            Register
        };

        explicit LoginDialog(QWidget *parent = nullptr);

        QString getLogin() const;
        QString getPassword() const;
        Action getAction() const;

    private:
        QLineEdit *loginLineEdit;
        QLineEdit *passwordLineEdit;
        QPushButton *loginButton;
        QPushButton *registerButton;

        Action userAction = Action::None;
    };

} // namespace view
