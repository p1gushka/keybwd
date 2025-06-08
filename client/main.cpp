#include <QApplication>
#include "view/MainWindow.h"
#include "../../database/text_database.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    view::MainWindow window;
    window.show();
    return app.exec();
}
