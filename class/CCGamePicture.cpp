//
// Created by David on 2021/4/19.
//

#include "CCGamePicture.h"

CCGamePicture *CCGamePicture::create(int id, int ranking, string skin, int priority) {
    auto gift = new (nothrow) CCGamePicture(id, ranking, move(skin), priority);
    if (gift && gift->init()) {
        gift->autorelease();
    } else {
        CC_SAFE_DELETE(gift);
    }
    if (gift)
        gift->initPicture();
    return gift;
}

void CCGamePicture::setTexture(const std::string &filename) {

    if (GifUtils::isGif(filename.c_str())) {
        if (nullptr == _picture_gif) return;
        _picture_gif->init(filename.c_str());
        _picture_gif->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _picture_gif->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
        _picture_gif->setScale(getMaxScale(_picture_gif));
    } else {
        if (nullptr == _picture_png) return;
        _picture_png->setTexture(filename.c_str());
        _picture_png->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _picture_png->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
        _picture_png->setScale(getMaxScale(_picture_png));
    }

}

void CCGamePicture::initPicture() {
    this->_scale_factor = Director::getInstance()->getContentScaleFactor();
    setContentSize(Size(_CONTENT_SIZE_WIDTH_MAX, _CONTENT_SIZE_HEIGHT_MAX));
    setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);

    _picture_gif = CacheGif::create("");
    if (nullptr != _picture_gif) {
        addChild(_picture_gif);
    }
    _picture_png = Sprite::create();
    addChild(_picture_png);

    if (startWith(_skin, "http")) {
        loadTexture(_skin.c_str());
    } else {
        setTexture(_skin);
    }
}

float CCGamePicture::getMaxScale(Node* node) {
    float _max_width = _CONTENT_SIZE_WIDTH_MAX / _scale_factor;
    float _max_height = _CONTENT_SIZE_HEIGHT_MAX / _scale_factor;
    if (node->getContentSize().width > _max_width  || node->getContentSize().height > _max_height) {
        float _width_ratio = node->getContentSize().width / _max_width;
        float _height_ratio = node->getContentSize().height / _max_height;
        return 1 / MAX(_width_ratio, _height_ratio);
    }
    return 1.0;
}

void CCGamePicture::setOpacity(GLubyte opacity) {
    Node::setOpacity(opacity);
    if (nullptr != _picture_gif)
        _picture_gif->setOpacity(opacity);
    if (nullptr != _picture_png)
        _picture_png->setOpacity(opacity);
}


bool CCGamePicture::startWith(const std::string& str, const std::string& start)
{
    int srclen = str.size();
    int startlen = start.size();
    if(srclen >= startlen)
    {
        string temp = str.substr(0, startlen);
        if(temp == start)
            return true;
    }
    return false;
}
