//
// Created by zel on 23-3-10.
//

#ifndef LOG_QGLOG_H
#define LOG_QGLOG_H

#include "LogControl.h"

void addTimeAndPid(std::string &content);

void log(const char *format, ...);

#endif //LOG_QGLOG_H
