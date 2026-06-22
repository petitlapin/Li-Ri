LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL3

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
		    $(LOCAL_PATH)/../SDL3_mixer \
		    $(LOCAL_PATH)/../simpleini \

# Add your application source files here...
LOCAL_SRC_FILES := ../../../../../../src/audio.cc \
	../../../../../../src/editor.cc \
	../../../../../../src/game.cc \
	../../../../../../src/gamepad.cc \
	../../../../../../src/level.cc \
	../../../../../../src/loco.cc \
	../../../../../../src/main.cc \
	../../../../../../src/menu.cc \
	../../../../../../src/mouse.cc \
	../../../../../../src/screen.cc \
	../../../../../../src/sprite.cc \
	../../../../../../src/utils.cc \

LOCAL_SHARED_LIBRARIES := SDL3 SDL3_mixer

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid  # SDL

include $(BUILD_SHARED_LIBRARY)
