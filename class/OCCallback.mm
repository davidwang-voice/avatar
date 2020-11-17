//
// Created by David on 2020/11/1.
//

#include "OCCallback.h"



static OCCallback *instance = nullptr;

OCCallback *OCCallback::getInstance() {
    if (!instance) {
        instance = new OCCallback();
    }
    return instance;
}

void OCCallback::onTouchAvatar(const char *uid) {
    if (nullptr != onTouchAvatarOCObj && nullptr != onTouchAvatarOCFunc) {
        (*onTouchAvatarOCFunc)(onTouchAvatarOCObj, uid);
    }
}

void OCCallback::onTouchScene() {
    if (nullptr != onTouchSceneOCObj && nullptr != onTouchSceneOCFunc) {
        (*onTouchSceneOCFunc)(onTouchSceneOCObj);
    }

}

void OCCallback::registerOnTouchAvatarOCFunc(void *object, OnTouchAvatarOCFunc func) {
    this->onTouchAvatarOCObj = object;
    this->onTouchAvatarOCFunc = func;


}

void OCCallback::registerOnTouchSceneOCFunc(void *object, OnTouchSceneOCFunc func) {
    this->onTouchSceneOCObj = object;
    this->onTouchSceneOCFunc = func;

}
