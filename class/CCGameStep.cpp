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
    this->_mute = false;
    setTexture("bg_stage_step.png");
    setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);

    float _scale_factor = Director::getInstance()->getContentScaleFactor();

    auto _step_place = CCBaseSprite::create();
    _step_place->setTexture("anim/wave_ani_1.png");
    _step_place->setAnchorPoint(Point::ANCHOR_MIDDLE);
    float _target_x = getContentSize().width / 2;
    float _target_y = 18 / _scale_factor;
    _step_place->setPosition(Vec2(_target_x, _target_y));

    this->addChild(_step_place, 1, _TAG_STEP_PLACE);

    setUid("");
    setMute(false);
}


void CCGameStep::runVoiceWave() {

    auto _child = this->getChildByTag(_TAG_STEP_PLACE);
    if (auto _step_place = dynamic_cast<CCBaseSprite *>(_child)) {
        if (this->_mute || !_step_place->isVisible()) return;

        auto _wave_action = _step_place->getActionByTag(_TAG_WAVE_ANIM_ACTION);
        if (nullptr != _wave_action && !_wave_action->isDone()) {
            _step_place->stopAction(_wave_action);
        }

        Animation *_animation = Animation::create();
        for (int i = 1; i <= 12; i++) {
            auto imagePath = "anim/wave_ani_" + std::to_string(i) + ".png";
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
    if (auto _step_place = dynamic_cast<CCBaseSprite *>(_child)) {

        auto _wave_action = _step_place->getActionByTag(_TAG_WAVE_ANIM_ACTION);
        if (nullptr != _wave_action && !_wave_action->isDone()) {
            _step_place->stopAction(_wave_action);
        }
        this->_mute = mute;
        _step_place->setTexture(mute ? "voice_mute.png": "anim/wave_ani_1.png");
    }

}

void CCGameStep::setUid(const char *uid) {
    this->_uid = uid;
    auto _child = this->getChildByTag(_TAG_STEP_PLACE);
    if (auto _step_place = dynamic_cast<CCBaseSprite *>(_child)) {
        _step_place->setVisible(!this->_uid.empty());
    }

}

const char* CCGameStep::getUid() {
    return this->_uid.c_str();
}

bool CCGameStep::onTouchBegan(Touch *touch, Event *event) {
    if (auto _step = dynamic_cast<CCGameStep*>(event->getCurrentTarget())) {
        Point _locationInNode = touch->getLocation();
        auto _pos = _step->getParent()->convertToNodeSpace(_locationInNode);
        Rect _rect = _step->getBoundingBox();
        if (_rect.containsPoint(_pos)) {
            log("Step onTouchesBegan... index = %d, uid = %s", _step->_id, _step->getUid());

            return true;
        }
    }

    return false;
}

void CCGameStep::onTouchMoved(Touch *touch, Event *event) {

}

void CCGameStep::onTouchEnded(Touch *touch, Event *event) {
    if (auto _step = dynamic_cast<CCGameStep*>(event->getCurrentTarget())) {
        Point _locationInNode = touch->getLocation();
        auto _pos = _step->getParent()->convertToNodeSpace(_locationInNode);
        Rect _rect = _step->getBoundingBox();
        if (_rect.containsPoint(_pos)) {
            log("Step onTouchesEnd... index = %d, uid = %s", _step->_id, _step->getUid());
            onTouchStageAvatar(_step->getUid());
        }
    }
}