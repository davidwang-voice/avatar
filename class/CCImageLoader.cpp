//
// Created by David on 2020/12/28.
//

#include "CCImageLoader.h"
#include "CCBaseSprite.h"


static CCImageLoader *instance = NULL;


CCImageLoader *CCImageLoader::getInstance() {
    if (!instance) {
        instance = new CCImageLoader();
    }
    return instance;
}

void CCImageLoader::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(instance);
}


CCImageLoader::CCImageLoader(void)
{
    HttpClient::getInstance()->setTimeoutForConnect(20);
    HttpClient::getInstance()->setTimeoutForRead(60);
}

CCImageLoader::~CCImageLoader(void)
{
    map<std::string, Vector<Sprite*>>::iterator _iterator;
    for(_iterator = _imageMap.begin(); _iterator != _imageMap.end(); _iterator++) {
        _iterator->second.clear();
    }
    _imageMap.clear();

}

void CCImageLoader::sendRequest(Sprite* sprite, string url)
{
    if (nullptr == sprite || url.empty()) {
        return;
    }

    map<std::string, Vector<Sprite*>>::iterator _iterator;
    _iterator = _imageMap.find(url.c_str());
    if (_iterator == _imageMap.end())  {
        _imageMap[url];
        _imageMap[url].pushBack(sprite);

        HttpRequest* _request = new (std::nothrow) HttpRequest();
        _request->setRequestType(HttpRequest::Type::GET);
        _request->setUrl(url.c_str());
        _request->setResponseCallback(CC_CALLBACK_2(CCImageLoader::onHttpRequestCompleted, this));
        _request->setTag(url.c_str());
        HttpClient::getInstance()->sendImmediate(_request);
        _request->release();
    } else {
        if (!_imageMap[url].contains(sprite)) {
            _imageMap[url].pushBack(sprite);
        }
    }

    log("request send map size: %d", _imageMap.size());
}


void CCImageLoader::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) {
    if (response) {
        const char* _tag = response->getHttpRequest()->getTag();
        map<std::string, Vector<Sprite*>>::iterator _iterator;
        _iterator = _imageMap.find(_tag);
        if (_iterator == _imageMap.end())  {
            return;
        }
        Vector<Sprite*>& _images = _imageMap[_tag];
        bool isRequestRetry = false;


        if (response->isSucceed() && response->getResponseCode() == 200) {
            std::string _file_path("");
            getGameResourcePath(_file_path, _tag);

            long long _current_ms = cocos2d::utils::getTimeInMilliseconds();
            std::string _download(_file_path);
            _download.append(std::to_string(_current_ms));

            try {

                vector<char> *_buffer = response->getResponseData();
                std::string _buff(_buffer->begin(),_buffer->end());

                FILE *fp = fopen(_download.c_str(), "wb+");
                fwrite(_buff.c_str(), 1,_buffer->size(),  fp);
                fclose(fp);


//                if (!FileUtils::sharedFileUtils()->isFileExist(_file_path)) {
//
//                }

                std::rename(_download.c_str(), _file_path.c_str());


                log("request completed - resource file path: %s", _file_path.c_str());

                for (int i = 0; i < _images.size(); ++i) {
                    auto _sprite = _images.at(i);
                    if (_sprite && nullptr != _sprite->getParent()) {
                        _sprite->setTexture(_file_path);
                    } else {
                        log("request completed - sprite is removed already!");
                    }
                }

                isRequestRetry = false;
            }
            catch(...) {

                log("request completed - save to local error: %s", _file_path.c_str());
            }
            std::remove(_download.c_str());
        } else {
            log("request completed - Error buffer: %s", response->getErrorBuffer());
            isRequestRetry = true;
        }

        releaseImageWithRetry(_tag, isRequestRetry);
    } else {
        log("request completed - No Response");
    }
}


void CCImageLoader::releaseImageWithRetry(const char* url, bool retry) {
    map<std::string, Vector<Sprite*>>::iterator _iterator;
    _iterator = _imageMap.find(url);
    if (_iterator == _imageMap.end())  {
        return;
    }
    Vector<Sprite*> &_images = _imageMap[url];
    for (int i = 0; i < _images.size(); ++i) {
        if (auto _base_sprite = dynamic_cast<CCBaseSprite*>(_images.at(i))) {
            _base_sprite->isRequestRetry = retry;
        }
    }
    _images.clear();
    _imageMap.erase(url);

//    if (_sprite) {
//        log("request completed - sprite ref count:%d", _sprite->getReferenceCount());
//    }
}
