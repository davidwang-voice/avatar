#ifndef __Utils_H__
#define __Utils_H__

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#endif

#include "cocos2d.h"


static const std::string _CC_GAME_FILE_HTTP_PATH = "https://img.icons8.com/emoji/2x/";


static const long _TOUCH_EVENT_TAP_VALID_MS = 200;
static const long _TOUCH_EVENT_TAP_GAP_MS = 200;
static long long _touch_down_timestamp = 0;
static long long _touch_tap_timestamp = 0;

bool __touchBegin();

bool __isTapEvent();

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
char* jstringToChar(JNIEnv* env, jstring jstr);
#endif

void getGameResourceUrl(std::string &res_url, const char* name);
void getGameResourcePath(std::string &res_path, const char* name);

std::string getStringMD5Hash(const std::string &string);

#endif

