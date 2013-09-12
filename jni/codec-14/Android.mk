
LOCAL_PATH := $(call my-dir)

PRIVATELIB_PATH := $(call my-dir)/../codec-15

$(info path $(LOCAL_PATH))

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS += -Wno-non-virtual-dtor -DHAVE_PTHREADS -Wno-multichar

LOCAL_SRC_FILES =       \
	dp.cpp \
	OMXHarness.cpp  \
	CidanaMediaSource.cpp	

#	DummyAudioSource.cpp

#LOCAL_SHARED_LIBRARIES += $(LOCAL_PATH)/lib/libstagefright.so
LOCAL_LDFLAGS += $(PRIVATELIB_PATH)/lib/libstagefright.so
LOCAL_LDFLAGS += $(PRIVATELIB_PATH)/lib/libutils.so
LOCAL_LDFLAGS += $(PRIVATELIB_PATH)/lib/libcutils.so

# set include path
LOCAL_C_INCLUDES:= \
	$(LOCAL_PATH)/../ \
	$(LOCAL_PATH)/../cimpeg2ts \
	$(PRIVATELIB_PATH)/include/frameworks/base/include \
	$(PRIVATELIB_PATH)/include/system/core/include \
	$(PRIVATELIB_PATH)/include/frameworks/base/include/media/stagefright/openmax \
	$(PRIVATELIB_PATH)/include/hardware/libhardware/include \
	$(PRIVATELIB_PATH)/include/frameworks/base/media/libstagefright/include \
	$(PRIVATELIB_PATH)/include/frameworks/av/include/	\
	$(PRIVATELIB_PATH)/include/frameworks/native/include/		\
	$(PRIVATELIB_PATH)/include/frameworks/native/include/media/openmax/	\
	$(PRIVATELIB_PATH)/include/frameworks/av/media/libstagefright/include/	\
	
LOCAL_SHARED_LIBRARIES := libcimpeg2ts
LOCAL_STATIC_LIBRARIES := 

LOCAL_LDLIBS += -llog -landroid

LOCAL_MODULE := libcodec-14


include $(BUILD_SHARED_LIBRARY)

##########################

