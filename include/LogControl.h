//
// Created by zel on 23-3-9.
//

#ifndef LOG_LOGCONTROL_H
#define LOG_LOGCONTROL_H

#include <deque>
#include "LzmaUtil.h"


#define FILE_SZIE (10 * 1024 * 1024)
#define MAX_FILE_SIZE (50 * FILE_SZIE)
#define MAX_FILE_COUNT (25)

#define LOG_FOLDER "/private/log/qglog/"
#define LOG_FILE "/private/log/qglog/last.log"

#define COMPRESS_SUFFIX ".lzma"



class LogControl {

private:
    char *mPtr;
    char *mStartPtr;
    char *mEndPtr;
    pthread_t mThreadID;
    std::deque<std::string> mLogQueue;
    std::deque<std::string> mWriteQueue;
    pthread_mutex_t mLogLock;
    pthread_cond_t mLogCondition;

    LogControl();
    void initLogFile();
    size_t getFileRealSize(const char *path);
    void compressFile(const char *path);
    bool mmapFile(const char *path);
    void getFileName(std::string &name, int &index);
    void writeLogToFile(std::string const &logStr);
    void writeLog();
    void checkLogFolder();
    static bool endsWith(std::string const &str, std::string const &suffix);
    static void *threadLoop(void *args);

public:
    static LogControl *getInstance();
    void logMsg(std::string const &str);

};


#endif //LOG_LOGCONTROL_H
