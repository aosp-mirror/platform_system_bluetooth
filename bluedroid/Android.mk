#
# libbluedroid
#

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(BOARD_BLUETOOTH_DOES_NOT_USE_RFKILL),true)
  LOCAL_CFLAGS := $(LOCAL_CFLAGS) -DBLUETOOTH_DOES_NOT_USE_RFKILL
endif

ifeq ($(BOARD_BLUETOOTH_USES_HCIATTACH_PROPERTY),true)
  LOCAL_CFLAGS := $(LOCAL_CFLAGS) -DBLUETOOTH_HCIATTACH_USING_PROPERTY
endif

LOCAL_SRC_FILES := \
	bluetooth.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	system/bluetooth/bluez-clean-headers

LOCAL_SHARED_LIBRARIES := \
	libcutils

LOCAL_MODULE := libbluedroid

include $(BUILD_SHARED_LIBRARY)
