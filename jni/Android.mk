LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE	:= qing
NDK_MODULE_PATH := ./

MY_FILES_PATH  :=  $(LOCAL_PATH)/../src/net \
                   $(LOCAL_PATH)/../src/util \
                   $(LOCAL_PATH)/../src/xml \
                   $(LOCAL_PATH)/../src/parallel

MY_FILES_SUFFIX := %.cpp %.c %.cc

My_All_Files := $(foreach src_path,$(MY_FILES_PATH), $(shell find $(src_path) -type f) ) 
My_All_Files := $(My_All_Files:$(MY_CPP_PATH)/./%=$(MY_CPP_PATH)%)
MY_SRC_LIST  := $(filter $(MY_FILES_SUFFIX),$(My_All_Files)) 
MY_SRC_LIST  := $(MY_SRC_LIST:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(MY_SRC_LIST)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include

LOCAL_CFLAGS := -I"$(LOCAL_PATH)/../include"

LOCAL_CPPFLAGS := -std=c++11 -pthread -frtti -fexceptions

#LOCAL_LDLIBS :=  -landroid

include $(BUILD_STATIC_LIBRARY)

#$(call import-module,tcmalloc)
