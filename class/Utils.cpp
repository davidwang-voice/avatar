#include "Utils.h"

#include "base/ccUtils.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
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
#endif

void getGameResourceUrl(std::string &res_url, const char* name) {
//    res_url.append(_CC_GAME_FILE_HTTP_PATH);

    res_url.append(name);

}

void getGameResourcePath(std::string &res_path, const char* name) {
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



