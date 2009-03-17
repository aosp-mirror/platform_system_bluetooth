#
# libbluedroid
#

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	bluetooth.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	system/bluetooth/bluez-clean-headers

LOCAL_SHARED_LIBRARIES := \
	libcutils

LOCAL_MODULE := libbluedroid

include $(BUILD_SHARED_LIBRARY)


#
# bttest
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES := bttest.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SHARED_LIBRARIES := libbluedroid

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := bttest

include $(BUILD_EXECUTABLE)
