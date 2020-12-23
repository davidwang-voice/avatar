//
// Created by David on 2020/10/14.
//
#include <jni.h>
#include <CCRoomDelegate.h>
#include "platform/android/CCApplication-android.h"
#include "platform/android/CCGLViewImpl-android.h"
#include "platform/android/jni/JniHelper.h"
#include "Utils.h"
#include "AppDelegate.h"

#include "com_iandroid_allclass_ccgame_room_CCGameRoomJNI.h"


static const std::string className = "com.iandroid.allclass.ccgame.room.CCGameRoomJNI";

using namespace cocos2d;

extern "C" {
    JNIEXPORT jintArray JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_getGLContextAttrs(JNIEnv *env, jobject thiz) {
        cocos2d::Application::getInstance()->initGLContextAttrs();
        GLContextAttrs _glContextAttrs = GLView::getGLContextAttrs();

        int tmp[7] = {_glContextAttrs.redBits, _glContextAttrs.greenBits, _glContextAttrs.blueBits,
                      _glContextAttrs.alphaBits, _glContextAttrs.depthBits, _glContextAttrs.stencilBits,
                      _glContextAttrs.multisamplingCount};


        jintArray glContextAttrsJava = env->NewIntArray(7);
        env->SetIntArrayRegion(glContextAttrsJava, 0, 7, tmp);

        return glContextAttrsJava;
    }

    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_setDesignResolution(JNIEnv *env, jobject thiz,
                                                                             jfloat width,
                                                                             jfloat height) {
        AppDelegate::setDesignResolutionSize(width, height);
    }
    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_setStageBackground(JNIEnv *env, jobject thiz, jstring url) {
        CCRoomDelegate::getInstance()->setStageBackground(jstringToChar(env, url));

    }

    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_setupStageGiftHeap(JNIEnv *env, jobject thiz,
                                                                          jstring json) {
        CCRoomDelegate::getInstance()->setupStageGiftHeap(jstringToChar(env, json));
    }

    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_updateSelfAvatar(JNIEnv *env, jobject thiz,
                                                                          jstring json) {
        CCRoomDelegate::getInstance()->updateSelfAvatar(jstringToChar(env, json));
    }

    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_updateStageAvatars(JNIEnv *env, jobject thiz,
                                                                                                   jstring json) {
        CCRoomDelegate::getInstance()->updateStageAvatars(jstringToChar(env, json));
    }

    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_updateStandAvatars(JNIEnv *env, jobject thiz,
                                                                                                   jstring json) {
        CCRoomDelegate::getInstance()->updateStandAvatars(jstringToChar(env, json));
    }
    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_backOffStageAvatar(JNIEnv *env, jobject thiz,
                                                                                                   jstring uid) {
        CCRoomDelegate::getInstance()->backOffStageAvatar(jstringToChar(env, uid));
    }
    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_backOffStandAvatar(JNIEnv *env, jobject thiz,
                                                                                                   jstring uid) {
        CCRoomDelegate::getInstance()->backOffStandAvatar(jstringToChar(env, uid));
    }

    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_receiveGiftMessage(JNIEnv *env, jobject thiz,
                                                                                                   jstring uid, jstring url) {
        CCRoomDelegate::getInstance()->receiveGiftMessage(jstringToChar(env, uid), jstringToChar(env, url));
    }
    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_receiveChatMessage(JNIEnv *env, jobject thiz,
                                                                                                   jstring uid, jstring content) {
        CCRoomDelegate::getInstance()->receiveChatMessage(jstringToChar(env, uid), jstringToChar(env, content));
    }

    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_receiveVoiceWave(JNIEnv *env, jobject thiz,
                                                                      jstring uids) {
        CCRoomDelegate::getInstance()->receiveVoiceWave(jstringToChar(env, uids));
    }

    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_receiveRandomSnore(JNIEnv *env, jobject thiz, jstring uids) {
        CCRoomDelegate::getInstance()->receiveRandomSnore(jstringToChar(env, uids));
    }

    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_releaseResource(JNIEnv *env, jobject thiz) {
        CCRoomDelegate::getInstance()->releaseResource();
    }

}

void Java_onTouchStageAvatar(const char* uid) {
    JniHelper::callStaticVoidMethod(className, "onTouchStageAvatar", uid);
}

void Java_onTouchStandAvatar(const char* uid) {
    JniHelper::callStaticVoidMethod(className, "onTouchStandAvatar", uid);
}

void Java_onTouchDown() {
    
}

void Java_onTouchScene() {
    JniHelper::callStaticVoidMethod(className, "onTouchScene");
}