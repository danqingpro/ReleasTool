#include "releasetool.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ReleaseTool w;
    w.show();

    return a.exec();
}
