#include <QApplication>
#include "C_Main.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Main window of the application
    C_Main appMainWin;

    return app.exec();
}
