//
// Created by David on 2020/10/3.
//



#include <utility>
#include "CCGameAvatar.h"
#include <iomanip>

USING_NS_CC;

using namespace cocos2d::network;


CCGameAvatar *CCGameAvatar::create(int id, int ranking, string uid, string skin, string name, int priority) {
    auto avatar = new (nothrow) CCGameAvatar(id, ranking, uid, move(skin), move(name), priority);
    if (avatar && avatar->init()) {
        avatar->autorelease();
    } else {
        CC_SAFE_DELETE(avatar);
    }
    if (avatar)
        avatar->initAvatar();
    return avatar;
}

void CCGameAvatar::initAvatar() {

    // set the avatar skin and other params

    //assets/avatar/default_avatar.png
    loadTexture(_skin.c_str(), "avatar/default_avatar.png");

    setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);

    float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 1.5;
    auto delay = DelayTime::create(random);
    auto callback = CallFuncN::create(CC_CALLBACK_0(CCGameAvatar::shakingBody, this));
    auto seq = Sequence::create(delay, callback, nullptr);
    runAction(seq);

    TTFConfig labelConfig;
    labelConfig.fontFilePath = "font/droid.ttf";
    labelConfig.fontSize = 16;
    labelConfig.glyphs = GlyphCollection::DYNAMIC;
    labelConfig.outlineSize = 0;
    labelConfig.customGlyphs = nullptr;
    labelConfig.distanceFieldEnabled = false;
    auto nameLabel = Label::createWithTTF(labelConfig, _name);
    nameLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    nameLabel->setPosition(getContentSize().width / 2, getContentSize().height);




    Color4B color4B = Color4B(0, 0, 0, 100);

    auto bgLayer = LayerColor::create(color4B, nameLabel->getContentSize().width+10, nameLabel->getContentSize().height);
    bgLayer->setPosition(Vec2(getPosition().x + (getContentSize().width - bgLayer->getContentSize().width)/2, getPosition().y+getContentSize().height));
    addChild(bgLayer);

    addChild(nameLabel);


    _loaded = false;

}

void CCGameAvatar::onEnter() {
    CCBaseSprite::onEnter();
    if (auto listener = dynamic_cast<EventListenerTouchOneByOne*>(_listener)) {
//        listener->setSwallowTouches(true);
    }


}


void CCGameAvatar::shakingBody() {
    auto _rotateRight = RotateTo::create(0.9f, 6.0f);
    auto _rotateLeft = RotateTo::create(0.9f, -6.0f);
    auto _action = Sequence::create(_rotateRight, _rotateLeft, nullptr);
    runAction(RepeatForever::create(_action));
}

void CCGameAvatar::setPosition(const Vec2 &position) {

    Sprite::setPosition(position);
//    log("sprite x: %f, y: %f", getPosition().x, getPosition().y);
}


void CCGameAvatar::onTouchesBegan(const vector<Touch *> &touches, Event *event) {
    CCBaseSprite::onTouchesBegan(touches, event);
    if(touches.size() == 1) {
        if (auto _avatar = dynamic_cast<CCGameAvatar*>(event->getCurrentTarget())) {
            Point _locationInNode = touches[0]->getLocation();
            auto _pos = _avatar->getParent()->convertToNodeSpace(_locationInNode);
            Rect _rect = _avatar->getBoundingBox();
            if (_rect.containsPoint(_pos)) {
                log("Avatar onTouchesBegan... index = %d, uid = %s", _avatar->_id, _avatar->getUid());
            }
        }
    }
}

void CCGameAvatar::onTouchesMoved(const vector<Touch *> &touches, Event *event) {
    CCBaseSprite::onTouchesMoved(touches, event);
}

void CCGameAvatar::onTouchesEnded(const vector<Touch *> &touches, Event *event) {
    CCBaseSprite::onTouchesEnded(touches, event);

    if(touches.size() == 1) {
        if (auto _avatar = dynamic_cast<CCGameAvatar*>(event->getCurrentTarget())) {
            Point _locationInNode = touches[0]->getLocation();
            auto _pos = _avatar->getParent()->convertToNodeSpace(_locationInNode);
            Rect _rect = _avatar->getBoundingBox();
            if (_rect.containsPoint(_pos)) {
                log("Avatar onTouchesEnd... index = %d, uid = %s", _avatar->_id, _avatar->getUid());
                onTouchedAvatar(_avatar->getUid());
            }
        }
    }
}

