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
#include "ui/UIScale9Sprite.h"
#include "CCGamePicture.h"

using namespace cocos2d::ui;

USING_NS_CC;

using namespace cocos2d::network;

static int _chat_local_z_order = 0;

const std::string CCGameAvatar::_BG_NAME_NORMAL_PNG = "bg_name_nor9.png";
const std::string CCGameAvatar::_BG_RANK_LABEL_PNG = "bg_rank_label9.png";
const std::string CCGameAvatar::_BG_RANK_NAME_PNG = "bg_rank_name9.png";
const std::string CCGameAvatar::_BG_SR_NAME_NORMAL_PNG = "bg_sr_name_nor9.png";
const std::string CCGameAvatar::_BG_SR_RANK_NAME_PNG = "bg_sr_rank_name9.png";
const std::string CCGameAvatar::_BG_SSR_NAME_NORMAL_PNG = "bg_ssr_name_nor9.png";
const std::string CCGameAvatar::_BG_SSR_RANK_NAME_PNG = "bg_ssr_rank_name9.png";

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

CCGameAvatar::~CCGameAvatar() {
    log("game avatar del alloc. uid=%s", _uid.c_str());
}

void CCGameAvatar::setTexture(const std::string &filename) {

    if (this->_inner_sprite) {
        this->_inner_sprite->setTexture(filename);
        if (strcmp(filename.c_str(), "cocos/avatar/default_avatar.png") != 0) {
            float _width_ratio = this->_inner_sprite->getContentSize().width / getContentSize().width;
            float _height_ratio = this->_inner_sprite->getContentSize().height / getContentSize().height;
            this->_inner_sprite->setScale(1 / MAX(_width_ratio, _height_ratio));
        } else {
            this->_inner_sprite->setScale(1);
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

    loadTexture(_skin.c_str(), "cocos/avatar/default_avatar.png");

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
    _name = "";
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


    auto _name_layer = ui::Scale9Sprite::create();
//    _name_layer->setTexture("avatar/bg_rank_name.png");
//    _name_layer->setCapInsets(Rect(6, 6, 1, 1));
//    _name_layer->setContentSize(Size(_name_label_width + 10 / _scale_factor,
//                                     _name_label_height));


//    auto _name_layer = ui::Scale9Sprite::create("avatar/chat_bubble9.png",
//                                                Rect(0, 0, 13, 13), Rect(6, 6, 1, 1));
//    auto _name_layer = LayerColor::create(Color4B(0, 0, 0, 100),
//                                          _name_label_width + 10 / _scale_factor,
//                                          _name_label_height);
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

    auto _rank_layer = ui::Scale9Sprite::create();//LayerColor::create(Color4B(0, 0, 0, 100), _rank_label->getContentSize().width, _rank_label->getContentSize().height);
    _rank_layer->setTag(_TAG_RANK_LAYER);
    _rank_layer->setLocalZOrder(1);
    _rank_layer->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    _rank_layer->setPosition((getContentSize().width - _name_layer->getContentSize().width) / 2, _name_label_height + _LABEL_HEIGHT_DEFAULT / _scale_factor);
    addChild(_rank_layer);


    auto _ssr_marker = Sprite::create("cocos/avatar/avatar_rare_sr.png");
    _ssr_marker->setTag(_TAG_SSR_MARKER);
    _ssr_marker->setLocalZOrder(3);
    _ssr_marker->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
    _ssr_marker->setPosition(getContentSize().width / _scale_factor, 0);
    addChild(_ssr_marker);
    _ssr_marker->setVisible(false);


    auto _snore_anim = Sprite::create();
    _snore_anim->setTexture("cocos/anim/snore_ani_1.png");
    _snore_anim->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
    _snore_anim->setPosition(Vec2(getContentSize().width + 10 / _scale_factor, getContentSize().height));
    _snore_anim->setLocalZOrder(4);
    this->addChild(_snore_anim, 1, _TAG_SNORE_ANIM);
    _snore_anim->setVisible(false);


    this->setLocalZOrder(1);

    _loaded = false;
    _target_x = 0.0;
    _target_y = 0.0;
    _rare = 0;
    _last_rare = 0;
    _self_chat_bubble_count = 0;
    _real_local_z_order = 0;
    offline = 0;
    realRanking = 0;
    _jump_times = 0;
    _jump_ing = false;

    isRequestRetry = false;
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
    this->realRanking = rank;
    int _column = _FRONT_COLUMN_COUNT;

    int _new_local_z_order = rank;
    if (rank < 100) {
        _new_local_z_order = ((rank - 1) / _column) * _column + (_column - (rank - 1) % _column);
    } else {
        _new_local_z_order = rank;
    }

//    _real_local_z_order = (rank < 100) ? (100 - rank) : rank;
    if (this->getLocalZOrder() < _CHAT_LOCAL_Z_ORDER_BASE)
        this->setLocalZOrder(_new_local_z_order);

//    if ((_new_local_z_order == this->_real_local_z_order) && (this->_last_rare == this->_rare))
//        return;

    this->_last_rare = this->_rare;
    this->_real_local_z_order = _new_local_z_order;

    auto _rank_label = dynamic_cast<Label*>(this->getChildByTag(_TAG_RANK_LABEL));
    auto _rank_layer = dynamic_cast<ui::Scale9Sprite*>(this->getChildByTag(_TAG_RANK_LAYER));
    auto _name_layer = dynamic_cast<ui::Scale9Sprite*>(this->getChildByTag(_TAG_NAME_LAYER));

    bool _is_front = rank <= _FRONT_COLUMN_COUNT;
    bool _is_sr = this->_rare == 3;
    bool _is_ssr = this->_rare == 4;
    if (_is_front) {
        float _labelWidth = 0;
        float _labelHeight = 0;
        if (_rank_label) {
            _rank_label->setVisible(true);
            _rank_label->setString(std::to_string(rank) + "名");
            _labelWidth = _rank_label->getContentSize().width;
            _labelHeight = _rank_label->getContentSize().height;


            if (rank <= 3) {
                _rank_label->setColor(Color3B(255, 230, 138));
            } else {
                _rank_label->setColor(Color3B(255, 255, 255));
            }
        }
        if (_rank_layer) {
            _rank_layer->setVisible(true);

            std::string _bg_rank_label = _BG_RANK_LABEL_PNG;
            string _origin_name = _rank_layer->getName();
            if ((strcmp(_bg_rank_label.c_str(), _origin_name.c_str()) != 0)) {
                _rank_layer->setTexture("cocos/avatar/" + _bg_rank_label);
                _rank_layer->setName(_bg_rank_label);
                _rank_layer->setCapInsets(Rect(6, 6, 1, 1));
            }

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


    std::string _bg_name_label;
    if (_is_sr) {
        _bg_name_label = _is_front ? _BG_SR_RANK_NAME_PNG : _BG_SR_NAME_NORMAL_PNG;
    } else if (_is_ssr) {
        _bg_name_label = _is_front ? _BG_SSR_RANK_NAME_PNG : _BG_SSR_NAME_NORMAL_PNG;
    } else {
        _bg_name_label = _is_front ? _BG_RANK_NAME_PNG : _BG_NAME_NORMAL_PNG;
    }

    if (_name_layer) {
        string _origin_name = _name_layer->getName();
        if ((strcmp(_bg_name_label.c_str(), _origin_name.c_str()) != 0)) {
            Size _origin_size = _name_layer->getContentSize();
            _name_layer->setTexture("cocos/avatar/" + _bg_name_label);
            _name_layer->setName(_bg_name_label);
            _name_layer->setCapInsets(Rect(6, 6, 1, 1));
            _name_layer->setContentSize(_origin_size);
        }
    }
}

void CCGameAvatar::updateElement(const char *name, const char *path, int rare, int guard, int offline) {

    if ((strcmp(path, this->_skin.c_str()) != 0) || this->isRequestRetry) {
        this->_skin = path;
        loadTexture(_skin.c_str(), "cocos/avatar/default_avatar.png");
    }


    this->_rare = rare;
    this->offline = offline;

    bool _is_sr = rare == 3;
    bool _is_ssr = rare == 4;
    bool _is_guard = guard == 1;
    auto _rank_label = dynamic_cast<Label*>(this->getChildByTag(_TAG_RANK_LABEL));
    auto _rank_layer = dynamic_cast<ui::Scale9Sprite*>(this->getChildByTag(_TAG_RANK_LAYER));
    auto _name_label = dynamic_cast<Label*>(this->getChildByTag(_TAG_NAME_LABEL));
    auto _name_layer = dynamic_cast<ui::Scale9Sprite*>(this->getChildByTag(_TAG_NAME_LAYER));
    auto _ssr_marker = dynamic_cast<Sprite*>(this->getChildByTag(_TAG_SSR_MARKER));
    auto _snore_anim = dynamic_cast<Sprite*>(this->getChildByTag(_TAG_SNORE_ANIM));

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
//        if (_rank_label)
//            _rank_label->setColor(Color3B(255, 230, 138));
    } else {
        if (_name_label)
            _name_label->setColor(Color3B(255, 255, 255));
//        if (_rank_label)
//            _rank_label->setColor(Color3B(255, 255, 255));
    }

    if (_ssr_marker) {
        if (_is_sr) {
            _ssr_marker->setVisible(true);
            _ssr_marker->setTexture("cocos/avatar/avatar_rare_sr.png");
        } else if (_is_ssr) {
            _ssr_marker->setVisible(true);
            _ssr_marker->setTexture("cocos/avatar/avatar_rare_ssr.png");
        } else {
            _ssr_marker->setVisible(false);
        }
    }

    if (offline != 1) {
        _snore_anim->setVisible(false);
    }



//    if (offline == 1) {
//        _rank_label->setOpacity(_is_sr ? 179 : 125);
//        _rank_layer->setOpacity(_is_sr ? 230 : 255);
//        _name_label->setOpacity(_is_sr ? 179 : 125);
//        _name_layer->setOpacity(_is_sr ? 230 : 255);
//    } else {
//        _rank_label->setOpacity(255);
//        _rank_layer->setOpacity(255);
//        _name_label->setOpacity(255);
//        _name_layer->setOpacity(255);
//        _snore_anim->setVisible(false);
//    }
//    cocos2d::GLProgramState* glProgramState = offline == 1 ? getDarkGLProgramState() : getLightGLProgramState();
//
//    if (nullptr != glProgramState) {
//        if (this->_inner_sprite) {
//            this->_inner_sprite->setGLProgramState(glProgramState);
//        }
//        if (_ssr_marker) {
//            _ssr_marker->setGLProgramState(glProgramState);
//        }
//    }
}

void CCGameAvatar::setOffline(int offline) {
    if (this->offline != offline) {
        this->offline = offline;
//        auto _ssr_marker = dynamic_cast<Sprite*>(this->getChildByTag(_TAG_SSR_MARKER));
//        cocos2d::GLProgramState* glProgramState = offline == 1 ? getDarkGLProgramState() : getLightGLProgramState();
//        if (nullptr != glProgramState) {
//            if (this->_inner_sprite) {
//                this->_inner_sprite->setGLProgramState(glProgramState);
//            }
//            if (_ssr_marker) {
//                _ssr_marker->setGLProgramState(glProgramState);
//            }
//        }
    }
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
    auto _self = this;
    _self->_jump_times = 0;

    auto _jump_action = getActionByTag(_TAG_JUMP_BY_ACTION);

    if (!_self->_jump_ing) {
        this->stopAction(_jump_action);
        jumpByRepeat();
    }

    auto _reset_order_action = getActionByTag(_TAG_REST_ORDER_ACTION);
    if (nullptr != _reset_order_action && !_reset_order_action->isDone()) {
        this->stopAction(_reset_order_action);
    }

    _self->setLocalZOrder(_PRESENT_LOCAL_Z_ORDER);
    auto _resetSelfZOrder = CallFunc::create([_self](){
        int _current_z_order = _self->getLocalZOrder();
        if (_current_z_order == _PRESENT_LOCAL_Z_ORDER) {
            if (_self->_self_chat_bubble_count <= 0)
                _self->setLocalZOrder(_self->_real_local_z_order);
        }
    });

    auto _action = Sequence::create(DelayTime::create(1), _resetSelfZOrder, nullptr);
    _action->setTag(_TAG_REST_ORDER_ACTION);
    runAction(_action);
}

void CCGameAvatar::jumpByRepeat() {
    auto _self = this;

    auto _callback = CallFunc::create([_self](){
        _self->setPosition(Vec2(_self->_target_x, _self->_target_y));
        if (_self->_jump_times < 3) {
            _self->jumpByRepeat();
            _self->_jump_times++;
        } else {
            _self->_jump_times = 0;
            _self->_jump_ing = false;
        }
    });
    auto _jumpBy = JumpBy::create(0.5f, Vec2(0, 0), 120  / _scale_factor, 1);
    auto _action = Sequence::create(_jumpBy, _callback, nullptr);
    _action->setTag(_TAG_JUMP_BY_ACTION);
    _self->runAction(_action);
    _self->_jump_ing = true;
}

void CCGameAvatar::popChatMsgBubble(const char* content, ChatBubbleType type) {
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

    if (type == MESSAGE) {
        _chat_label->setTextColor(Color4B(0,0,0,255));
    }else if (type == INFO) {
        _chat_label->setTextColor(Color4B(255,255,255,255));
    }
    completeBubble(_chat_label, type);
}

void CCGameAvatar::popChatPicBubble(const char *url) {
    auto _chat_picture = CCGamePicture::create(0, 0, url);
    _chat_picture->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _chat_picture->loadTexture(url);
    _chat_picture->setPosition(10 / _scale_factor + _chat_picture->getContentSize().width / 2 , 10 / _scale_factor + _chat_picture->getContentSize().height / 2 );
    completeBubble(_chat_picture, ChatBubbleType::PIC);
}

void CCGameAvatar::completeBubble(Node *content, ChatBubbleType type) {
    auto _chat_bubble = CCBaseSprite::create();
    _chat_bubble->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    _chat_bubble->setPosition(getContentSize().width/ 2, _LABEL_HEIGHT_DEFAULT / _scale_factor);;
    _chat_bubble->setContentSize(Size(content->getContentSize().width + 20  / _scale_factor, content->getContentSize().height + 20  / _scale_factor));


    auto _background = ui::Scale9Sprite::create();
    if (type == INFO) {
        _background->initWithFile("cocos/avatar/chat_bubble_info9.png.png", Rect(0, 0, 25, 25), Rect(13, 13, 1, 1));
    } else {
        if (type == MESSAGE) {
            auto _name_label = Label::createWithSystemFont(_name, "Arial", 20 / _scale_factor, Size::ZERO, TextHAlignment::RIGHT);
            _name_label->setOverflow(Label::Overflow::CLAMP);
            _name_label->setAnchorPoint(Point::ANCHOR_BOTTOM_RIGHT);
            _name_label->setPosition(content->getContentSize().width + 10 / _scale_factor ,  10 / _scale_factor);
            _name_label->setDimensions(_CHAT_POP_BUBBLE_WIDTH / _scale_factor, _name_label->getContentSize().height);
            _name_label->setTextColor(Color4B(0,0,0,127));
            _chat_bubble->addChild(_name_label, 1);
        }
        _background->initWithFile("cocos/avatar/chat_bubble9.png", Rect(0, 0, 25, 25), Rect(13, 13, 1, 1));
    }

    _background->setAnchorPoint(Point::ANCHOR_MIDDLE);
    _background->setPosition(_chat_bubble->getContentSize().width / 2, _chat_bubble->getContentSize().height / 2);
    _background->setContentSize(Size(_chat_bubble->getContentSize().width, _chat_bubble->getContentSize().height));


    _chat_bubble->addChild(_background);
    _chat_bubble->addChild(content);
    this->addChild(_chat_bubble, 3);

    auto _moveBy = MoveBy::create(4, Vec2(0, 30 / _scale_factor));

    auto _fadeFunc = CallFunc::create([content, _background](){
        content->runAction(FadeOut::create(2));
        _background->runAction(FadeOut::create(2));
    });

    if (_chat_local_z_order > 10000) {
        _chat_local_z_order = 0;//极限 不至于.
    }
    _chat_local_z_order++;
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

//    CC_SAFE_DELETE_ARRAY(vertices);
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

void CCGameAvatar::runSnoreAnim() {
    auto _child = this->getChildByTag(_TAG_SNORE_ANIM);
    if (auto _snore_anim = dynamic_cast<Sprite *>(_child)) {
        if (this->offline != 1) return;
        _snore_anim->setVisible(true);

        auto _anim_action = _snore_anim->getActionByTag(_TAG_SNORE_ANIM_ACTION);
        if (nullptr != _anim_action && !_anim_action->isDone()) {
            _snore_anim->stopAction(_anim_action);
        }

        Animation *_animation = Animation::create();
        for (int i = 1; i <= 75; i++) {
            auto imagePath = "cocos/anim/snore_ani_" + std::to_string(i) + ".png";
            _animation->addSpriteFrameWithFileName(imagePath.c_str());
        }
        _animation->setDelayPerUnit(0.040f);
        _animation->setRestoreOriginalFrame(false);
        Animate *_animate = Animate::create(_animation);
        auto _callback = CallFunc::create([_snore_anim](){
            _snore_anim->setVisible(false);
        });
        auto _action = Sequence::create(_animate, _callback, NULL);
        _action->setTag(_TAG_SNORE_ANIM_ACTION);
        _snore_anim->runAction(_action);
    }

}
