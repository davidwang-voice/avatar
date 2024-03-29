//
// Created by David on 2020/12/28.
//

#ifndef PROJ_ANDROID_CCIMAGELOADER_H
#define PROJ_ANDROID_CCIMAGELOADER_H

#include "cocos2d.h"
#include "Utils.h"
#include "network/HttpClient.h"

using namespace std;
using namespace cocos2d;
using namespace cocos2d::network;


class CCImageLoader: public Ref {

public:
    CCImageLoader(void);
    ~CCImageLoader(void);
//    CC_SYNTHESIZE(Sprite*, _imageSprite, ImageSprite);

    void sendRequest(Sprite* sprite, string url);

    // Static Methods
    static CCImageLoader *getInstance();
    static void destroyInstance();

private:
    map<std::string, Vector<Sprite*>> _imageMap;
    void releaseImageWithRetry(const char* url, bool retry);
    void onHttpRequestCompleted(HttpClient* sender,HttpResponse* response);
};

#endif //PROJ_ANDROID_CCIMAGELOADER_H
