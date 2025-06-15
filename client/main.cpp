#include <QFont>
#include <QApplication>
#include "view/MainWindow.h"
#include "../../database/text_database.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Temporary
    QFont font = app.font();
    font.setPointSize(16);
    app.setFont(font);

    view::MainWindow window;
    window.show();
    return app.exec();
}
