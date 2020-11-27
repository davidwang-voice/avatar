//
// Created by David on 2020/10/3.
//



#include <utility>
#include "CCGameAvatar.h"
#include "CCRoomScene.h"
#include "CCRoomDelegate.h"
#include <iomanip>
#include <cocos/editor-support/spine/extension.h>
#include "platform/CCDevice.h"


USING_NS_CC;

using namespace cocos2d::network;

static unsigned int _chat_local_z_order = 0;

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

void CCGameAvatar::setTexture(const std::string &filename) {

    if (this->_inner_sprite) {
        this->_inner_sprite->setTexture(filename);
        if (strcmp(filename.c_str(), "avatar/default_avatar.png") != 0) {
            float _width_ratio = this->_inner_sprite->getContentSize().width / getContentSize().width;
            float _height_ratio = this->_inner_sprite->getContentSize().height / getContentSize().height;
            this->_inner_sprite->setScale(1 / MAX(_width_ratio, _height_ratio));
        }
    }
}

void CCGameAvatar::initAvatar() {
    this->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);

    this->_scale_factor = Director::getInstance()->getContentScaleFactor();
    this->setContentSize(Size(_CONTENT_SIZE_WIDTH / _scale_factor, _CONTENT_SIZE_HEIGHT / _scale_factor));

    //assets/avatar/default_avatar.png
    this->_inner_sprite = Sprite::create();
    this->_inner_sprite->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    this->_inner_sprite->setPosition(getContentSize().width / 2, 0);
    this->addChild(this->_inner_sprite, 0);

    loadTexture(_skin.c_str(), "avatar/default_avatar.png");

    float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 1.5;
    auto delay = DelayTime::create(random);
    auto callback = CallFuncN::create(CC_CALLBACK_0(CCGameAvatar::shakingBody, this));
    auto seq = Sequence::create(delay, callback, nullptr);
    runAction(seq);

    TTFConfig labelConfig;
    labelConfig.fontFilePath = "font/droid.ttf";
    labelConfig.fontSize = 24 / _scale_factor;
    labelConfig.glyphs = GlyphCollection::DYNAMIC;
    labelConfig.outlineSize = 0;
    labelConfig.customGlyphs = nullptr;
    labelConfig.distanceFieldEnabled = false;

    int dpi = Device::getDPI();
//    auto _name_label = Label::createWithTTF(labelConfig, _name);

    auto _name_label = Label::createWithSystemFont(_name, "Arial", 24 / _scale_factor);
    _name_label->setOverflow(Label::Overflow::CLAMP);
    _name_label->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    _name_label->setPosition(getContentSize().width / 2, _LABEL_HEIGHT_DEFAULT / _scale_factor);
    _name_label->setTextColor(Color4B(255,255,255,255));
    _name_label->setTag(_TAG_NAME_LABEL);
    _name_label->setLocalZOrder(2);

    addChild(_name_label);
    float _name_label_width = _name_label->getContentSize().width;
    float _name_label_height = _name_label->getContentSize().height;

    if (_name_label_width > _NAME_LABEL_MAX_WIDTH / _scale_factor) {
        _name_label_width = _NAME_LABEL_MAX_WIDTH / _scale_factor;
    }
    _name_label->setDimensions(_name_label_width, _name_label_height);


    auto _name_layer = LayerColor::create(Color4B(0, 0, 0, 100),
                                          _name_label_width + 10 / _scale_factor,
                                          _name_label_height);
    _name_layer->setTag(_TAG_NAME_LAYER);
    _name_layer->setLocalZOrder(1);
    _name_layer->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _name_layer->setPosition((getContentSize().width - _name_layer->getContentSize().width) / 2, _LABEL_HEIGHT_DEFAULT / _scale_factor);
    addChild(_name_layer);


    auto _rank_label = Label::createWithSystemFont("", "Arial", 24 / _scale_factor);

    _rank_label->setTag(_TAG_RANK_LABEL);
    _rank_label->setLocalZOrder(2);
    _rank_label->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _rank_label->setPosition((getContentSize().width - _name_label_width) / 2, _name_label_height + _LABEL_HEIGHT_DEFAULT / _scale_factor);

    addChild(_rank_label);

    auto _rank_layer = LayerColor::create(Color4B(0, 0, 0, 100), _rank_label->getContentSize().width, _rank_label->getContentSize().height);
    _rank_layer->setTag(_TAG_RANK_LAYER);
    _rank_layer->setLocalZOrder(1);
    _rank_layer->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _rank_layer->setPosition((getContentSize().width - _name_layer->getContentSize().width) / 2, _name_label_height + _LABEL_HEIGHT_DEFAULT / _scale_factor);
    addChild(_rank_layer);


    auto _ssr_marker = Sprite::create("avatar/avatar_rare_ssr.png");
    _ssr_marker->setTag(_TAG_SSR_MARKER);
    _ssr_marker->setLocalZOrder(3);
    _ssr_marker->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
    _ssr_marker->setPosition(getContentSize().width, 0);
    addChild(_ssr_marker);
    _ssr_marker->setVisible(false);


    this->setLocalZOrder(1);

    _loaded = false;
    _target_x = 0.0;
    _target_y = 0.0;
    _self_chat_bubble_count = 0;
}

