#include "Utils.h"

char* jstringToChar(JNIEnv* env, jstring jstr) {
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char*) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}


const char* getGameResourceUrl(const char* name) {
    std::string _res_url("");
    _res_url.append(_CC_GAME_FILE_HTTP_PATH);
    _res_url.append(name);
    return _res_url.c_str();
}

const char* getGameResourcePath(const char* name) {
    std::string _res_path("");
    _res_path.append(cocos2d::FileUtils::sharedFileUtils()->getWritablePath());
    _res_path.append(name);
    return _res_path.c_str();
}