//
// Created by David on 2020/11/1.
//

#ifndef PROJ_ANDROID_OCCALLBACK_H
#define PROJ_ANDROID_OCCALLBACK_H

#include "cocos2d.h"

using namespace cocos2d;

typedef void (*OnTouchStageAvatarOCFunc)(void *object, const char* uid);
typedef void (*OnTouchStandAvatarOCFunc)(void *object, const char* uid);
typedef void (*OnTouchSceneOCFunc)(void *object);

class OCCallback {

public:
//    OCCallback();
//    ~OCCallback();

    void onTouchStageAvatar(const char* uid);
    void onTouchStandAvatar(const char* uid);
    void onTouchScene();


    void registerOnTouchStageAvatarOCFunc(void *object, OnTouchStageAvatarOCFunc);
    void registerOnTouchStandAvatarOCFunc(void *object, OnTouchStandAvatarOCFunc);
    void registerOnTouchSceneOCFunc(void *object, OnTouchSceneOCFunc);

    // Static Methods
    static OCCallback *getInstance();

private:
    // 要实现这个回调的对象
    void *onTouchStageAvatarOCObj;
    void *onTouchStandAvatarOCObj;
    void *onTouchSceneOCObj;

    // 函数声明
    OnTouchStageAvatarOCFunc onTouchStageAvatarOCFunc;
    OnTouchStandAvatarOCFunc onTouchStandAvatarOCFunc;

    OnTouchSceneOCFunc onTouchSceneOCFunc;
};


#endif //PROJ_ANDROID_OCCALLBACK_H
