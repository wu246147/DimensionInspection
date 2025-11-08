#include "projecttool.h"

projecttool::projecttool() {}


void saveCsvSimple(const QString& fileName,
                   const QList<QStringList>& data)
{
    QFile f(fileName);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        return;
    }

    QTextStream ts(&f);
    ts.setCodec("UTF-8");          // Qt6 用 ts.setEncoding(QStringConverter::Utf8);
    for(const QStringList &row : data)
    {
        ts << row.join(',') << Qt::endl;   // 逗号分隔
    }
}
