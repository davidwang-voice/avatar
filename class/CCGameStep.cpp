//
// Created by David on 2020/10/21.
//

#include "CCGameStep.h"
#include "CCRoomDelegate.h"


CCGameStep *CCGameStep::create(int id, int ranking, std::string skin, int priority) {
    auto step = new (nothrow) CCGameStep(id, ranking, move(skin), priority);
    if (step && step->init()) {
        step->autorelease();
    } else {
        CC_SAFE_DELETE(step);
    }
    if (step)
        step->initStep();
    return step;
}


void CCGameStep::initStep() {
    float _scale_factor = Director::getInstance()->getContentScaleFactor();

    this->setContentSize(Size(_CONTENT_SIZE_WIDTH / _scale_factor, _CONTENT_SIZE_HEIGHT / _scale_factor));

//    setTexture("bg_stage_step.png");
    setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);



    auto _step_base = CCBaseSprite::create();
    _step_base->setTexture("cocos/step_base.png");
    _step_base->setAnchorPoint(Point::ANCHOR_MIDDLE_TOP);
    _step_base->setPosition(Vec2(getContentSize().width / 2, 100 / _scale_factor));
    this->addChild(_step_base, 1, _TAG_STEP_BASE);


    auto _step_place = Sprite::create();
    _step_place->setTexture("cocos/anim/wave_ani_1.png");
    _step_place->setAnchorPoint(Point::ANCHOR_MIDDLE);
    float _target_x = getContentSize().width / 2;
    float _target_y = 19 / _scale_factor;
    _step_place->setPosition(Vec2(_target_x, _target_y));

    this->addChild(_step_place, 1, _TAG_STEP_PLACE);

    auto _step_add = Sprite::create();
    _step_add->setTexture("cocos/step_add.png");
    _step_add->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    _step_add->setPosition(Vec2(getContentSize().width / 2, 70 / _scale_factor));
    this->addChild(_step_add, 1, _TAG_STEP_ADD);



    auto _step_label = Label::createWithSystemFont("连麦", "Arial", 24 / _scale_factor);
    _step_label->setOverflow(Label::Overflow::CLAMP);
    _step_label->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _step_label->setPosition(_step_place->getPosition());
    _step_label->setTextColor(Color4B(255,255,255,255));
    this->addChild(_step_label, 1, _TAG_STEP_LABEL);


    setUid("");
    setBase("");
    setMute(false);
}


void CCGameStep::runVoiceWave() {

    auto _child = this->getChildByTag(_TAG_STEP_PLACE);
    if (auto _step_place = dynamic_cast<Sprite *>(_child)) {
        if (this->_mute || !_step_place->isVisible()) return;

        auto _wave_action = _step_place->getActionByTag(_TAG_WAVE_ANIM_ACTION);
        if (nullptr != _wave_action && !_wave_action->isDone()) {
            _step_place->stopAction(_wave_action);
        }

        Animation *_animation = Animation::create();
        for (int i = 1; i <= 12; i++) {
            auto imagePath = "cocos/anim/wave_ani_" + std::to_string(i) + ".png";
            _animation->addSpriteFrameWithFileName(imagePath.c_str());
        }
        // should last 2.8 seconds. And there are 14 frames.
        _animation->setDelayPerUnit(0.040f);
        _animation->setRestoreOriginalFrame(false);
        Animate *_animate = Animate::create(_animation);
        auto _action = Sequence::create(_animate, _animate->reverse(), _animate, _animate->reverse(), NULL);

//    Repeat *_action = Repeat::create(_animate, 2);
        _action->setTag(_TAG_WAVE_ANIM_ACTION);
        _step_place->runAction(_action);
    }


}

void CCGameStep::setMute(bool mute) {
    auto _child = this->getChildByTag(_TAG_STEP_PLACE);
    if (auto _step_place = dynamic_cast<Sprite *>(_child)) {

        auto _wave_action = _step_place->getActionByTag(_TAG_WAVE_ANIM_ACTION);
        if (nullptr != _wave_action && !_wave_action->isDone()) {
            _step_place->stopAction(_wave_action);
        }
        this->_mute = mute;
        _step_place->setTexture(mute ? "cocos/voice_mute.png": "cocos/anim/wave_ani_1.png");
    }

}

void CCGameStep::setUid(const char *uid) {
    this->_uid = uid;
    if (auto _step_place = dynamic_cast<Sprite *>(this->getChildByTag(_TAG_STEP_PLACE))) {
        _step_place->setVisible(!this->_uid.empty());
    }
    if (auto _step_add = dynamic_cast<Sprite *>(this->getChildByTag(_TAG_STEP_ADD))) {
        _step_add->setVisible(this->_uid.empty());
    }
    if (auto _step_label = dynamic_cast<Label *>(this->getChildByTag(_TAG_STEP_LABEL))) {
        _step_label->setVisible(this->_uid.empty());
    }
}

const char* CCGameStep::getUid() {
    return this->_uid.c_str();
}

void CCGameStep::setBase(const char *base) {

    string _base_url(base);
    if (auto _step_base = dynamic_cast<CCBaseSprite *>(this->getChildByTag(_TAG_STEP_BASE))) {

        if (_base_url.empty()) {
            _step_base->setTexture("cocos/step_base.png");
        } else {
            _step_base->loadTexture(_base_url.c_str(), "cocos/step_base.png");
        }
    }
}

bool CCGameStep::onTouchBegan(Touch *touch, Event *event) {
    if (auto _step = dynamic_cast<Sprite*>(event->getCurrentTarget())) {
        Point _locationInNode = touch->getLocation();
        auto _pos = _step->getParent()->convertToNodeSpace(_locationInNode);
        Rect _rect = _step->getBoundingBox();
        if (_rect.containsPoint(_pos)) {

            if (!__touchBegin()) return false;

            if (this->_uid.empty()) {

                log("Step add onTouchesBegan... index = %d, uid = %s", this->_id, this->getUid());
                return true;
            }
        }
    }

    return false;
}

void CCGameStep::onTouchMoved(Touch *touch, Event *event) {

}

void CCGameStep::onTouchEnded(Touch *touch, Event *event) {
    if (!__isTapEvent()) return;
    if (auto _step = dynamic_cast<Sprite*>(event->getCurrentTarget())) {
        Point _locationInNode = touch->getLocation();
        auto _pos = _step->getParent()->convertToNodeSpace(_locationInNode);
        Rect _rect = _step->getBoundingBox();
        if (_rect.containsPoint(_pos)) {
            log("Step add onTouchesEnd... index = %d, uid = %s", this->_id, this->getUid());
            onTouchStageAvatar(this->getUid());
        }
    }
}

void CCGameStep::bindTargetNode(EventListener *listener) {
    if (auto _step_add = dynamic_cast<Sprite *>(this->getChildByTag(_TAG_STEP_ADD))) {
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _step_add);
    }
}