void CCGameAvatar::shakingBody() {
    auto _rotateRight = RotateTo::create(0.9f, 6.0f);
    auto _rotateLeft = RotateTo::create(0.9f, -6.0f);
    auto _action = Sequence::create(_rotateRight, _rotateLeft, nullptr);
    runAction(RepeatForever::create(_action));
}

void CCGameAvatar::setPosition(const Vec2 &position) {

    Sprite::setPosition(position);
//    _target_x = position.x;
//    _target_y = position.y;
//    log("sprite x: %f, y: %f", getPosition().x, getPosition().y);
}

const Vec2 CCGameAvatar::getCenterPosition() const {
    return Vec2(_target_x, _target_y + getContentSize().height / 2);
}

bool CCGameAvatar::onTouchBegan(Touch *touch, Event *event) {
    if (auto _avatar = dynamic_cast<CCGameAvatar*>(event->getCurrentTarget())) {
        Point _locationInNode = touch->getLocation();
        auto _pos = _avatar->getParent()->convertToNodeSpace(_locationInNode);
        Rect _rect = _avatar->getBoundingBox();
        if (_rect.containsPoint(_pos)) {
            if (!__touchBegin()) return false;

            log("Avatar onTouchesBegan... index = %d, uid = %s", _avatar->_id, _avatar->getUid());
            return true;
        }
    }

    return false;
}

void CCGameAvatar::onTouchMoved(Touch *touch, Event *event) {

}

void CCGameAvatar::onTouchEnded(Touch *touch, Event *event) {
    if (!__isTapEvent()) return;
    if (auto _avatar = dynamic_cast<CCGameAvatar*>(event->getCurrentTarget())) {
        Point _locationInNode = touch->getLocation();
        auto _pos = _avatar->getParent()->convertToNodeSpace(_locationInNode);
        Rect _rect = _avatar->getBoundingBox();
        if (_rect.containsPoint(_pos)) {
            log("Avatar onTouchesEnd... index = %d, uid = %s", _avatar->_id, _avatar->getUid());
            if (this->isOnStage) {
                onTouchStageAvatar(_avatar->getUid());
            } else {
                onTouchStandAvatar(_avatar->getUid());
            }
        }
    }
}

