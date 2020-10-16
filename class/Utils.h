#ifndef __Utils_H__
#define __Utils_H__

#include <jni.h>
#include "platform/android/jni/JniHelper.h"

char* jstringToChar(JNIEnv* env, jstring jstr);

#endif
