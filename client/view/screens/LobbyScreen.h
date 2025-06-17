// #pragma once
// #include <QWidget>
// #include <QPushButton>
// #include <QLabel>
// #include <QLineEdit>
// #include <QTextEdit>

// class UdpManager;

// namespace view
// {

//     class LobbyScreen : public QWidget
//     {
//         Q_OBJECT

//     public:
//         explicit LobbyScreen(QWidget *parent = nullptr);

//     public slots:
//         void onUserAuthenticated(const QString &username);

//     signals:
//         void startDuelRequested();
//         void backToMenuRequested();

//     private slots:
//         void onCreateLobbyClicked();
//         void onJoinLobbyClicked();
//         void onStartDuelClicked();
//         void onBackClicked();

//         // UDP события
//         void onConnectionEstablished();
//         void onConnectionLost();
//         void onMessageReceived(const QString &message);
//         void onUdpError(const QString &error);

//     private:
//         // UI элементы
//         QLabel *welcomeLabel;
//         QLabel *statusLabel;
//         QPushButton *createLobbyButton;
//         QPushButton *joinLobbyButton;
//         QLineEdit *portEdit;
//         QPushButton *startDuelButton;
//         QPushButton *backButton;
//         QTextEdit *chatArea;
//         QLineEdit *messageEdit;
//         QPushButton *sendMessageButton;

//         // Логика
//         UdpManager *udpManager;
//         QString currentUsername;
//         bool isHost = false;

//         void setupUI();
//         void updateUI();
//         void resetLobby();
//         void showLobbyControls(bool show);
//     };
// }