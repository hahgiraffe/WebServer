/*
 * @Description:LogFormatter日志格式
 * @Author: hahagiraffe
 * @Date: 2019-08-29 17:24:34
 */
#ifndef HAFFE_LOGFORMATTER_H
#define HAFFE_LOGFORMATTER_H

#include <string>
#include <vector>
#include <time.h>
#include <memory>
#include "../base/Util.h"
namespace haffe {

    class LogEvent;
    class Logger;
    class LogFormatter {
    public:
        LogFormatter(const std::string &patten);
        std::ostream &
        format(std::ostream &ofs, std::shared_ptr<Logger> logger, Loglevel level, std::shared_ptr<LogEvent> event);

        std::string format(std::shared_ptr<Logger> logger, Loglevel level, std::shared_ptr<LogEvent> event);

        void init();

        bool isError() {
            return error_;
        }

        std::string getpatten() {
            return patten_;
        }

    public:
        class FormatItem {
        public:
            ~FormatItem() {}

            virtual void format(std::ostream &stream, std::shared_ptr<Logger> logger, Loglevel level,std::shared_ptr<LogEvent> event) = 0;
        };

    private:
        std::string patten_;
        std::vector<std::shared_ptr<FormatItem>> items_;
        bool error_;
    };
}

#endif //HAFFE_LOGFORMATTER_H