bool CCGameAvatar::onTouchBegan(Touch *touch, Event *event) {
    log("Avatar onTouchesBegan... name = %s", typeid(event->getCurrentTarget()).name());
    if (auto _avatar = dynamic_cast<CCGameAvatar*>(event->getCurrentTarget())) {
        Point _locationInNode = touch->getLocation();
        auto _pos = _avatar->getParent()->convertToNodeSpace(_locationInNode);
        Rect _rect = _avatar->getBoundingBox();
        if (_rect.containsPoint(_pos)) {
            log("Avatar onTouchesBegan... index = %d, uid = %s", _avatar->_id, _avatar->getUid());
        }
    }
    return true;
}

void CCGameAvatar::onTouchMoved(Touch *touch, Event *event) {

}

void CCGameAvatar::onTouchEnded(Touch *touch, Event *event) {
    if (auto _avatar = dynamic_cast<CCGameAvatar*>(event->getCurrentTarget())) {
        Point _locationInNode = touch->getLocation();
        auto _pos = _avatar->getParent()->convertToNodeSpace(_locationInNode);
        Rect _rect = _avatar->getBoundingBox();
        if (_rect.containsPoint(_pos)) {
            log("Avatar onTouchesEnd... index = %d, uid = %s", _avatar->_id, _avatar->getUid());
            onTouchedAvatar(_avatar->getUid());
        }
    }
}



void CCGameAvatar::updateStatus(bool mute, bool ssr) {


}


const char* CCGameAvatar::getUid() {
    return _uid.c_str();
}

void CCGameAvatar::jumpToPosition(const Vec2 &target) {

    auto _cur_pos = this->getPosition();
    if (_cur_pos.equals(target)) {
        // already at target position
        log("already at target position  %f, %f", target.x, target.y);
        return;
    }

    auto _move_action = getActionByTag(_TAG_MOVE_TO_ACTION);
    if (nullptr != _move_action && !_move_action->isDone()) {
        this->stopAction(_move_action);
    }

    auto _jump_action = getActionByTag(_TAG_JUMP_TO_ACTION);
    if (nullptr != _jump_action && !_jump_action->isDone()) {
        this->stopAction(_jump_action);
    }

    auto _callback = CallFunc::create([this, target](){
        this->setPosition(target);
    });

    if (_loaded) {
        auto _jumpTo = JumpTo::create(0.5, target, 80, 1);
        auto _action = Sequence::create(_jumpTo, _callback, nullptr);
        _action->setTag(_TAG_JUMP_TO_ACTION);
        this->runAction(_action);

    } else {
        auto _moveTo = MoveTo::create(0.3, target);
        auto _action = Sequence::create(_moveTo, _callback, nullptr);
        _action->setTag(_TAG_MOVE_TO_ACTION);
        this->runAction(_action);
    }
    _loaded = true;
}