void CCGameAvatar::updateRank(int rank) {
    _real_local_z_order = (rank < 100) ? (100 - rank) : rank;
    if (this->getLocalZOrder() < _CHAT_LOCAL_Z_ORDER_BASE)
        this->setLocalZOrder(_real_local_z_order);
    auto _rank_label = dynamic_cast<Label*>(this->getChildByTag(_TAG_RANK_LABEL));
    auto _rank_layer = dynamic_cast<LayerColor*>(this->getChildByTag(_TAG_RANK_LAYER));
    auto _name_layer = dynamic_cast<LayerColor*>(this->getChildByTag(_TAG_NAME_LAYER));
    if (rank <= 13) {
        float _labelWidth = 0;
        float _labelHeight = 0;
        if (_rank_label) {
            _rank_label->setVisible(true);
            _rank_label->setString(std::to_string(rank) + "名");
            _labelWidth = _rank_label->getContentSize().width;
            _labelHeight = _rank_label->getContentSize().height;
        }
        if (_rank_layer) {
            _rank_layer->setVisible(true);
            _rank_layer->setContentSize(Size(_labelWidth + 10 / _scale_factor, _labelHeight));
        }

    } else {
        if (_rank_label) {
            _rank_label->setVisible(false);
        }
        if (_rank_layer) {
            _rank_layer->setVisible(false);
        }
    }


}

void CCGameAvatar::updateElement(const char *name, const char *path, int rare, int guard) {

    if ((strcmp(path, this->_skin.c_str()) != 0)) {
        this->_skin = path;
        loadTexture(_skin.c_str(), "avatar/default_avatar.png");
    }

    bool _is_ssr = rare == 4;
    bool _is_guard = guard == 1;
    auto _rank_label = dynamic_cast<Label*>(this->getChildByTag(_TAG_RANK_LABEL));
    auto _rank_layer = dynamic_cast<LayerColor*>(this->getChildByTag(_TAG_RANK_LAYER));
    auto _name_label = dynamic_cast<Label*>(this->getChildByTag(_TAG_NAME_LABEL));
    auto _name_layer = dynamic_cast<LayerColor*>(this->getChildByTag(_TAG_NAME_LAYER));
    auto _ssr_marker = dynamic_cast<Sprite*>(this->getChildByTag(_TAG_SSR_MARKER));


    if ((strcmp(name, this->_name.c_str()) != 0)) {
        log("avatar old name: %s, new name: %s", this->_name.c_str(), name);
        this->_name = name;

        float _labelWidth = 0;
        float _labelHeight = 0;
        if (_name_label) {
            _name_label->removeFromParent();


            _name_label = Label::createWithSystemFont(_name, "Arial", 24 / _scale_factor);
            _name_label->setTag(_TAG_NAME_LABEL);
            _name_label->setLocalZOrder(2);
            _name_label->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
            _name_label->setOverflow(Label::Overflow::CLAMP);
            _name_label->setPosition(getContentSize().width / 2, _LABEL_HEIGHT_DEFAULT / _scale_factor);
            _name_label->setTextColor(Color4B(255,255,255,255));
            this->addChild(_name_label);

            _labelWidth = _name_label->getContentSize().width;
            _labelHeight = _name_label->getContentSize().height;

            if (_labelWidth > _NAME_LABEL_MAX_WIDTH / _scale_factor) {
                _labelWidth = _NAME_LABEL_MAX_WIDTH / _scale_factor;
            }
            _name_label->setDimensions(_labelWidth, _labelHeight);
        }
        if (_name_layer) {
            _name_layer->setContentSize(Size(_labelWidth + 10 / _scale_factor, _labelHeight));
            _name_layer->setPosition((getContentSize().width - _name_layer->getContentSize().width) / 2, _LABEL_HEIGHT_DEFAULT / _scale_factor);
        }
        if (_rank_label) {
            _rank_label->setPosition((getContentSize().width - _labelWidth) / 2, _labelHeight + _LABEL_HEIGHT_DEFAULT / _scale_factor);
        }
        if (_rank_layer) {
            _rank_layer->setPosition((getContentSize().width - _name_layer->getContentSize().width) / 2, _labelHeight + _LABEL_HEIGHT_DEFAULT / _scale_factor);
        }
    }



    if (_is_guard) {
        if (_name_label)
            _name_label->setColor(Color3B(255, 230, 138));
        if (_rank_label)
            _rank_label->setColor(Color3B(255, 230, 138));
    } else {
        if (_name_label)
            _name_label->setColor(Color3B(255, 255, 255));
        if (_rank_label)
            _rank_label->setColor(Color3B(255, 255, 255));
    }

    if (_is_ssr) {
        if (_name_layer) {
            _name_layer->setColor(Color3B(235, 94, 137));
            _name_layer->setOpacity(250);
        }
        if (_rank_layer) {
            _rank_layer->setColor(Color3B(235, 94, 137));
            _rank_layer->setOpacity(250);
        }
    } else {
        if (_name_layer) {
            _name_layer->setColor(Color3B(0, 0, 0));
            _name_layer->setOpacity(100);
        }
        if (_rank_layer) {
            _rank_layer->setColor(Color3B(0, 0, 0));
            _rank_layer->setOpacity(100);
        }
    }
    _ssr_marker->setVisible(_is_ssr);
}

