//
// Created by David on 2020/11/1.
//

#ifndef PROJ_ANDROID_OCCALLBACK_H
#define PROJ_ANDROID_OCCALLBACK_H

#include "cocos2d.h"

using namespace cocos2d;

typedef void (*OnTouchAvatarOCFunc)(void *object, const char* uid);
typedef void (*OnTouchSceneOCFunc)(void *object);

class OCCallback {

public:
//    OCCallback();
//    ~OCCallback();

    void onTouchAvatar(const char* uid);
    void onTouchScene();


    void registerOnTouchAvatarOCFunc(void *object, OnTouchAvatarOCFunc);
    void registerOnTouchSceneOCFunc(void *object, OnTouchSceneOCFunc);

    // Static Methods
    static OCCallback *getInstance();

private:
    // 要实现这个回调的对象
    void *onTouchAvatarOCObj;
    void *onTouchSceneOCObj;

    // 函数声明
    OnTouchAvatarOCFunc onTouchAvatarOCFunc;

    OnTouchSceneOCFunc onTouchSceneOCFunc;
};


#endif //PROJ_ANDROID_OCCALLBACK_H
