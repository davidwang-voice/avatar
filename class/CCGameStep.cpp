//
// Created by David on 2020/10/21.
//

#include "CCGameStep.h"


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
    setTexture("anim/wave_ani_1.png");
    setAnchorPoint(Point::ANCHOR_MIDDLE);
}


void CCGameStep::runVoiceWave() {

    if (this->_mute || !isVisible()) return;

    auto _wave_action = getActionByTag(_TAG_WAVE_ANIM_ACTION);
    if (nullptr != _wave_action && !_wave_action->isDone()) {
        this->stopAction(_wave_action);
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
    runAction(_action);
}

void CCGameStep::setMute(bool mute) {
    auto _wave_action = getActionByTag(_TAG_WAVE_ANIM_ACTION);
    if (nullptr != _wave_action && !_wave_action->isDone()) {
        this->stopAction(_wave_action);
    }
    this->_mute = mute;
    setTexture(mute ? "voice_mute.png": "anim/wave_ani_1.png");
}

void CCGameStep::setUid(const char *uid) {
    this->_uid = uid;
    setVisible(!this->_uid.empty());
}

const char* CCGameStep::getUid() {
    return this->_uid.c_str();
}