#ifndef TOOL_H
#define TOOL_H

#include <QString>
// #include <q
#include <QDir>
#include "opencv2/opencv.hpp"
#include <QImage>
#include <QPixmap>

#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace util
{
    class ArgBase
    {
    public:
        ArgBase() {}
        virtual ~ArgBase() {}
        virtual void Format(std::ostringstream &ss, const std::string& fmt) = 0;
    };

    template <class T>
    class Arg : public ArgBase
    {
    public:
        Arg(T arg) : m_arg(arg) {}
        virtual ~Arg() {}
        virtual void Format(std::ostringstream &ss, const std::string& fmt)
        {
            ss << m_arg;
        }
    private:
        T m_arg;
    };

    class ArgArray : public std::vector < ArgBase* >
    {
    public:
        ArgArray() {}
        ~ArgArray()
        {
            std::for_each(begin(), end(), [](ArgBase * p)
            {
                delete p;
            });
        }
    };

    static void FormatItem(std::ostringstream& ss, const std::string& item, const ArgArray& args)
    {
        int index = 0;
        int alignment = 0;
        std::string fmt;

        char* endptr = nullptr;
        index = strtol(&item[0], &endptr, 10);
        if(index < 0 || index >= args.size())
        {
            return;
        }

        if(*endptr == ',')
        {
            alignment = strtol(endptr + 1, &endptr, 10);
            if(alignment > 0)
            {
                ss << std::right << std::setw(alignment);
            }
            else if(alignment < 0)
            {
                ss << std::left << std::setw(-alignment);
            }
        }

        if(*endptr == ':')
        {
            fmt = endptr + 1;
        }

        args[index]->Format(ss, fmt);

        return;
    }

    template <class T>
    static void Transfer(ArgArray& argArray, T t)
    {
        argArray.push_back(new Arg<T>(t));
    }

    template <class T, typename... Args>
    static void Transfer(ArgArray& argArray, T t, Args&&... args)
    {
        Transfer(argArray, t);
        Transfer(argArray, args...);
    }

    template <typename... Args>
    std::string Format(const std::string& format, Args&&... args)
    {
        if(sizeof...(args) == 0)
        {
            return format;
        }

        ArgArray argArray;
        Transfer(argArray, args...);
        size_t start = 0;
        size_t pos = 0;
        std::ostringstream ss;
        while(true)
        {
            pos = format.find('{', start);
            if(pos == std::string::npos)
            {
                ss << format.substr(start);
                break;
            }

            ss << format.substr(start, pos - start);
            if(format[pos + 1] == '{')
            {
                ss << '{';
                start = pos + 2;
                continue;
            }

            start = pos + 1;
            pos = format.find('}', start);
            if(pos == std::string::npos)
            {
                ss << format.substr(start - 1);
                break;
            }

            FormatItem(ss, format.substr(start, pos - start), argArray);
            start = pos + 1;
        }

        return ss.str();
    }
}


template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6);

///
/// \brief MatToQImage mat图片转QImage图片 有个bug，如果图片太大，有可能会转换失败。
/// \param cvImage 输入mat图片
/// \return 返回QImage图片
///
QImage MatToQImage(const cv::Mat &cvImage);

///
/// \brief MatToPixmap mat图片转Pixmap图片
/// \param cvImage 输入mat图片
/// \return 返回Pixmap图片
///
QPixmap MatToPixmap(const cv::Mat &cvImage);


///
/// \brief createMultipleFolders 创建文件夹，生成多层子文件，注意的是路径必须要用/符号分开
/// \param path 文件夹路线
/// \return
///
QString createMultipleFolders(const QString path);


void replaceAll(std::string& str, const std::string& from, const std::string& to);


void byte2string(uint8_t byteList[], int startID, int endID, std::string &name);




class tool
{
public:
    tool();
};

#endif // TOOL_H
