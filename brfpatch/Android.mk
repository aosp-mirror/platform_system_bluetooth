# Copyright 2007 The Android Open Source Project
#
# Utility to create Android bluetooth firmware from Texas Instruments .bts
# scripts

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	brfpatch.c

LOCAL_MODULE := brfpatch

include $(BUILD_HOST_EXECUTABLE)
