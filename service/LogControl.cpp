//
// Created by zel on 23-3-9.
//
#include <fstream>
#include <sys/stat.h>
#include <cstring>
#include <unistd.h>
#include <dirent.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <vector>
#include <algorithm>
#include "LogControl.h"

using namespace std;

LogControl::LogControl() {
    pthread_mutex_init(&mLogLock, nullptr);
    pthread_cond_init(&mLogCondition, nullptr);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int ret = pthread_create(&mThreadID, &attr, threadLoop, this);
    if (ret) {
        mThreadID = 0;
        cout << "Start Write Log thread failed !\n";
        return;
    }
}

LogControl *LogControl::getInstance() {
    static LogControl sInstance;
    return &sInstance;
}

void LogControl::initLogFile() {
    // 创建log文件夹
    if (access(LOG_FOLDER, 0) != 0) {
        int result = mkdir(LOG_FOLDER, S_IRWXU | S_IRWXG | S_IRWXO);
        //ALOGD("%s, mkdir %s, result: %d, errno: %s", __FUNCTION__, LOG_FOLDER, result, strerror(errno));
        std::cout << "mkdir result: " << result << ", errno: " << strerror(errno);
    }

//    // 如果log文件写满了就压缩
//    size_t logRealSize = getFileRealSize(LOG_FOLDER);
//    if (logRealSize > 0 && FILE_SZIE - logRealSize < DIFF_SIZE) {
//        compressFile(LOG_FILE);
//    }

    mmapFile(LOG_FILE);
}

void LogControl::compressFile(const char *path) {
    // 压缩log文件并删除原文件
    string newName;
    int index;
    newName.append(LOG_FOLDER);
    getFileName(newName, index);
    int ret = Compress(path, newName.c_str());
    cout << "Compress ret: " << ret << "\n";
    ret = remove(path);
    cout << "remove ret: " << ret << "\n";
    if (index > MAX_FILE_COUNT) {
        checkLogFolder();
    }
}

void LogControl::checkLogFolder() {
    // 获取文件压缩列表
    DIR * dir;
    struct dirent * ptr;
    vector<string> files;

    dir = opendir(LOG_FOLDER);
    if (dir == nullptr) {
        return;
    }

    while ((ptr = readdir(dir)) != nullptr) {
        if (endsWith(ptr->d_name, COMPRESS_SUFFIX)) {
            files.emplace_back(ptr->d_name);
        }
    }
    closedir(dir);

    // 按照序号将列表重新排序
    sort(files.begin(), files.end(), [](const string &l, const string &r){
        string tailL = l.substr(l.find("log"));
        string tailR = r.substr(r.find("log"));
        return l < r;
    });

    // 删除 1/3 文件
    const int count = files.size() / 3;
    string path;
    for (int i = 0; i < count; i++) {
        path.clear();
        path.append(LOG_FOLDER);
        path.append(files[i]);
        remove(path.c_str());
    }

    files.erase(files.begin(), files.begin() + count);

    // 更新剩下的文件序号
    string oldPath;
    string newPath;
    string newName;
    int index = 1;
    char chIndex[16];
    for (const string& name:files) {
        newName = name;
        oldPath.clear();
        oldPath.append(LOG_FOLDER);
        oldPath.append(name);

        sprintf(chIndex, "%03d", index);
        newName.replace(newName.find("log") + 4, 3, chIndex);
        newPath.clear();
        newPath.append(LOG_FOLDER);
        newPath.append(newName);
        rename(oldPath.c_str(), newPath.c_str());
        index ++;
    }
}

void LogControl::getFileName(std::string &name, int &index) {
    // 根据lzma文件根数计算文件序号
    DIR * dir;
    struct dirent * ptr;
    index = 1;

    dir = opendir(LOG_FOLDER);
    if (dir == nullptr) {
        return;
    }

    while ((ptr = readdir(dir)) != nullptr) {
        if (endsWith(ptr->d_name, COMPRESS_SUFFIX)) {
            index ++;
        }
    }
    closedir(dir);

    // 根据系统当前和序号生成文件名
    char chName[64];
    struct tm tm{};
    time_t ts = time(nullptr);
    localtime_r(&ts,&tm);

    sprintf(chName, "%d-%02d-%02d-%02d-%02d-%02d.log.%03d.lzma", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, index);
    name.append(chName);
}