void CCGameAvatar::jumpByPresent() {

    auto _jump_action = getActionByTag(_TAG_JUMP_BY_ACTION);
    if (nullptr == _jump_action || _jump_action->isDone()) {
        auto _jump_action = JumpBy::create(0.5f, Vec2(0, 0), 60, 1);
        _jump_action->setTag(_TAG_JUMP_BY_ACTION);
        runAction(_jump_action);
    }
}
void CCGameAvatar::popChatBubble(const char* content) {

    TTFConfig _label_config;
    _label_config.fontFilePath = "font/droid.ttf";
    _label_config.fontSize = 25;
    _label_config.glyphs = GlyphCollection::DYNAMIC;
    _label_config.outlineSize = 0;
    _label_config.customGlyphs = nullptr;
    _label_config.distanceFieldEnabled = false;

    auto _chat_label = Label::createWithTTF(_label_config, content);
//    label->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    _chat_label->setPosition(10 + _chat_label->getContentSize().width / 2 , 10 + _chat_label->getContentSize().height / 2 );
    Color4B  textColor(116,116,124,255);
    _chat_label->setTextColor(textColor);



    auto _chat_bubble = CCBaseSprite::create();
    _chat_bubble->setPosition(getContentSize().width/ 2, getContentSize().height * 2 - 40);;
    _chat_bubble->setContentSize(Size(_chat_label->getContentSize().width + 20, _chat_label->getContentSize().height + 20));


    auto _drawNode = DrawNode::create();
    const Vec2 &origin = Vec2(0,0);
    const Vec2 &destination = Vec2(_chat_bubble->getContentSize().width, _chat_bubble->getContentSize().height);
    Color4F color4F(1, 1, 1, 1);

    drawRoundRect(_drawNode, origin, destination,10, 88,color4F);

    _chat_bubble->addChild(_drawNode);
    _chat_bubble->addChild(_chat_label);
    this->addChild(_chat_bubble, 1001);

    auto _moveBy = MoveBy::create(2, Vec2(0, 30));

    auto _fadeFunc = CallFunc::create([_chat_label, _drawNode](){
        _chat_label->runAction(FadeOut::create(2));
        _drawNode->runAction(FadeOut::create(2));
    });

    auto _removeFunc = CallFunc::create([_chat_bubble](){
        _chat_bubble->removeFromParentAndCleanup(true);
    });

    auto _action = Sequence::create(_moveBy, _fadeFunc, DelayTime::create(2), _removeFunc, nullptr);
    _chat_bubble->runAction(_action);
}

void CCGameAvatar::drawRoundRect(DrawNode *drawNode, const Vec2 &origin, const Vec2 &destination,
                                 float radius, unsigned int segments, const Color4F &color)
{
    const float coef = 2.0f * (float)M_PI/(segments - 8);

    Vec2 *vertices = new (std::nothrow) Vec2[segments];
    if( ! vertices )
        return;

    // Draw right edge
    vertices[0].x = destination.x;
    vertices[0].y = origin.y + radius;
    vertices[1].x = destination.x;
    vertices[1].y = destination.y - radius;

    unsigned int quadrant = 1;
    unsigned int radsI = 0;
    for(unsigned int i = 2; i <= segments; i++) {
        float rads = radsI*coef;
        GLfloat j = radius * cosf(rads);
        GLfloat k = radius * sinf(rads);

        if (rads < M_PI_2 || rads > M_PI + M_PI_2) {
            if (quadrant == 3) {
                // Draw bottom edge
                vertices[i].x = origin.x + radius;
                vertices[i].y = origin.y;
                i++;
                vertices[i].x = destination.x - radius;
                vertices[i].y = origin.y;

                quadrant++;
                continue;
            }
            j += destination.x - radius;
        } else {
            if (quadrant == 1) {
                // Draw top edge
                vertices[i].x = destination.x - radius;
                vertices[i].y = destination.y;
                i++;
                vertices[i].x = origin.x + radius;
                vertices[i].y = destination.y;

                quadrant++;
                continue;
            }
            j += origin.x + radius;
        }

        if (rads < M_PI) {
            k += destination.y - radius;
        } else {
            if (quadrant == 2) {
                // Draw left edge
                vertices[i].x = origin.x;
                vertices[i].y = destination.y - radius;
                i++;
                vertices[i].x = origin.x;
                vertices[i].y = origin.y + radius;

                quadrant++;
                continue;
            }
            k += origin.y + radius;
        }

        vertices[i].x = j;
        vertices[i].y = k;

        radsI++;
    }

    drawNode->drawSolidPoly(vertices, segments, color);

    CC_SAFE_DELETE_ARRAY(vertices);
}

const Vec2 CCGameAvatar::roundPoint(const Vec2 &origin) const {

    float _target_x = origin.x;
    float _target_y = origin.y;

    std::stringstream _value_x;
    std::stringstream _value_y;
//    cout<<endl;
    _value_x << fixed << setprecision(2) << _target_x;
    _value_y << fixed << setprecision(2) << _target_y;

    float _ret_x = atof(_value_x.str().c_str());
    float _ret_y = atof(_value_y.str().c_str());

    return Vec2(_ret_x, _ret_y);
}




