#include "AsyncStream.h"
#include <algorithm>
#include <string>
#include <string.h>
#include <stdint.h>
using namespace std;

namespace haha_giraffe{
//


// 高效的整型数字转字符算法, by Matthew Wilson
template<typename T>
size_t Convert(char buf[], T value)
{
    static const char digits[] = "9876543210123456789";
    static const char* zero = digits + 9;
    T i = value;
    char* p = buf;

    do
    {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0)
    {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

// 格式化整型为字符串并输出到缓冲区
template<typename T>
void AsyncStream::FormatInteger(T v)
{
    if (buffer_.GetAvailable() >= MAX_NUMERIC_SIZE)
    {
        size_t len = Convert(buffer_.GetCur(), v);
        buffer_.AppendComplete(len);
    }
}

AsyncStream& AsyncStream::operator<<(bool v)
{
    // 直接添加进缓冲区
    buffer_.Append(v ? "1" : "0", 1);
    return *this;
}

AsyncStream& AsyncStream::operator<<(short v)
{
    // 调用operator<<(int v)
    *this << static_cast<int>(v);
    return *this;
}

AsyncStream& AsyncStream::operator<<(int v)
{
    // 格式化整型为字符串并输出到缓冲区
    FormatInteger(v);
    return *this;
}

AsyncStream& AsyncStream::operator<<(long v)
{
    // 格式化整型为字符串并输出到缓冲区
    FormatInteger(v);
    return *this;
}

AsyncStream& AsyncStream::operator<<(long long v)
{
    // 格式化整型为字符串并输出到缓冲区
    FormatInteger(v);
    return *this;
}

AsyncStream& AsyncStream::operator<<(float v)
{
    // 调用operator<<(double v)
    *this << static_cast<double>(v);
    return *this;
}

AsyncStream& AsyncStream::operator<<(double v)
{
    // 直接输出到缓冲区
    if (buffer_.GetAvailable() >= MAX_NUMERIC_SIZE)
    {
        int len = snprintf(buffer_.GetCur(), MAX_NUMERIC_SIZE, "%.12g", v);
        buffer_.AppendComplete(len);
    }
    return *this;
}

AsyncStream& AsyncStream::operator<<(char v)
{
    // 直接输出到缓冲区
    buffer_.Append(&v, 1);
    return *this;
}

AsyncStream& AsyncStream::operator<<(const char* str)
{
    // 直接输出到缓冲区
    if (str)
    {
        buffer_.Append(str, strlen(str));
    }
    else
    {
        buffer_.Append("(null)", 6);
    }
    return *this;
}

AsyncStream& AsyncStream::operator<<(const std::string& v)
{
    // 直接输出到缓冲区
    buffer_.Append(v.c_str(), v.size());
    return *this;
}
}//namespace