#include <QtCore/QCoreApplication>
#include "sillycompress.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SillyCompress foo("/home/alex/Downloads/fotia.csv");

    foo.Compress();
    return 0;//a.exec();
}
