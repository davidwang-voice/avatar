//
// Created by Rayan on 2019/2/15.
//

#include "CCBaseSprite.h"

CCBaseSprite *CCBaseSprite::create(int priority, int id, int ranking) {
    auto avatar = new (std::nothrow) CCBaseSprite(priority, id, ranking);
    if (avatar && avatar->init()) {
        avatar->autorelease();
    } else {
        CC_SAFE_DELETE(avatar);
    }
    return avatar;
}

void CCBaseSprite::onEnter() {
    Sprite::onEnter();

    auto listener = EventListenerTouchOneByOne::create();

    listener->setSwallowTouches(true);

    listener->onTouchBegan = CC_CALLBACK_2(CCBaseSprite::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(CCBaseSprite::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(CCBaseSprite::onTouchEnded, this);

//    if (_fixedPriority != 0) {
//        _eventDispatcher->addEventListenerWithFixedPriority(listener, _ranking + 1);
//    } else {
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
//    }

    _listener = listener;
}

void CCBaseSprite::onExit() {
    _eventDispatcher->removeEventListener(_listener);

    Sprite::onExit();
}

int CCBaseSprite::getId() {
    return _id;
}

int CCBaseSprite::getRanking() {
    return _ranking;
}

void CCBaseSprite::updateRanking(int ranking) {
    _ranking = ranking;
}

bool CCBaseSprite::onTouchBegan(Touch* touch, Event  *event) {
    return false;
}
void CCBaseSprite::onTouchMoved(Touch* touch, Event  *event) {

}
void CCBaseSprite::onTouchEnded(Touch* touch, Event  *event) {

}



void CCBaseSprite::loadTexture(const char *name, const char *def) {

    std::string _file_path("");
    getGameResourcePath(_file_path, name);

    log("request start - resource _file_path: %s", _file_path.c_str());
    if (FileUtils::sharedFileUtils()->isFileExist(_file_path)) {
        setTexture(_file_path);
    } else {
        if (nullptr != def) {
            setTexture(def);
        }

        std::string _file_url("");
        getGameResourceUrl(_file_url, name);

        sendResourceRequest(_file_url.c_str(), name);
    }
}

void CCBaseSprite::sendResourceRequest(const char *url, const char *tag) {
    log("request start - resource url: %s , tag: %s", url, tag);
    HttpRequest* _request = new (std::nothrow) HttpRequest();
    _request->setUrl(url);
    _request->setRequestType(HttpRequest::Type::GET);
    _request->setResponseCallback(CC_CALLBACK_2(CCBaseSprite::onRequestCompleted, this));
    _request->setTag(tag);
    HttpClient::getInstance()->setTimeoutForConnect(30);
    HttpClient::getInstance()->sendImmediate(_request);
    _request->release();
}

void CCBaseSprite::onRequestCompleted(HttpClient *sender, HttpResponse *response) {
    if (!response) {
        log("request completed - No Response");
        return;
    }

    const char* _tag = response->getHttpRequest()->getTag();
    log("request completed - Response code: %lu, succeed: %d, tag: %s",
            response->getResponseCode(), (response->isSucceed() ? 1 : 0), _tag);

    if (!response->isSucceed() || response->getResponseCode() != 200) {
        log("request completed - Error buffer: %s", response->getErrorBuffer());
        return;
    }



//    std::vector<char> *buffer = response->getResponseData();
//    Image *image = new Image();
//    image->initWithImageData((unsigned char*)buffer->data(), buffer->size());
//    Texture2D *texture = new Texture2D();
//    texture->initWithImage(image);
//
//    image->release();
//    setTexture(texture);

    std::string _file_path("");
    getGameResourcePath(_file_path, _tag);

    try {
        vector<char> *_buffer = response->getResponseData();

        std::string _buff(_buffer->begin(),_buffer->end());

        FILE *fp = fopen(_file_path.c_str(), "wb+");
        fwrite(_buff.c_str(), 1,_buffer->size(),  fp);
        fclose(fp);

        log("request completed - resource file path: %s", _file_path.c_str());

        setTexture(_file_path);
    }
    catch(...) {
        log("request completed - save to local error: %s", _file_path.c_str());
    }
}