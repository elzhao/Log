LOG_CommonCFlags = -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:= \
	lzma/XzCrc64Opt.c \
	lzma/XzDec.c \
	lzma/MtCoder.c \
	lzma/AesOpt.c \
	lzma/Bra.c \
	lzma/BraIA64.c \
	lzma/XzCrc64.c \
	lzma/Bcj2Enc.c \
	lzma/7zBuf2.c \
	lzma/LzFindMt.c \
	lzma/MtDec.c \
	lzma/Ppmd7Dec.c \
	lzma/LzmaDec.c \
	lzma/CpuArch.c \
	lzma/Alloc.c \
	lzma/Delta.c \
	lzma/Sha256Opt.c \
	lzma/7zCrc.c \
	lzma/LzFindOpt.c \
	lzma/Lzma2Dec.c \
	lzma/7zDec.c \
	lzma/7zArcIn.c \
	lzma/Sort.c \
	lzma/Bcj2.c \
	lzma/Threads.c \
	lzma/XzIn.c \
	lzma/DllSecur.c \
	lzma/Lzma2DecMt.c \
	lzma/7zStream.c \
	lzma/Xz.c \
	lzma/Bra86.c \
	lzma/Lzma2Enc.c \
	lzma/Lzma86Dec.c \
	lzma/7zFile.c \
	lzma/Ppmd7Enc.c \
	lzma/LzmaEnc.c \
	lzma/7zCrcOpt.c \
	lzma/Ppmd7.c \
	lzma/7zAlloc.c \
	lzma/7zBuf.c \
	lzma/LzmaLib.c \
	lzma/Sha256.c \
	lzma/Lzma86Enc.c \
	lzma/Aes.c \
	lzma/XzEnc.c \
	lzma/LzFind.c

LOCAL_MODULE := libqg_lzma

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)



include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:= \
	main.cpp \
	service/LogControl.cpp \
	service/QGLog.cpp \
	service/LzmaUtil.cpp

LOCAL_SHARED_LIBRARIES += \
	libqg_lzma

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/lzma \
	$(call include-path-for, corecg graphics)

$(warning $(LOCAL_CFLAGS))

LOCAL_MODULE := qg_log

include $(BUILD_EXECUTABLE)
