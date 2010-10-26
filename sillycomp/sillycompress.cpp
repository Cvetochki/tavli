#include "sillycompress.h"

#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QDebug>

#include <iostream>

class element {
public:
    int value;
    int appearences;
};

bool caseInsensitiveLessThan(const element *s1, const element *s2);


bool caseInsensitiveLessThan(const element *s1, const element *s2)
{
    return s1->appearences > s2->appearences;
}


SillyCompress::SillyCompress(char *filename)
{
    m_filename=filename;
}

int SillyCompress::Compress(void)
{
    unsigned char *buffer;
    QList<element *> weights;

    QFileInfo *finfo=new QFileInfo(m_filename);
    m_originalSize=finfo->size();
    qDebug() << m_filename <<" : " << m_originalSize;

    if (m_originalSize>0) {
        buffer=new unsigned char[m_originalSize];
    }

    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly))
         return 20;

    QDataStream in(&file);

    in.readRawData((char *)buffer,m_originalSize);

    for(int i=0; i<256; ++i) {
        weights.append(new element());
        weights[i]->value=i;
        weights[i]->appearences=0;
    }
    for(int i=0; i<m_originalSize; ++i)
        ++weights[(unsigned char)buffer[i]]->appearences;

    qSort(weights.begin(),weights.end(),caseInsensitiveLessThan);
    int nonempty=0;
    for(int i=0; i<256; ++i)
        if (weights[i]->appearences) {
            qDebug() << (char)weights[i]->value << "(" << weights[i]->value<< ") : " << weights[i]->appearences;
            ++nonempty;
        }
    file.close();
    qDebug() << nonempty << " values appeared...";

    QFile file2(m_filename+".sz");
    if (!file2.open(QIODevice::WriteOnly))
         return 20;
    QDataStream out(&file2);

    int b,j,sum=nonempty;
    out << (quint8)(nonempty-1);
    for(int i=0; i<nonempty; ++i)
        out << (quint8) (weights[i]->value);

    for(int i=0; i<m_originalSize; ++i) {
        for(j=0; j<nonempty; ++j)
            if (buffer[i]==weights[j]->value) {
                break;
            }
        if (j<3)
            b=1;
        else if (j<5)
            b=2;
        else if (j<9)
            b=3;
        else if (j<17)
            b=4;
        else if (j<33)
            b=5;
        else if (j<65)
            b=6;
        else if (j<129)
            b=7;
        else
            b=8;

        sum+=(3+b);
    }
    file2.close();
    qDebug() << sum/8 << "/" << m_originalSize << " "<< 100* ( (m_originalSize-sum/8.0)/m_originalSize) << "%";

    std::cin >> b;
    return 0;
}

