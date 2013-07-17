  /*=================================================================}
 /  <<<<<<<<<<<<<<<<<<<<<--- Harsh Engine --->>>>>>>>>>>>>>>>>>>>>>  }
/  e-mail  : xproger@list.ru                                         }
{  github  : https://github.com/xproger/harsh                        }
{====================================================================}
{ LICENSE:                                                           }
{ Copyright (c) 2013, Timur "XProger" Gagiev                         }
{ All rights reserved.                                               }
{                                                                    }
{ Redistribution and use in source and binary forms, with or without /
{ modification, are permitted under the terms of the BSD License.   /
{=================================================================*/
#include <jni.h>
#include <stdint.h>
#include <sys/time.h>
#include "core.h"
#include "resource.h"
#include <android/log.h>

size_t startTime;

extern "C" {
	JNIEXPORT void JNICALL Java_com_xproger_harsh_GameActivity_nativeInit(JNIEnv* env, jobject obj, jstring path);
	JNIEXPORT void JNICALL Java_com_xproger_harsh_GameActivity_nativeFree(JNIEnv* env);
	JNIEXPORT void JNICALL Java_com_xproger_harsh_GameActivity_nativePause(JNIEnv* env);
	JNIEXPORT void JNICALL Java_com_xproger_harsh_GameActivity_nativeResume(JNIEnv* env);
	JNIEXPORT void JNICALL Java_com_xproger_harsh_GameActivity_nativeSoundFill(JNIEnv* env, jobject obj, jshortArray buffer);
	JNIEXPORT void JNICALL Java_com_xproger_harsh_GameRenderer_nativeReset(JNIEnv* env);
	JNIEXPORT void JNICALL Java_com_xproger_harsh_GameRenderer_nativeUpdate(JNIEnv* env);
	JNIEXPORT void JNICALL Java_com_xproger_harsh_GameRenderer_nativeRender(JNIEnv* env);
	JNIEXPORT void JNICALL Java_com_xproger_harsh_GameRenderer_nativeResize(JNIEnv* env, jobject obj, jint w, jint h);
	JNIEXPORT void JNICALL Java_com_xproger_harsh_GameGLSurfaceView_nativeTouch(JNIEnv* env, jobject obj, jint id, jint x, jint y, jint state);
};

int getTime() {
	timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000) + (time.tv_usec / 1000) - startTime;
}

void Java_com_xproger_harsh_GameActivity_nativeInit(JNIEnv * env, jobject obj, jstring path) {
	startTime = 0;
	startTime = size_t(getTime());
	const char* buffer = env->GetStringUTFChars(path, NULL);
	Core::init(buffer, getTime);
	env->ReleaseStringUTFChars(path, buffer);
}

void Java_com_xproger_harsh_GameActivity_nativeFree(JNIEnv * env) {
	Core::deinit();
}

void Java_com_xproger_harsh_GameActivity_nativePause(JNIEnv * env) {
	Core::pause();
}

void Java_com_xproger_harsh_GameActivity_nativeResume(JNIEnv * env) {
	Core::resume();
}

void Java_com_xproger_harsh_GameActivity_nativeSoundFill(JNIEnv* env, jobject obj, jshortArray buffer) {
	jshort *frames = env->GetShortArrayElements(buffer, NULL);
	jsize count = env->GetArrayLength(buffer) / 2;
	Sound::fill((SoundFrame*)frames, count);
	env->ReleaseShortArrayElements(buffer, frames, 0);
}

void Java_com_xproger_harsh_GameRenderer_nativeReset(JNIEnv * env) {
	Core::reset();
}

void JNICALL Java_com_xproger_harsh_GameRenderer_nativeUpdate(JNIEnv* env) {
	Core::update();
}

void Java_com_xproger_harsh_GameRenderer_nativeRender(JNIEnv * env) {
	Core::render();
}

void Java_com_xproger_harsh_GameRenderer_nativeResize(JNIEnv * env, jobject  thiz, jint w, jint h) {
	Core::resize(w, h);
}

void Java_com_xproger_harsh_GameGLSurfaceView_nativeTouch(JNIEnv * env, jobject thiz, jint id, jint x, jint y, jint state) {
	Core::touch(id, state, x, y);
}

