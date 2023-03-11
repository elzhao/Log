#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include <vector>
#include <random>
#include <stdarg.h>
#include <sys/time.h>

#include "LzmaUtil.h"
#include "LogControl.h"
#include "QGLog.h"

using namespace std;

int findLastPos() {
    std::ifstream inputFile("/home/zel/下载/ls/test/test.txt");
    if (!inputFile.is_open()) {
        std::cout << "File not found!\n";
        return -1;
    }

    std::string contents((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    char searchChar = '\n';
    std::size_t lastPosition = contents.find_last_of(searchChar);
    inputFile.close();
    if (lastPosition != std::string::npos) {
        std::cout << "Last position of " << searchChar << " found at: " << lastPosition << std::endl;
    } else {
        std::cout << searchChar << " not found!\n";
    }

    return lastPosition + 1;

}

int mapFile() {
    //    FILE *fp = fopen("/data/enlinzhao/下载/ls/test/test.txt", "rb+");
//    fseek(fp, 0, SEEK_END);
//    long longBytes = ftell(fp);
//    char* buffer = (char*) calloc(longBytes, sizeof(char));
//    fseek(fp, 0, SEEK_SET);
//    fread(buffer, sizeof(char), longBytes, fp);
//    std::cerr << "longBytes:" << longBytes << "\n";
//    std::cerr << "buffer:" << buffer << "\n";


    // 打开文件（不存在则创建，读写权限）
    int fd = open("/home/zel/下载/ls/test/test.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd == -1) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    // 获取文件大小并扩展至1KB
    int file_size = lseek(fd, 0, SEEK_END);
    std::cerr << "file_size:" << file_size << "\n";
    if (file_size == -1) {
        std::cerr << "Failed to determine file size.\n";
        close(fd);
        return 1;
    }
    if (file_size != 1024) {
        if (ftruncate(fd, 1024) == -1) {
            std::cerr << "Failed to truncate file.\n";
            close(fd);
            return 1;
        }
        file_size = 1024;
    }

    // 映射文件到内存
    char* data = (char*) mmap(nullptr, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (data == MAP_FAILED) {
        std::cerr << "Failed to map file to memory.\n";
        return 1;
    }
    data += findLastPos();

    // 写入数据并同步到磁盘
    std::string message = "Hello, world!\n";
    memcpy(data, message.c_str(), message.size());
    msync(data , message.size(), MS_SYNC);

    // 解除映射并关闭文件
    munmap(data, file_size);

    return 0;
}

void compress() {
    FILE *fp = fopen("/data/enlinzhao/test.txt", "rb+");
    fseek(fp, 0, SEEK_END);
    long longBytes = ftell(fp);
    unsigned char* buffer = (unsigned char*) calloc(longBytes, sizeof(char));
    fseek(fp, 0, SEEK_SET);
    fread(buffer, sizeof(char), longBytes, fp);
    fclose(fp);

    unsigned char *dest = (unsigned char*) calloc(longBytes, sizeof(char));
    size_t len = longBytes;
    int ret = StdLzmaCompress(buffer, longBytes, dest, &len);
    std::cerr << "longBytes: " << longBytes << "\n";
    std::cerr << "ret: " << ret << "\n";
    std::cerr << "len: " << len << "\n";

    FILE *fpc = fopen("/data/enlinzhao/test.txt.lzma", "wb+");
    fseek(fpc, 0, SEEK_SET);
    fwrite(dest, sizeof(char), len, fpc);
    fclose(fpc);
}

void uncompress() {
    FILE *fp = fopen("/data/enlinzhao/test.txt.lzma", "rb+");
    fseek(fp, 0, SEEK_END);
    size_t longBytes = ftell(fp);
    unsigned char* buffer = (unsigned char*) calloc(longBytes, sizeof(char));
    fseek(fp, 0, SEEK_SET);
    fread(buffer, sizeof(char), longBytes, fp);
    fclose(fp);
    std::cerr << "longBytes: " << longBytes << "\n";

    unsigned char *dest = (unsigned char*) calloc(10 * longBytes, sizeof(char));
    size_t len = 10 * longBytes;
    int ret = StdLzmaUncompress(buffer, &longBytes, dest, &len);
    std::cerr << "longBytes: " << longBytes << "\n";
    std::cerr << "ret: " << ret << "\n";
    std::cerr << "len: " << len << "\n";

    fp = fopen("/data/enlinzhao/test.txt", "wb+");
    fseek(fp, 0, SEEK_SET);
    fwrite(dest, sizeof(char), len, fp);
    fclose(fp);
}

void printCurrentTime() {
    struct tm tm;
    time_t ts = time(nullptr);
    localtime_r(&ts,&tm);

    char buf[32];
    strftime(buf,sizeof(buf),"%Y-%m-%d-%H-%M-%S",&tm);
    std::cout << "buf: " << buf << "\n";

    struct timeval tv{};
    gettimeofday(&tv,nullptr);

    sprintf(buf, "%d-%02d-%02d-%02d-%02d-%02d.%06ld", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec);
    std::cout << "buf: " << buf << "\n";
}

bool endsWith(std::string const &str, std::string const &suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

void printCurrentTime(char *buffer, int index) {
    struct tm tm;
    time_t ts = time(nullptr);
    localtime_r(&ts,&tm);

    sprintf(buffer, "%d-%02d-%02d-%02d-%02d-%02d.%010d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, index);
//    std::cout << "buf: " << buffer << "\n";
}


void randomStr(string &content) {
    const int len = 100;
    const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string str(len, 0); //初始化字符串
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, alphabet.size() - 1);
    // 生成随机字符串
    for (int i = 0; i < len; i++) {
        str[i] = alphabet[dis(gen)];
    }


//    char buffer[64];
//    printCurrentTime(buffer, 0);
//    str.insert(0, buffer);

    content = str;
}

void testWriteLog() {
    LogControl *pInstance = LogControl::getInstance();
    std::cout << "222" << "\n";
//    char buffer[64];
    string content;
    for (int i = 0; i < 10000000; i++) {
        //printCurrentTime(buffer, i);
        randomStr(content);
//        content.append("\n");
//        pInstance->logMsg(content);
        log(content.c_str());
        usleep(1 * 1000);
        //std::cout << "buffer: " << buffer << "\n";
    }
}

void checkLogFolder() {
    // 计算目录占用内存大小
    DIR * dir;
    struct dirent * ptr;

    dir = opendir("/home/zel/下载/ls/test");
    if (dir == nullptr) {
        return;
    }

    while ((ptr = readdir(dir)) != nullptr) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        std::cout << "d_name: " << ptr->d_name << "\n";
    }
    closedir(dir);


}

void combine_strings(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[1000];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    printf("%s\n", buffer);
}

int main() {
//    combine_strings("aaa %d", 1);
//    std::cout << "endsWith: " << endsWith("asdasd.lzma", "lzma") << "\n";
//    char buffer[10];
//    sprintf(buffer, "%03d", 1);
//    std::cout << "buffer: " << buffer << "\n";
//    printCurrentTime();
//    findLastPos();
//    return mapFile();
//    compress();

//    Compress("/home/zel/下载/ls/test/test.txt", "/home/zel/下载/ls/test/test.txt.lzma");
//    testWriteLog();

//    checkLogFolder();
//    uncompress();
//    Uncompress("/home/zel/下载/ls/test/test.txt.lzma", "/home/zel/下载/ls/test/test.txt");


//    std::string s = "2020-01-01-08-30-09.log.131.lzma";
//    char chIndex[16];
//    sprintf(chIndex, "%03d", 12);
//    s.replace(s.find("log") + 4, 3, chIndex);
//    std::cout << "s: " << s << "\n";


    return 0;
}
