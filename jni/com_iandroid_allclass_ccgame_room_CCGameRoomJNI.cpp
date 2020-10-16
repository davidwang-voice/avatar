//
// Created by David on 2020/10/14.
//
#include <jni.h>
#include <RoomManager.h>
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
    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_receiveGiftMessage(JNIEnv *env, jobject thiz,
                                                                            jint id, jstring path) {
        RoomManager::getInstance()->receiveGiftMessage(id, jstringToChar(env, path));
    }
    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_receiveChatMessage(JNIEnv *env, jobject thiz,
                                                                            jint id, jstring content) {
        RoomManager::getInstance()->receiveChatMessage(id, jstringToChar(env, content));
    }
    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_updateStageAvatars(JNIEnv *env, jobject thiz,
                                                                            jstring json) {
        RoomManager::getInstance()->updateStageAvatars(jstringToChar(env, json));
    }
    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_updateStandAvatars(JNIEnv *env, jobject thiz,
                                                                            jstring json) {
        RoomManager::getInstance()->updateStandAvatars(jstringToChar(env, json));
    }
    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_backOffStageAvatar(JNIEnv *env, jobject thiz,
                                                                            jint id) {
        RoomManager::getInstance()->backOffStageAvatar(id);
    }
    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_backOffStandAvatar(JNIEnv *env, jobject thiz,
                                                                            jint id) {
        RoomManager::getInstance()->backOffStandAvatar(id);
    }
    JNIEXPORT void JNICALL Java_com_iandroid_allclass_ccgame_room_CCGameRoomJNI_releaseResource(JNIEnv *env, jobject thiz) {
        RoomManager::getInstance()->releaseResource();
    }

}

void onTouchedAvatar(int id) {
    JniHelper::callStaticVoidMethod(className, "onTouchedAvatar", id);
}

void onTouchDown() {
    
}

void onTouchUp() {
    JniHelper::callStaticVoidMethod(className, "onTouchedScene");
}
