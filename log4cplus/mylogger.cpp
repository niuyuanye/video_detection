/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-20 09:08:21
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-02-24 08:48:34
 * @FilePath: /video_detection/log4cplus/mylogger.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <log4cplus/logger.h>
#include <log4cplus/layout.h>
#include <log4cplus/configurator.h>

// #include "logger.h"
#include "mylogger.h"

// Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("logmain"));
log4cplus::Logger mylogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("console"));

void initLogger(bool isDebug) {
    if (isDebug) {
    	log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus_debug.cfg"));
    }
    else {
        log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus_release.cfg"));
    }
}

void logshutDown() {
    log4cplus::Logger::shutdown();
}

void logclose()
{
    log4cplus::deinitialize();
}