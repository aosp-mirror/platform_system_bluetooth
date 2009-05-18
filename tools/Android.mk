LOCAL_PATH:= $(call my-dir)

#
# bttest
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES := bttest.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../bluedroid/include

LOCAL_SHARED_LIBRARIES := libbluedroid

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := bttest

include $(BUILD_EXECUTABLE)

BUILD_EXTRA_BT_TOOLS:=false

ifeq ($(BUILD_EXTRA_BT_TOOLS),true)

#
# socktest
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES := socktest.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../bluedroid/include \
	$(LOCAL_PATH)/../bluez-clean-headers

LOCAL_SHARED_LIBRARIES := libbluedroid

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := socktest

include $(BUILD_EXECUTABLE)

#
# sock_shutdown_test
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES := sock_shutdown_test.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../bluez-clean-headers

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := sock_shutdown_test

include $(BUILD_EXECUTABLE)

#
# pipetest
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES := pipetest.c

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := pipetest

include $(BUILD_EXECUTABLE)

endif
