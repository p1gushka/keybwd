// #include "LobbyScreen.h"
// #include "../UdpManager.h"
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QFormLayout>
// #include <QGroupBox>
// #include <QMessageBox>

// using namespace view;

// LobbyScreen::LobbyScreen(QWidget *parent) : QWidget(parent) {
//     udpManager = new UdpManager(this);

//     setupUI();

//     // Подключаем сигналы кнопок
//     connect(createLobbyButton, &QPushButton::clicked, this, &LobbyScreen::onCreateLobbyClicked);
//     connect(joinLobbyButton, &QPushButton::clicked, this, &LobbyScreen::onJoinLobbyClicked);
//     connect(startDuelButton, &QPushButton::clicked, this, &LobbyScreen::onStartDuelClicked);
//     connect(backButton, &QPushButton::clicked, this, &LobbyScreen::onBackClicked);
//     connect(sendMessageButton, &QPushButton::clicked, [this]() {
//         QString message = messageEdit->text().trimmed();
//         if (!message.isEmpty()) {
//             udpManager->sendMessage(currentUsername + ": " + message);
//             chatArea->append("Вы: " + message);
//             messageEdit->clear();
//         }
//     });

//     // Подключаем UDP сигналы
//     connect(udpManager, &UdpManager::connectionEstablished, this, &LobbyScreen::onConnectionEstablished);
//     connect(udpManager, &UdpManager::connectionLost, this, &LobbyScreen::onConnectionLost);
//     connect(udpManager, &UdpManager::messageReceived, this, &LobbyScreen::onMessageReceived);
//     connect(udpManager, &UdpManager::errorOccurred, this, &LobbyScreen::onUdpError);

//     // Enter в поле сообщения отправляет сообщение
//     connect(messageEdit, &QLineEdit::returnPressed, sendMessageButton, &QPushButton::click);

//     updateUI();
// }

// void LobbyScreen::setupUI() {
//     auto *mainLayout = new QVBoxLayout(this);

//     // Приветствие
//     welcomeLabel = new QLabel("Добро пожаловать!", this);
//     welcomeLabel->setAlignment(Qt::AlignCenter);
//     welcomeLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px;");
//     mainLayout->addWidget(welcomeLabel);

//     // Статус
//     statusLabel = new QLabel("Выберите действие:", this);
//     statusLabel->setAlignment(Qt::AlignCenter);
//     statusLabel->setStyleSheet("color: blue; margin: 5px;");
//     mainLayout->addWidget(statusLabel);

//     // Группа кнопок лобби
//     auto *lobbyGroup = new QGroupBox("Лобби", this);
//     auto *lobbyLayout = new QVBoxLayout(lobbyGroup);

//     createLobbyButton = new QPushButton("Создать лобби", this);
//     createLobbyButton->setMinimumHeight(40);
//     lobbyLayout->addWidget(createLobbyButton);

//     // Форма для подключения
//     auto *joinLayout = new QHBoxLayout();
//     joinLayout->addWidget(new QLabel("Порт:", this));

//     portEdit = new QLineEdit(this);
//     portEdit->setPlaceholderText("Введите номер порта");
//     portEdit->setValidator(new QIntValidator(1024, 65535, this));
//     joinLayout->addWidget(portEdit);

//     joinLobbyButton = new QPushButton("Подключиться", this);
//     joinLayout->addWidget(joinLobbyButton);

//     lobbyLayout->addLayout(joinLayout);
//     mainLayout->addWidget(lobbyGroup);

//     // Чат (скрыт по умолчанию)
//     auto *chatGroup = new QGroupBox("Чат", this);
//     auto *chatLayout = new QVBoxLayout(chatGroup);

//     chatArea = new QTextEdit(this);
//     chatArea->setReadOnly(true);
//     chatArea->setMaximumHeight(150);
//     chatLayout->addWidget(chatArea);

//     auto *messageLayout = new QHBoxLayout();
//     messageEdit = new QLineEdit(this);
//     messageEdit->setPlaceholderText("Введите сообщение...");
//     messageLayout->addWidget(messageEdit);

//     sendMessageButton = new QPushButton("Отправить", this);
//     messageLayout->addWidget(sendMessageButton);

//     chatLayout->addLayout(messageLayout);
//     mainLayout->addWidget(chatGroup);

//     // Кнопки управления (скрыты по умолчанию)
//     auto *controlLayout = new QHBoxLayout();

