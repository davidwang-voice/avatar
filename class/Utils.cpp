#include "Utils.h"

#include "base/ccUtils.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

char *jstringToChar(JNIEnv *env, jstring jstr) {
    char *rtn = "";
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

#endif

void getGameResourceUrl(std::string &res_url, const char *name) {
//    res_url.append(_CC_GAME_FILE_HTTP_PATH);

    res_url.append(name);

}

void getGameResourcePath(std::string &res_path, const char *name) {
    res_path.append(cocos2d::FileUtils::sharedFileUtils()->getWritablePath());

    res_path.append(getStringMD5Hash(name));
}

std::string getStringMD5Hash(const std::string &string) {
    cocos2d::Data data;
    data.fastSet((unsigned char *) string.c_str(), string.size());
    std::string md5Str = cocos2d::utils::getDataMD5Hash(data);
    data.fastSet(nullptr, NULL);
    return md5Str;
}


bool __touchBegin() {
    long long _current_ms = cocos2d::utils::getTimeInMilliseconds();
    if (_current_ms - _touch_tap_timestamp < _TOUCH_EVENT_TAP_GAP_MS) {
        return false;
    }

    _touch_down_timestamp = _current_ms;

    return true;
}

bool __isTapEvent() {
    long long _current_ms = cocos2d::utils::getTimeInMilliseconds();
    if (_current_ms - _touch_down_timestamp < _TOUCH_EVENT_TAP_VALID_MS) {
        _touch_tap_timestamp = _current_ms;
        return true;
    }
    return false;
}

cocos2d::GLProgramState *darkGLProgramState = nullptr;
cocos2d::GLProgramState* getDarkGLProgramState() {

    if (nullptr == darkGLProgramState) {
        auto _file_utils = cocos2d::FileUtils::getInstance();
        auto _frag_full_path = _file_utils->fullPathForFilename("cocos/shader/dark.fsh");
        auto _frag_content = _file_utils->getStringFromFile(_frag_full_path);
        auto _gl_program = cocos2d::GLProgram::createWithByteArrays(cocos2d::ccPositionTextureColor_noMVP_vert, _frag_content.c_str());
        darkGLProgramState = cocos2d::GLProgramState::getOrCreateWithGLProgram(_gl_program);
        darkGLProgramState->retain();
    }
    return darkGLProgramState;
}

cocos2d::GLProgramState *lightGLProgramState = nullptr;
cocos2d::GLProgramState* getLightGLProgramState() {

    if (nullptr == lightGLProgramState) {
        auto _file_utils = cocos2d::FileUtils::getInstance();
        auto _frag_full_path = _file_utils->fullPathForFilename("cocos/shader/light.fsh");
        auto _frag_content = _file_utils->getStringFromFile(_frag_full_path);
        auto _gl_program = cocos2d::GLProgram::createWithByteArrays(cocos2d::ccPositionTextureColor_noMVP_vert, _frag_content.c_str());
        lightGLProgramState = cocos2d::GLProgramState::getOrCreateWithGLProgram(_gl_program);
        lightGLProgramState->retain();
    }
    return lightGLProgramState;
}

void releaseGLProgramState() {
    CC_SAFE_RELEASE_NULL(darkGLProgramState);
    CC_SAFE_RELEASE_NULL(lightGLProgramState);
}