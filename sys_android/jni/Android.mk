LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -DANDROID -DGL_GLEXT_PROTOTYPES=1

LOCAL_MODULE := game	
LOCAL_SRC_FILES := main.cpp \
	../../src/core.cpp \
	../../src/utils.cpp \
	../../src/renderGL.cpp \
	../../src/resource.cpp \
	../../src/game.cpp \
	../../src/scene.cpp \

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../src/ \

LOCAL_LDLIBS := -lGLESv2 -ldl -llog

include $(BUILD_SHARED_LIBRARY)
