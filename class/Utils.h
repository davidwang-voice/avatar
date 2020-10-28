#ifndef __Utils_H__
#define __Utils_H__

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#endif

#include "cocos2d.h"


static const std::string _CC_GAME_FILE_HTTP_PATH = "https://img.icons8.com/emoji/2x/";


#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
char* jstringToChar(JNIEnv* env, jstring jstr);
#endif

void getGameResourceUrl(std::string &res_url, const char* name);
void getGameResourcePath(std::string &res_path, const char* name);

#endif
