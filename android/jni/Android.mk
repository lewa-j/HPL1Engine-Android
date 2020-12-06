LOCAL_PATH:= $(call my-dir)/../..

include $(CLEAR_VARS)
LOCAL_MODULE := hpl1game
LOCAL_CPP_EXTENSION := .cpp
LOCAL_SRC_FILES := tests/SpotLightTest/SpotLightTest.cpp \
tests/Common/SimpleCamera.cpp

LOCAL_STATIC_LIBRARIES := hpl1engine
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/dependencies
LOCAL_CPPFLAGS := -std=c++11
LOCAL_LDFLAGS :=
#LOCAL_CPPFLAGS += -g -ggdb
#LOCAL_LDFLAGS += -g -ggdb

include $(BUILD_SHARED_LIBRARY)

include engine.mk


