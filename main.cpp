#include "canvas.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Canvas c;
    c.resize(c.getXLength(),c.getYLength());
    c.show();
    return a.exec();
}