//     startDuelButton = new QPushButton("Начать дуэль", this);
//     startDuelButton->setMinimumHeight(40);
//     startDuelButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");
//     controlLayout->addWidget(startDuelButton);

//     backButton = new QPushButton("Назад", this);
//     backButton->setMinimumHeight(40);
//     controlLayout->addWidget(backButton);

//     mainLayout->addLayout(controlLayout);

//     mainLayout->addStretch();

//     // Изначально скрываем элементы лобби
//     showLobbyControls(false);
// }

// void LobbyScreen::onUserAuthenticated(const QString& username) {
//     currentUsername = username;
//     welcomeLabel->setText("Добро пожаловать, " + username + "!");
//     updateUI();
// }

// void LobbyScreen::onCreateLobbyClicked() {
//     if (udpManager->createLobby()) {
//         isHost = true;
//         statusLabel->setText("Создание лобби...");
//         createLobbyButton->setEnabled(false);
//         joinLobbyButton->setEnabled(false);
//         portEdit->setEnabled(false);
//     }
// }

// void LobbyScreen::onJoinLobbyClicked() {
//     QString portText = portEdit->text().trimmed();
//     if (portText.isEmpty()) {
//         QMessageBox::warning(this, "Ошибка", "Введите номер порта");
//         return;
//     }

//     bool ok;
//     int port = portText.toInt(&ok);
//     if (!ok || port < 1024 || port > 65535) {
//         QMessageBox::warning(this, "Ошибка", "Введите корректный номер порта (1024-65535)");
//         return;
//     }

//     if (udpManager->joinLobby(port)) {
//         isHost = false;
//         statusLabel->setText("Подключение к лобби...");
//         createLobbyButton->setEnabled(false);
//         joinLobbyButton->setEnabled(false);
//         portEdit->setEnabled(false);
//     }
// }

// void LobbyScreen::onStartDuelClicked() {
//     if (isHost) {
//         udpManager->sendMessage("START_DUEL");
//         emit startDuelRequested();
//     } else {
//         QMessageBox::information(this, "Информация", "Только создатель лобби может начать дуэль");
//     }
// }

// void LobbyScreen::onBackClicked() {
//     resetLobby();
//     emit backToMenuRequested();
// }

// void LobbyScreen::onConnectionEstablished() {
//     if (isHost) {
//         statusLabel->setText("Лобби создано на порту: " + QString::number(udpManager->getPort()));
//         chatArea->append("Лобби создано. Ожидание подключения игроков...");
//     } else {
//         statusLabel->setText("Подключено к лобби");
//         chatArea->append("Подключение к лобби успешно!");
//     }

//     showLobbyControls(true);
// }

// void LobbyScreen::onConnectionLost() {
//     statusLabel->setText("Соединение потеряно");
//     chatArea->append("Соединение разорвано.");
//     resetLobby();
// }

// void LobbyScreen::onMessageReceived(const QString& message) {
//     if (message == "START_DUEL") {
//         chatArea->append("Хост начинает дуэль!");
//         emit startDuelRequested();
//     } else {
//         chatArea->append(message);
//     }
// }

// void LobbyScreen::onUdpError(const QString& error) {
//     statusLabel->setText("Ошибка: " + error);
//     statusLabel->setStyleSheet("color: red;");
//     chatArea->append("Ошибка: " + error);
//     resetLobby();
// }

// void LobbyScreen::updateUI() {
//     bool authenticated = !currentUsername.isEmpty();
//     createLobbyButton->setEnabled(authenticated);
//     joinLobbyButton->setEnabled(authenticated);

//     if (!authenticated) {
//         statusLabel->setText("Войдите в систему для доступа к лобби");
//         statusLabel->setStyleSheet("color: orange;");
//     } else {
//         statusLabel->setText("Выберите действие:");
//         statusLabel->setStyleSheet("color: blue;");
//     }
// }

// void LobbyScreen::resetLobby() {
//     udpManager->disconnect();
//     isHost = false;

//     createLobbyButton->setEnabled(!currentUsername.isEmpty());
//     joinLobbyButton->setEnabled(!currentUsername.isEmpty());
//     portEdit->setEnabled(true);
//     portEdit->clear();

//     showLobbyControls(false);
//     chatArea->clear();
//     messageEdit->clear();

//     statusLabel->setText("Выберите действие:");
//     statusLabel->setStyleSheet("color: blue;");
// }

// void LobbyScreen::showLobbyControls(bool show) {
//     chatArea->parent()->setVisible(show);
//     startDuelButton->setVisible(show);
//     backButton->setVisible(show);
// }