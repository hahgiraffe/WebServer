/*
 * @Description: private继承noncopyable类
 * @Author: haha_giraffe
 * @Date: 2019-06-21 16:14:18
 */
#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class noncopyable{
public:
    noncopyable() = default;
    ~noncopyable() = default;
private:
    const noncopyable& operator = (const noncopyable&) = delete;
    noncopyable(const noncopyable&) = delete;
};

#endif
