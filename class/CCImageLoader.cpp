//
// Created by David on 2020/12/28.
//

#include "CCImageLoader.h"
#include "CCBaseSprite.h"

CCImageLoader::CCImageLoader(void)
{
    _imageSprite = nullptr;
    HttpClient::getInstance()->setTimeoutForConnect(5);
}

CCImageLoader::~CCImageLoader(void)
{
}

void CCImageLoader::sendRequest(Sprite* sprite, string url, string tag)
{
    if (nullptr == sprite || url.empty() || tag.empty()) {
        return;
    }
    HttpRequest* _request = new (std::nothrow) HttpRequest();
    _request->setRequestType(HttpRequest::Type::GET);
    _request->setUrl(url.c_str());
    _request->setResponseCallback(CC_CALLBACK_2(CCImageLoader::onHttpRequestCompleted, this));
    _request->setTag(tag.c_str());
    HttpClient::getInstance()->sendImmediate(_request);
    _request->release();

    _imageSprite = sprite;
    CC_SAFE_RETAIN(sprite);
}


void CCImageLoader::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) {
    auto _sprite = dynamic_cast<CCBaseSprite *>(_imageSprite);

    if (response) {
        const char* _tag = response->getHttpRequest()->getTag();
        if (response->isSucceed() && response->getResponseCode() == 200) {
            std::string _file_path("");
            try {
                getGameResourcePath(_file_path, _tag);

                vector<char> *_buffer = response->getResponseData();

                std::string _buff(_buffer->begin(),_buffer->end());

                FILE *fp = fopen(_file_path.c_str(), "wb+");
                fwrite(_buff.c_str(), 1,_buffer->size(),  fp);
                fclose(fp);

                log("request completed - resource file path: %s", _file_path.c_str());
                if (_sprite && nullptr != _sprite->getParent()) {
                    _sprite->setTexture(_file_path);
                } else {
                    log("request completed - sprite is removed already!");
                }

                _sprite->isRequestRetry = false;
            }
            catch(...) {
                log("request completed - save to local error: %s", _file_path.c_str());
            }
        } else {
            log("request completed - Error buffer: %s", response->getErrorBuffer());
            _sprite->isRequestRetry = true;
        }

    } else {
        log("request completed - No Response");
        _sprite->isRequestRetry = true;
    }
    CC_SAFE_RELEASE(_sprite);
    if (_sprite) {
        log("request completed - sprite ref count:%d", _sprite->getReferenceCount());
    }
}