
LOCAL_PATH := $(call my-dir)

$(info path $(LOCAL_PATH))

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS += -Wno-non-virtual-dtor -DHAVE_PTHREADS

LOCAL_SRC_FILES =       \
	dp.cpp \
	OMXCodec.cpp  \
	OMXHarness.cpp 
	

LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/libstagefright.so
LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/libutils.so
LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/libcutils.so
LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/libbinder.so
LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/libmedia.so
LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/libstagefright_foundation.so
LOCAL_LDFLAGS += $(LOCAL_PATH)/lib/libui.so

# set include path
LOCAL_C_INCLUDES:= \
	$(LOCAL_PATH)/../ \
	$(LOCAL_PATH)/include/frameworks/base/include \
	$(LOCAL_PATH)/include/system/core/include \
	$(LOCAL_PATH)/include/frameworks/base/include/media/stagefright/openmax \
	$(LOCAL_PATH)/include/hardware/libhardware/include \
	$(LOCAL_PATH)/include/frameworks/base/media/libstagefright/include \
	

LOCAL_STATIC_LIBRARIES := 

LOCAL_LDLIBS += -llog -landroid

LOCAL_MODULE := libomxcodec-14


include $(BUILD_SHARED_LIBRARY)

##########################

