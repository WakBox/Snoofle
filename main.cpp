#include <QCoreApplication>
#include "snoofle.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Snoofle snoofle;

    if (!snoofle.init())
        return 0;

    return a.exec();
}