void CCGameAvatar::setUid(const char *uid) {
    this->_uid = uid;
}

const char* CCGameAvatar::getUid() {
    return _uid.c_str();
}

void CCGameAvatar::jumpToPosition(const Vec2 &target) {


    auto _cur_pos = Vec2(this->_target_x, this->_target_y);//this->getPosition();
    if (_cur_pos.equals(target)) {
        // already at target position
        log("already at target position  %f, %f", target.x, target.y);
        return;
    }

    this->_target_x = target.x;
    this->_target_y = target.y;

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
        auto _jumpTo = JumpTo::create(0.5, target, 80  / _scale_factor, 1);
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
        auto _jump_action = JumpBy::create(0.5f, Vec2(0, 0), 60  / _scale_factor, 1);
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

//    auto _chat_label = Label::createWithTTF(_label_config, content);
    auto _chat_label = Label::createWithSystemFont(StringUtils::toString(content) + "\n", "Arial", 24  / _scale_factor, Size(_CHAT_POP_BUBBLE_WIDTH / _scale_factor, 0));
    _chat_label->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _chat_label->setPosition(10 / _scale_factor + _chat_label->getContentSize().width / 2 , 10 / _scale_factor + _chat_label->getContentSize().height / 2 );
    Color4B  textColor(0,0,0,255);
    _chat_label->setTextColor(textColor);


    auto _name_label = Label::createWithSystemFont(_name, "Arial", 20 / _scale_factor, Size::ZERO, TextHAlignment::RIGHT);
    _name_label->setOverflow(Label::Overflow::CLAMP);
    _name_label->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
    _name_label->setPosition(_chat_label->getContentSize().width + 10 / _scale_factor ,  10 / _scale_factor);
    _name_label->setDimensions(_CHAT_POP_BUBBLE_WIDTH / _scale_factor, _name_label->getContentSize().height);
    _name_label->setTextColor(Color4B(0,0,0,127));


    auto _chat_bubble = CCBaseSprite::create();
    _chat_bubble->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    _chat_bubble->setPosition(getContentSize().width/ 2, _LABEL_HEIGHT_DEFAULT / _scale_factor);;
    _chat_bubble->setContentSize(Size(_chat_label->getContentSize().width + 20  / _scale_factor, _chat_label->getContentSize().height + 20  / _scale_factor));


    auto _drawNode = DrawNode::create();
    const Vec2 &origin = Vec2(0,0);
    const Vec2 &destination = Vec2(_chat_bubble->getContentSize().width, _chat_bubble->getContentSize().height);
    Color4F color4F(1, 1, 1, 0.95);

    drawRoundRect(_drawNode, origin, destination,12  / _scale_factor, 88,  color4F);

    _chat_bubble->addChild(_drawNode);
    _chat_bubble->addChild(_chat_label);
    _chat_bubble->addChild(_name_label);
    this->addChild(_chat_bubble, 3);

    auto _moveBy = MoveBy::create(4, Vec2(0, 30 / _scale_factor));

    auto _fadeFunc = CallFunc::create([_chat_label, _drawNode](){
        _chat_label->runAction(FadeOut::create(2));
        _drawNode->runAction(FadeOut::create(2));
    });

    if (_chat_local_z_order > 10000) {
        _chat_local_z_order = 0;//极限 不至于.
    }
    _chat_local_z_order++;
    log("_chat_local_z_order:%d", _chat_local_z_order);
    auto _self = this;
    _self->setLocalZOrder(_CHAT_LOCAL_Z_ORDER_BASE + _chat_local_z_order);
    auto _removeFunc = CallFunc::create([_self, _chat_bubble](){
        _self->_self_chat_bubble_count--;
        _chat_bubble->removeFromParentAndCleanup(true);
        if (_self->_self_chat_bubble_count <= 0)
            _self->setLocalZOrder(_self->_real_local_z_order);

    });

    auto _action = Sequence::create(_moveBy, _fadeFunc, DelayTime::create(2), _removeFunc, nullptr);
    _chat_bubble->runAction(_action);
    _self_chat_bubble_count++;


    int _chat_local_z_order_py = _chat_local_z_order;
    auto _resetChatZOrder = CallFunc::create([_chat_local_z_order_py](){
        if (_chat_local_z_order_py == _chat_local_z_order) {
            _chat_local_z_order = 0;
        }
    });

    runAction(Sequence::create(DelayTime::create(8), _resetChatZOrder, nullptr));

}

