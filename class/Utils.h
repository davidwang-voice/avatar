#ifndef __Utils_H__
#define __Utils_H__

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "cocos2d.h"


static const std::string _CC_GAME_FILE_HTTP_PATH = "https://avatar.csdn.net/A/6/5/";

char* jstringToChar(JNIEnv* env, jstring jstr);


const char* getGameResourceUrl(const char* name);
const char* getGameResourcePath(const char* name);

#endif
