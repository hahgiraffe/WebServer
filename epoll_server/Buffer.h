/*
 * @Description: 应用层缓冲,参考muduo实现
 * @Author: hahagiraffe
 * @Date: 2019-07-19 14:45:11
 */
#ifndef BUFFER_H
#define BUFFER_H
#include <string>
#include <vector>
class Buffer{
public:
    const size_t kCheapPrepend=8;
    const size_t kInitialSize=1024;
    Buffer():buffer_(kCheapPrepend+kInitialSize),
            readindex_(kCheapPrepend),
            writeindex_(kCheapPrepend)
    {

    }
    //可读字节数
    size_t readableBytes() const
    { return writeindex_ - readindex_; }

    //可写字节数
    size_t writableBytes() const
    { return buffer_.size() - writeindex_; }

    //前置字节数
    size_t prependableBytes() const
    { return readindex_; }

private:

    std::vector<char> buffer_;//缓冲区
    size_t readindex_;//读指针
    size_t writeindex_;//写指针
};

#endif