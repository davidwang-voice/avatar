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
//    setTexture(_skin);
    setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
}


void CCGameStep::runVoiceWave() {

    log("runVoiceWave uid: %s", _uid.c_str());
    Animation *_anim = Animation::create();
    for(int i=1; i<=21;i++) {
        auto imagePath = "anim/audio_ani_" + std::to_string(i) + ".png";
        _anim->addSpriteFrameWithFileName(imagePath.c_str());
    }
    // should last 2.8 seconds. And there are 14 frames.
    _anim->setDelayPerUnit(1.0f / 24.0f);
    _anim->setRestoreOriginalFrame(true);
    Animate* _action = Animate::create(_anim);

    runAction(Sequence::create(_action, _action->reverse(), _action, _action->reverse(), NULL));
}

void CCGameStep::setUid(const char *uid) {
    this->_uid = uid;
}

const char* CCGameStep::getUid() {
    return this->_uid.c_str();
}