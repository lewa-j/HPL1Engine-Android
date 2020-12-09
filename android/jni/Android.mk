LOCAL_PATH:= $(call my-dir)/../..

include $(CLEAR_VARS)
LOCAL_MODULE := hpl1game
LOCAL_CPP_EXTENSION := .cpp
LOCAL_SRC_FILES := tests/SceneTest/SceneTest.cpp tests/SceneTest/SceneCamera.cpp
#tests/Common/SimpleCamera.cpp tests/SpotLightTest/SpotLightTest.cpp
#tests/GuiTest/GuiTest.cpp

LOCAL_STATIC_LIBRARIES := hpl1engine
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/dependencies
LOCAL_CPPFLAGS := -std=c++11
LOCAL_LDFLAGS :=
#LOCAL_CPPFLAGS += -g -ggdb
#LOCAL_LDFLAGS += -g -ggdb

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/android/jni/engine.mk


