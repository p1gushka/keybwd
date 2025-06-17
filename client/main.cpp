#include <QApplication>
#include <QFont>
#include "view/AppController.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFont font = app.font();
    font.setPointSize(16);
    app.setFont(font);

    AppController controller(app);
    controller.run();

    return app.exec();
}
