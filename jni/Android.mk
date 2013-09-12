
LOCAL_PATH := $(call my-dir)

USE_CUSTOM_IOMX := false

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS += -Wno-non-virtual-dtor 

LOCAL_SRC_FILES =       \
	surfacemain.cpp \
	dp.cpp 


LOCAL_C_INCLUDES:= 


ifeq ($(USE_CUSTOM_IOMX), true)
LOCAL_SHARED_LIBRARIES += libomxcodec-14
#LOCAL_SHARED_LIBRARIES += libomxcodec-15
else
LOCAL_SHARED_LIBRARIES += libcodec-14
endif

LOCAL_PRELINK_MODULE := false

LOCAL_STATIC_LIBRARIES := 

LOCAL_LDLIBS += -llog -landroid

LOCAL_MODULE := libtestcustomdatasource_jni

include $(BUILD_SHARED_LIBRARY)

SYSTEM_IOMX_PATH := $(LOCAL_PATH)/codec-14/Android.mk
CUSTOME_IOMX_14_PATH := $(LOCAL_PATH)/omxcodec-14/Android.mk
CUSTOME_IOMX_15_PATH := $(LOCAL_PATH)/omxcodec-15/Android.mk
MPEG2TS_PATH := $(LOCAL_PATH)/cimpeg2ts/Android.mk

ifeq ($(USE_CUSTOM_IOMX), true)
include $(CUSTOME_IOMX_14_PATH)
#include $(CUSTOME_IOMX_15_PATH)
else
include $(SYSTEM_IOMX_PATH)
endif

#include $(MPEG2TS_PATH)
