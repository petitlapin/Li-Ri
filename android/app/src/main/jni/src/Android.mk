LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/ \
		    $(LOCAL_PATH)/../SDL2_mixer \
		    $(LOCAL_PATH)/../simpleini \

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	../../../../../../src/audio.cc \
	../../../../../../src/editor.cc \
	../../../../../../src/game.cc \
	../../../../../../src/level.cc \
	../../../../../../src/loco.cc \
	../../../../../../src/main.cc \
	../../../../../../src/menu.cc \
	../../../../../../src/mouse.cc \
	../../../../../../src/screen.cc \
	../../../../../../src/sprite.cc \
	../../../../../../src/utils.cc \

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
