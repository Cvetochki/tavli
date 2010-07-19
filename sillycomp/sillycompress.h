#ifndef SILLYCOMPRESS_H
#define SILLYCOMPRESS_H

#include <QString>

class SillyCompress
{
    QString m_filename;
    int m_originalSize;
    int m_finalSize;
public:
    SillyCompress(char *filename);
    int Compress(void);
};

#endif // SILLYCOMPRESS_H
