//
// Created by zel on 23-3-8.
//
#include <cstring>
#include "LzmaUtil.h"

MY_STDAPI DefaultLzmaCompress(const unsigned char *src, const size_t srcLen, unsigned char *dest, size_t *destLen) {
    int level = 5;
    unsigned dictSize = 1 << 23;
    int lc = 3;
    int lp = 0;
    int pb = 2;
    int fb = 128;
    int numThreads = 1;
    size_t propSize = LZMA_PROPS_SIZE;
    int ret = LzmaCompress(dest + propSize, destLen, src, srcLen, dest, &propSize, level, dictSize, lc, lp, pb, fb, numThreads);
    *destLen += LZMA_PROPS_SIZE;
    return ret;
}

MY_STDAPI DefaultLzmaUncompress(const unsigned char *src, size_t *srcLen, unsigned char *dest, size_t *destLen) {
    size_t propSize = LZMA_PROPS_SIZE;
    *srcLen -= propSize;
    return LzmaUncompress(dest, destLen, src + propSize, srcLen, src, propSize);
}

MY_STDAPI StdLzmaCompress(const unsigned char *src, const size_t srcLen, unsigned char *dest, size_t *destLen) {
    int level = 5;
    unsigned dictSize = 1 << 23;
    int lc = 3;
    int lp = 0;
    int pb = 2;
    int fb = 128;
    int numThreads = 1;
    size_t propSize = LZMA_PROPS_SIZE;

    const int headSize = 8;
    Byte head[8];
    for (int i = 0; i < headSize; i++) {
        head[i] = (Byte) (srcLen >> (8 * i));
    }
    memcpy(dest + LZMA_PROPS_SIZE, head, 8);

    int ret = LzmaCompress(dest + propSize + headSize, destLen, src, srcLen, dest, &propSize, level, dictSize, lc, lp, pb, fb, numThreads);
    *destLen += LZMA_PROPS_SIZE;
    *destLen += headSize;
    return ret;
}

MY_STDAPI StdLzmaUncompress(const unsigned char *src, size_t *srcLen, unsigned char *dest, size_t *destLen) {
    const int headSize = 8;

    size_t propSize = LZMA_PROPS_SIZE;
    *srcLen -= propSize;
    *srcLen -= headSize;
    return LzmaUncompress(dest, destLen, src + propSize + headSize, srcLen, src, propSize);
}

MY_STDAPI Compress(const char *input, const char *out) {
    FILE *fp = fopen(input, "rb+");
    if (fp == nullptr) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    size_t fileSize = ftell(fp);

    Byte *buffer = (unsigned char*) calloc(fileSize, sizeof(char));
    fseek(fp, 0, SEEK_SET);
    fread(buffer, sizeof(char), fileSize, fp);
    fclose(fp);

    Byte *dest = (unsigned char*) calloc(fileSize, sizeof(char));
    size_t len = fileSize;
    int ret = StdLzmaCompress(buffer, fileSize, dest, &len);
    std::cerr << "longBytes: " << fileSize << "\n";
    std::cerr << "ret: " << ret << "\n";
    std::cerr << "len: " << len << "\n";

    fp = fopen(out, "wb+");
    fseek(fp, 0, SEEK_SET);
    fwrite(dest, sizeof(char), len, fp);
    fflush(fp);
    fclose(fp);
    free(buffer);
    free(dest);
    return 0;
}

MY_STDAPI Uncompress(const char *input, const char *out) {
    FILE *fp = fopen(input, "rb+");
    if (fp == nullptr) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    size_t fileSize = ftell(fp);

    Byte* buffer = (unsigned char*) calloc(fileSize, sizeof(char));
    fseek(fp, 0, SEEK_SET);
    fread(buffer, sizeof(char), fileSize, fp);
    fclose(fp);
    std::cerr << "fileSize: " << fileSize << "\n";

    Byte *dest = (unsigned char*) calloc(10 * fileSize, sizeof(char));
    size_t len = 10 * fileSize;
    int ret = StdLzmaUncompress(buffer, &fileSize, dest, &len);
    std::cerr << "fileSize: " << fileSize << "\n";
    std::cerr << "ret: " << ret << "\n";
    std::cerr << "len: " << len << "\n";

    fp = fopen(out, "wb+");
    fseek(fp, 0, SEEK_SET);
    fwrite(dest, sizeof(char), len, fp);
    fflush(fp);
    fclose(fp);
    free(buffer);
    free(dest);
    return 0;
}


