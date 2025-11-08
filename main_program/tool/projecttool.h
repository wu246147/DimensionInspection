#ifndef PROJECTTOOL_H
#define PROJECTTOOL_H

#include <QDir>
#include <QString>
#include <QFile>
#include <QTextStream>

class projecttool
{
public:
    projecttool();
};

void saveCsvSimple(const QString& fileName,
                   const QList<QStringList>& data);


#endif // PROJECTTOOL_H
