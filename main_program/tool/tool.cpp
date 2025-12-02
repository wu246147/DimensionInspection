#include "tool.h"
tool::tool() {}


template <typename T>
std::string to_string_with_precision(const T a_value, const int n)
{
    int nn = n ;
    std::ostringstream out;
    out.setf(std::ios::fixed);
    out << std::setprecision(nn) << a_value;
    return out.str();
}


QString createMultipleFolders(const QString path)
{
    QString pathReplace = path;
    pathReplace.replace("\\", "/");
    QDir dir(pathReplace);
    if(pathReplace == "" || dir.exists(pathReplace))
    {
        return pathReplace;
    }
    QString parentDir = createMultipleFolders(pathReplace.mid(0, pathReplace.lastIndexOf('/')));
    QString dirName = pathReplace.mid(pathReplace.lastIndexOf('/') + 1);
    QDir parentPath(parentDir);
    if(!dirName.isEmpty())
    {
        parentPath.mkpath(dirName);
    }
    return parentDir + "/" + dirName;
}

QImage MatToQImage(const cv::Mat &cvImage)
{
    std::vector<uchar> imgBuf;
    imencode(".bmp", cvImage, imgBuf);
    QByteArray baImg((char *)imgBuf.data(), static_cast<int>(imgBuf.size()));
    QImage image;
    image.loadFromData(baImg, "bmp");
    return image;
}

QPixmap MatToPixmap(const cv::Mat &cvImage)
{
    return QPixmap::fromImage(MatToQImage(cvImage));
}

void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

void byte2string(uint8_t byteList[], int startID, int endID, std::string &name)
{
    char*p = new char[endID - startID];

    memcpy(p, &byteList[startID], endID - startID);
    p[endID - startID] = 0;

    name = p;
}