void CCGameAvatar::drawRoundRect(DrawNode *drawNode, const Vec2 &origin, const Vec2 &destination,
                                 float radius, unsigned int segments, const Color4F &color)
{
    segments = 40;//radius * 2;
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


void CCGameAvatar::drawRoundRect(DrawNode *drawNode, const Vec2 &origin, const Vec2 &destination,
                                 float radius, const Color4F &color, int type)
{


    unsigned int segments = radius * 2;
    const float coef = 2.0f * (float)M_PI / segments;

    Vec2 center = Vec2(0, 0);

    Vec2 *vertices = new (std::nothrow) Vec2[segments];

    if( ! vertices )
        return;


    if (type == 1) {

    }



    for (int i = 0; i <= segments; ++i) {
        float rads = (segments - i) * coef;
        vertices[i].x = radius* SIN(rads);
        vertices[i].y = radius* COS(rads);
    }

    float minX = MIN(origin.x, destination.x);
    float maxX = MAX(origin.x, destination.x);
    float minY = MIN(origin.y, destination.y);
    float maxY = MAX(origin.y, destination.y);

    Vec2 *polygons = new(std::nothrow) Vec2[segments*4 + 1 ];
    if( ! polygons )
        return;


    //left-top
    center.x     = minX + radius;
    center.y     = maxY - radius;
    unsigned int index = 0;
    for(int i = 0; i <= segments; ++i){
        ++index;
        polygons[index].x = center.x - vertices[i].x;
        polygons[index].y = center.y + vertices[i].y;
    }

    //right-top
    center.x        = maxX - radius;
    center.y        = maxY - radius;
    for(int j = segments; j!=0; --j){
        ++index;
        polygons[index].x = center.x + vertices[j].x;
        polygons[index].y = center.y + vertices[j].y;
    }

    //right-bottom
    center.x        = maxX - radius;
    center.y        = minY + radius;
    for(int i = 0; i <= segments; ++i){
        ++index;
        polygons[index].x = center.x + vertices[i].x;
        polygons[index].y = center.y - vertices[i].y;
    }

    //left-bottom
    center.x        = minX + radius;
    center.y        = minY + radius;
//    thisVertices    = vertices + segments;
    for(int j=segments; j!=0 ; --j){
        ++index;
        polygons[index].x = center.x - vertices[j].x;
        polygons[index].y = center.y - vertices[j].y;

    }


    drawNode->drawSolidPoly(polygons, segments*4 + 1, color);

//    drawNode->drawSolidPoly(vertices, segments, color);

    CC_SAFE_DELETE_ARRAY(vertices);
    CC_SAFE_DELETE_ARRAY(polygons);
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




