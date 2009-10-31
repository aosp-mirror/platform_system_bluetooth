BUILD_EXTRA_BT_TOOLS:=false

LOCAL_PATH:= $(call my-dir)

#
# bttest
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES := bttest.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../bluedroid/include \
	system/bluetooth/bluez-clean-headers

LOCAL_SHARED_LIBRARIES := libbluedroid

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := bttest

include $(BUILD_EXECUTABLE)

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
# asocket_test
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES := asocket_test.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../bluez-clean-headers

LOCAL_SHARED_LIBRARIES := libcutils

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := asocket_test

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
# sock_shutdown_bug_l2cap
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES := sock_shutdown_bug_l2cap.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../bluez-clean-headers

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := sock_shutdown_bug_l2cap

LOCAL_SHARED_LIBRARIES := libbluetooth

include $(BUILD_EXECUTABLE)

#
# sock_shutdown_bug_rfcomm
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES := sock_shutdown_bug_rfcomm.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../bluez-clean-headers

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := sock_shutdown_bug_rfcomm

LOCAL_SHARED_LIBRARIES := libbluetooth

include $(BUILD_EXECUTABLE)

#
# sock_shutdown_bug_tcp
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES := sock_shutdown_bug_tcp.c

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := sock_shutdown_bug_tcp

LOCAL_SHARED_LIBRARIES := libbluetooth

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
