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
    if (nullptr == _picture) return;
    if (GifUtils::isGif(filename.c_str())) {
        _picture->init(filename.c_str());
    } else {
        _picture->setTexture(filename.c_str());
    }

    _picture->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _picture->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
    _picture->setScale(getMaxScale());
}

void CCGamePicture::initPicture() {
    this->_scale_factor = Director::getInstance()->getContentScaleFactor();
    setContentSize(Size(_CONTENT_SIZE_WIDTH_MAX, _CONTENT_SIZE_HEIGHT_MAX));
    setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);

    _picture = CacheGif::create("");
    if (nullptr == _picture) return;

    addChild(_picture);
    loadTexture(_skin.c_str());
}

float CCGamePicture::getMaxScale() {
    float _max_width = _CONTENT_SIZE_WIDTH_MAX / _scale_factor;
    float _max_height = _CONTENT_SIZE_HEIGHT_MAX / _scale_factor;
    if (_picture->getContentSize().width > _max_width  || _picture->getContentSize().height > _max_height) {
        float _width_ratio = _picture->getContentSize().width / _max_width;
        float _height_ratio = _picture->getContentSize().height / _max_height;
        return 1 / MAX(_width_ratio, _height_ratio);
    }
    return 1.0;
}

float CCGamePicture::getMinScale() {
    float _min_width = _CONTENT_SIZE_WIDTH_MIN / _scale_factor;
    float _min_height = _CONTENT_SIZE_HEIGHT_MIN / _scale_factor;
    if (getContentSize().width > _min_width  || getContentSize().height > _min_height) {
        float _width_ratio = getContentSize().width / _min_width;
        float _height_ratio = getContentSize().height / _min_height;
        return 1 / MAX(_width_ratio, _height_ratio);
    }
    return 1.0;
}

void CCGamePicture::setOpacity(GLubyte opacity) {
    Node::setOpacity(opacity);
    if (nullptr != _picture)
        _picture->setOpacity(opacity);
}
