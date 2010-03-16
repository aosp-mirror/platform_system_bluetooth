#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

dest_dir := $(TARGET_OUT)/etc/bluetooth

files := \
	audio.conf \
	input.conf \
	main.conf \
	blacklist.conf \
	auto_pairing.conf

copy_to := $(addprefix $(dest_dir)/,$(files))

$(copy_to): PRIVATE_MODULE := bluetooth_etcdir
$(copy_to): $(dest_dir)/%: $(LOCAL_PATH)/% | $(ACP)
	$(transform-prebuilt-to-target)

ALL_PREBUILT += $(copy_to)
