//
// Created by zel on 23-3-8.
//

#ifndef LOG_LZMAUTIL_H
#define LOG_LZMAUTIL_H

#include <iostream>
#ifdef __cplusplus
extern "C" {
#endif

#include "LzmaLib.h"

#ifdef __cplusplus
}
#endif


MY_STDAPI DefaultLzmaCompress(const unsigned char *src, size_t srcLen, unsigned char *dest,size_t *destLen);
MY_STDAPI DefaultLzmaUncompress(const unsigned char *src,size_t *srcLen, unsigned char *dest,size_t *destLen);

MY_STDAPI StdLzmaCompress(const unsigned char *src, size_t srcLen, unsigned char *dest,size_t *destLen);
MY_STDAPI StdLzmaUncompress(const unsigned char *src,size_t *srcLen, unsigned char *dest,size_t *destLen);

MY_STDAPI Compress(const char *input, const char *out);
MY_STDAPI Uncompress(const char *input, const char *out);

#endif //LOG_LZMAUTIL_H
