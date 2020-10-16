//
// Created by Rayan on 2019/2/15.
//

#include "BaseSprite.h"
#include "RoomAvatar.h"

BaseSprite *BaseSprite::create(int priority, int id, int ranking) {
    auto avatar = new (std::nothrow) BaseSprite(priority, id, ranking);
    if (avatar && avatar->init()) {
        avatar->autorelease();
    } else {
        CC_SAFE_DELETE(avatar);
    }
    return avatar;
}

void BaseSprite::onEnter() {
    Sprite::onEnter();

    auto listener = EventListenerTouchAllAtOnce::create();

    listener->onTouchesBegan = CC_CALLBACK_2(BaseSprite::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(BaseSprite::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(BaseSprite::onTouchesEnded, this);

    if (_fixedPriority != 0) {
        _eventDispatcher->addEventListenerWithFixedPriority(listener->clone(), _fixedPriority);
    } else {
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }

    _listener = listener;
}

void BaseSprite::onExit() {
    _eventDispatcher->removeEventListener(_listener);

    Sprite::onExit();
}

int BaseSprite::getId() {
    return _id;
}

int BaseSprite::getRanking() {
    return _ranking;
}

void BaseSprite::updateRanking(int ranking) {
    _ranking = ranking;
}

void BaseSprite::onTouchesBegan(const std::vector<Touch*>& touches, Event *event) {
    if(touches.size() == 1) {
        if (auto avatar = dynamic_cast<RoomAvatar*>(event->getCurrentTarget())) {
            Point locationInNode = touches[0]->getLocation();
            auto pos = avatar->getParent()->convertToNodeSpace(locationInNode);
            Rect rect = avatar->getBoundingBox();
            if (rect.containsPoint(pos)) {
                log("Avatar onTouchesBegan... id = %d", avatar->_id);
            }
        }
    }
}

void BaseSprite::onTouchesMoved(const std::vector<Touch*>& touches, Event *event) {
    // do nothing at present
}

void BaseSprite::onTouchesEnded(const std::vector<Touch*>& touches, Event *event) {
    if(touches.size() == 1) {
        if (auto avatar = dynamic_cast<RoomAvatar*>(event->getCurrentTarget())) {
            Point locationInNode = touches[0]->getLocation();
            auto pos = avatar->getParent()->convertToNodeSpace(locationInNode);
            Rect rect = avatar->getBoundingBox();
            if (rect.containsPoint(pos)) {
                log("Avatar onTouchesEnded... id = %d", avatar->_id);
                onTouchedAvatar(avatar->_id);
            }
        }
    }
}
