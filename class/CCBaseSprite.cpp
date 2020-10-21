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

    auto listener = EventListenerTouchAllAtOnce::create();

    listener->onTouchesBegan = CC_CALLBACK_2(CCBaseSprite::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(CCBaseSprite::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(CCBaseSprite::onTouchesEnded, this);

    if (_fixedPriority != 0) {
        _eventDispatcher->addEventListenerWithFixedPriority(listener->clone(), _fixedPriority);
    } else {
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }

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

void CCBaseSprite::onTouchesBegan(const std::vector<Touch*>& touches, Event *event) {
}

void CCBaseSprite::onTouchesMoved(const std::vector<Touch*>& touches, Event *event) {
    // do nothing at present
}

void CCBaseSprite::onTouchesEnded(const std::vector<Touch*>& touches, Event *event) {
}


void CCBaseSprite::loadTexture(const char *name, const char *def) {

    const char* _file_path = getGameResourcePath(name);
    const char* _file_url = getGameResourceUrl(name);

    log("loadTexture request start , _file_url: %s", _file_url);

    if (FileUtils::sharedFileUtils()->isFileExist(_file_path)) {
        setTexture(_file_path);
    } else {

        log("loadTexture- send request start , _file_url: %s", _file_url);
        sendResourceRequest(_file_url, name);
        if (nullptr != def) {
            setTexture(def);
        }
    }
}

void CCBaseSprite::sendResourceRequest(const char *url, const char *tag) {
    log("request start, image url: %s , tag: %s", url, tag);
    HttpRequest* _request = new (std::nothrow) HttpRequest();
    _request->setUrl(url);
    _request->setRequestType(HttpRequest::Type::GET);
    _request->setResponseCallback(CC_CALLBACK_2(CCBaseSprite::onRequestCompleted, this));
    _request->setTag(tag);
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

    vector<char> *_buffer = response->getResponseData();
    string _file_path = getGameResourcePath(_tag);
    string _buff(_buffer->begin(),_buffer->end());

    FILE *fp = fopen(_file_path.c_str(), "wb+");
    fwrite(_buff.c_str(), 1,_buffer->size(),  fp);
    fclose(fp);

    log("request completed - resource file path: %s",_file_path.c_str());

    setTexture(_file_path);

}
