#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Создаем окно с текстом "Hello, World!"
    QLabel *label = new QLabel("Hello, World!");
    label->setWindowTitle("My First Qt App"); // Заголовок окна
    label->resize(200, 100); // Размер окна
    label->show(); // Показываем окно

    return app.exec();
}