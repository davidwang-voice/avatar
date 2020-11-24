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

void OCCallback::onTouchStageAvatar(const char *uid) {
    if (nullptr != onTouchStageAvatarOCObj && nullptr != onTouchStageAvatarOCFunc) {
        (*onTouchStageAvatarOCFunc)(onTouchStageAvatarOCObj, uid);
    }
}

void OCCallback::onTouchStandAvatar(const char *uid) {
    if (nullptr != onTouchStandAvatarOCObj && nullptr != onTouchStandAvatarOCFunc) {
        (*onTouchStandAvatarOCFunc)(onTouchStandAvatarOCObj, uid);
    }
}

void OCCallback::onTouchScene() {
    if (nullptr != onTouchSceneOCObj && nullptr != onTouchSceneOCFunc) {
        (*onTouchSceneOCFunc)(onTouchSceneOCObj);
    }

}

void OCCallback::registerOnTouchStageAvatarOCFunc(void *object, OnTouchStageAvatarOCFunc func) {
    this->onTouchStageAvatarOCObj = object;
    this->onTouchStageAvatarOCFunc = func;
}

void OCCallback::registerOnTouchStandAvatarOCFunc(void *object, OnTouchStandAvatarOCFunc func) {
    this->onTouchStandAvatarOCObj = object;
    this->onTouchStandAvatarOCFunc = func;
}


void OCCallback::registerOnTouchSceneOCFunc(void *object, OnTouchSceneOCFunc func) {
    this->onTouchSceneOCObj = object;
    this->onTouchSceneOCFunc = func;

}
