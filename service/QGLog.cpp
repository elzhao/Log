//
// Created by zel on 23-3-10.
//

#include <cstdarg>
#include <sys/time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "QGLog.h"

LogControl *pInstance = LogControl::getInstance();

void addTimeAndPid(std::string &content) {
    char buffer[128];
    long tid = syscall(SYS_gettid);

    struct tm tm{};
    time_t ts = time(nullptr);
    localtime_r(&ts,&tm);

    struct timeval tv{};
    gettimeofday(&tv,nullptr);

    sprintf(buffer, "[%d-%02d-%02d %02d:%02d:%02d.%06ld] %5d %5ld ",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec, getpid(), tid);
    content.append(buffer);
}

void log(const char *format, ...) {
    std::string content;
    char buffer[1024];
    addTimeAndPid(content);

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    content.append(buffer);
    content.append("\n");
    pInstance->logMsg(content);
}

