#include <QtCore/QCoreApplication>
#include "sillycompress.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SillyCompress foo("/home/alex/foo.txt");

    foo.Compress();
    return 0;//a.exec();
}