bool LogControl::mmapFile(const char *path) {
    // 打开文件（不存在则创建，读写权限）
    int fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd == -1) {
        std::cerr << "Failed to open file.\n";
        return false;
    }

    // 获取文件大小并扩展至10MB
    int size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        std::cerr << "Failed to determine file size.\n";
        close(fd);
        return false;
    }
    if (size < FILE_SZIE) {
        if (ftruncate(fd, FILE_SZIE) == -1) {
            std::cerr << "Failed to truncate file.\n";
            close(fd);
            return false;
        }
    }

    // 映射文件到内存
    mStartPtr = (char*) mmap(nullptr, FILE_SZIE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (mStartPtr == MAP_FAILED) {
        std::cerr << "Failed to map file to memory.\n";
        return false;
    }
    mEndPtr = mStartPtr + FILE_SZIE;
    size_t realSize = getFileRealSize(path);
    std::cout << "realSize: " << realSize << "\n";
    mPtr = mStartPtr + realSize;

    return true;
}

size_t LogControl::getFileRealSize(const char *path) {
    std::ifstream inputFile(path);
    if (!inputFile.is_open()) {
        std::cout << "File not found!\n";
        return 0;
    }

    std::string contents((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    const char searchChar = '\n';
    size_t lastPosition = contents.find_last_of(searchChar);
    inputFile.close();

    if (lastPosition != std::string::npos) {
        std::cout << "Last position found at: " << lastPosition;
        return lastPosition + 1;
    } else {
        std::cout << " searchChar not found!\n";
        return 0;
    }
}

bool LogControl::endsWith(std::string const &str, std::string const &suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

void LogControl::writeLogToFile(const std::string &logStr) {
    // 如果内存不足将文件压缩并重新创建log文件
    if (mPtr + logStr.size() > mEndPtr) {
        size_t leaveSize = mEndPtr - mPtr;
        if (leaveSize > 0) {
            string bankStr(leaveSize, ' ');
            memcpy(mPtr, bankStr.c_str(), bankStr.size());
        }
        msync(mStartPtr , mEndPtr - mStartPtr, MS_SYNC);
        munmap(mStartPtr, mEndPtr - mStartPtr);

        compressFile(LOG_FILE);
        initLogFile();
    }
    // 将log写入文件
    memcpy(mPtr, logStr.c_str(), logStr.size());
    mPtr += logStr.size();
}

void LogControl::writeLog() {
    // 将log从队列中取出，并逐条写入文件中
    pthread_mutex_lock(&mLogLock);
    while (mLogQueue.empty()) {
        pthread_cond_wait(&mLogCondition, &mLogLock);
    }

    deque<string>::iterator pIter;
    mWriteQueue.clear();
    for (pIter = mLogQueue.begin(); pIter != mLogQueue.end(); pIter++) {
        mWriteQueue.push_back(*pIter);
    }
    mLogQueue.clear();
    pthread_mutex_unlock(&mLogLock);

    deque<string>::iterator pExecIter;
    for(pExecIter = mWriteQueue.begin(); pExecIter != mWriteQueue.end(); pExecIter++) {
        writeLogToFile(*pExecIter);
    }
}

void LogControl::logMsg(const std::string &str) {
    pthread_mutex_lock(&mLogLock);
    mLogQueue.emplace_back(str);
    pthread_cond_signal(&mLogCondition);
    pthread_mutex_unlock(&mLogLock);
}

void *LogControl::threadLoop(void *args) {
    auto pInstance = (LogControl*) args;
    pInstance->initLogFile();
    while(true) {
        pInstance->writeLog();
    }
}



