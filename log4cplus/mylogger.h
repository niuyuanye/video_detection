/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-20 09:08:21
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-02-24 08:47:08
 * @FilePath: /video_detection/log4cplus/mylogger.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

// using namespace log4cplus;
// using namespace log4cplus::helpers;

// global object
extern log4cplus::Logger mylogger;

// define some macros for simplicity
#define LOG_TRACE(logEvent)			LOG4CPLUS_TRACE(mylogger, logEvent)
#define LOG_DEBUG(logEvent)			LOG4CPLUS_DEBUG(mylogger, logEvent)
// #define LOG_DEBUG_F(...)            LOG4CPLUS_DEBUG_FMT(mylogger, __VA_ARGS__)
#define LOG_INFO(logEvent)			LOG4CPLUS_INFO(mylogger, logEvent)
#define LOG_WARN(logEvent)			LOG4CPLUS_WARN(mylogger, logEvent)
#define LOG_ERROR(logEvent)			LOG4CPLUS_ERROR(mylogger, logEvent)
#define LOG_FATAL(logEvent)			LOG4CPLUS_FATAL(mylogger, logEvent)

extern void initLogger(bool isDebug);

extern void logshutDown();

extern void logclose();