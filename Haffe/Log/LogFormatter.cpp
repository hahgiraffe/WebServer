/*
 * @Description: 
 * @Author: hahagiraffe
 * @Date: 2019-08-30 17:18:21
 */
#include "LogFormatter.h"
#include "Logger.h"
#include <sstream>
#include <tuple>
#include <map>
#include <functional>
#include <iostream>
namespace haffe {

    LogFormatter::LogFormatter(const std::string &patten)
        :patten_(patten)
    {
        init();
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger,Loglevel level,std::shared_ptr<LogEvent> event) {
        std::stringstream ss;
        for(auto it:items_){
            it->format(ss,logger,level,event);
        }
        return ss.str();
    }

    std::ostream& LogFormatter::format(std::ostream& ofs , std::shared_ptr<Logger> logger,Loglevel level,std::shared_ptr<LogEvent> event){
        //std::cout<<"into LogFormatter::format";
        for(auto& i:items_){
            i->format(ofs,logger,level,event);
        }
        return ofs;
    }
    class MessageFormatItem : public LogFormatter::FormatItem {
    public:
        MessageFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            os << event->getcontent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem {
    public:
        LevelFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            os << LevelToString(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem {
    public:
        ElapseFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            os << event->getelapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem {
    public:
        NameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            os << event->getlogger()->getname();
            //os<< "hhhhh";
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadIdFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            os << event->getpthreadid();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem {
    public:
        FiberIdFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            os << event->getcoroutineid();
        }
    };

    class ThreadNameFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadNameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            os << event->getpthreadname();
        }
    };

    class DateTimeFormatItem : public LogFormatter::FormatItem {
    public:
        DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
                :m_format(format) {
            if(m_format.empty()) {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }

        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            struct tm tm;
            time_t time = event->gettime();
            localtime_r(&time, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), m_format.c_str(), &tm);
            os << buf;
        }
    private:
        std::string m_format;
    };

    class FilenameFormatItem : public LogFormatter::FormatItem {
    public:
        FilenameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            os << event->getfilename();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem {
    public:
        LineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            os << event->getline();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem {
    public:
        NewLineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            os << std::endl;
        }
    };


    class StringFormatItem : public LogFormatter::FormatItem {
    public:
        StringFormatItem(const std::string& str)
                :m_string(str) {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            os << m_string;
        }
    private:
        std::string m_string;
    };

    class TabFormatItem : public LogFormatter::FormatItem {
    public:
        TabFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger,Loglevel level, std::shared_ptr<LogEvent> event) override {
            os << "\t";
        }
    private:
        std::string m_string;
    };

    void LogFormatter::init(){
        std::vector<std::tuple<std::string, std::string, int> > vec;
        std::string nstr;
        for(size_t i = 0; i < patten_.size(); ++i) {
            if(patten_[i] != '%') {
                nstr.append(1, patten_[i]);
                continue;
            }

            if((i + 1) < patten_.size()) {
                if(patten_[i + 1] == '%') {
                    nstr.append(1, '%');
                    continue;
                }
            }

            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;

            std::string str;
            std::string fmt;
            while(n < patten_.size()) {
                if(!fmt_status && (!isalpha(patten_[n]) && patten_[n] != '{'
                                   && patten_[n] != '}')) {
                    str = patten_.substr(i + 1, n - i - 1);
                    break;
                }
                if(fmt_status == 0) {
                    if(patten_[n] == '{') {
                        str = patten_.substr(i + 1, n - i - 1);
                        //std::cout << "*" << str << std::endl;
                        fmt_status = 1; //解析格式
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                } else if(fmt_status == 1) {
                    if(patten_[n] == '}') {
                        fmt = patten_.substr(fmt_begin + 1, n - fmt_begin - 1);
                        //std::cout << "#" << fmt << std::endl;
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if(n == patten_.size()) {
                    if(str.empty()) {
                        str = patten_.substr(i + 1);
                    }
                }
            }

            if(fmt_status == 0) {
                if(!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            } else if(fmt_status == 1) {
                std::cout << "pattern parse error: " << patten_ << " - " << patten_.substr(i) << std::endl;
                error_ = true;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }

        if(!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }
        static std::map<std::string, std::function<std::shared_ptr<FormatItem>(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return std::shared_ptr<FormatItem>(new C(fmt));}}

                XX(m, MessageFormatItem),           //m:消息
                XX(p, LevelFormatItem),             //p:日志级别
                XX(r, ElapseFormatItem),            //r:累计毫秒数
                XX(c, NameFormatItem),              //c:日志名称
                XX(t, ThreadIdFormatItem),          //t:线程id
                XX(n, NewLineFormatItem),           //n:换行
                XX(d, DateTimeFormatItem),          //d:时间
                XX(f, FilenameFormatItem),          //f:文件名
                XX(l, LineFormatItem),              //l:行号
                XX(T, TabFormatItem),               //T:Tab
                XX(F, FiberIdFormatItem),           //F:协程id
                XX(N, ThreadNameFormatItem)         //N:线程名称

#undef XX
        };

        for(auto& i : vec) {
            if(std::get<2>(i) == 0) {
                items_.push_back(std::shared_ptr<FormatItem>(new StringFormatItem(std::get<0>(i))));
            } else {
                auto it = s_format_items.find(std::get<0>(i));
                if(it == s_format_items.end()) {
                    items_.push_back(std::shared_ptr<FormatItem> (new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    error_ = true;
                } else {
                    items_.push_back(it->second(std::get<1>(i)));
                }
            }

            //std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
        }
        //std::cout << m_items.size() << std::endl;
    }

}