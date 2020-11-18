//
// Created by David on 2020/10/3.
//

#include <cmath>
#include <utility>

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <cocos/editor-support/cocostudio/DictionaryHelper.h>
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "DictionaryHelper.h"
#endif

#include "AppDelegate.h"
#include "CCRoomDelegate.h"
#include "AudioManager.h"

#include "json/rapidjson.h"
#include "json/document.h"

using namespace rapidjson;

USING_NS_CC;

static CCRoomDelegate *instance = NULL;

CCRoomDelegate *CCRoomDelegate::getInstance() {
    if (!instance) {
        instance = new CCRoomDelegate();
    }
    return instance;
}

CCRoomDelegate::~CCRoomDelegate() {
    _giftHolder.clear();
    _standAvatars.clear();
    _stageAvatars.clear();
}

void CCRoomDelegate::init(Scene* scene) {
    this->_scene = scene;
    this->_visibleOrigin = Director::getInstance()->getVisibleOrigin();
    this->_visibleSize = Director::getInstance()->getVisibleSize();
    this->_scaleFactor = Director::getInstance()->getContentScaleFactor();
    this->_centerPosition = Vec2(_visibleOrigin.x + _visibleSize.width / 2, _visibleOrigin.y + _visibleSize.height);

//    auto size = Director::getInstance()->getOpenGLView()->getFrameSize();


    for (int i = 0; i < _STAND_MAX_ROW_COUNT; ++i) {
        if (i == 0) {
            _standRowCount[0] = 13;
        } else {
            _standRowCount[i] = _standRowCount[i - 1] + 13;
        }
    }
}

void CCRoomDelegate::setStageBackground(const char *url) {
    if (_scene) {
        auto _child = _scene->getChildByTag(_TAG_STAGE_BACKGROUND);
        if (_child) {
            _child->removeFromParent();
        }
    }

    auto _stage_background = CCBaseSprite::create();
    _stage_background->loadTexture(url, "bg_game_room.png");
    float _target_x = _centerPosition.x;
    float _target_y = _centerPosition.y - _stage_background->getContentSize().height / 2;
    _stage_background->setPosition(Vec2(_target_x, _target_y));

    if (_scene) {
        _scene->addChild(_stage_background, 0, _TAG_STAGE_BACKGROUND);
    }
}

void CCRoomDelegate::setupStageGiftHeap(const char *json) {

    for (int i = 0; i < _giftHolder.size(); ++i) {
        auto _gift = dynamic_cast<CCGameGift*>(_giftHolder.at(i));
        _gift->removeFromParentAndCleanup(true);
    }

    _giftHolder.clear();

    rapidjson::Document _document;
    _document.Parse<rapidjson::kParseDefaultFlags>(json);
    if (_document.HasParseError()) {
        log("parse stage gift json error %s\n", _document.GetParseError());
        return;
    }
    if (!_document.IsArray()) {
        log("gift json is not array %s\n", json);
        return;
    }
    rapidjson::Value& _data_arr = _document;

    Vector<CCGameAvatar*> _new_stage_avatars;
    for (int i = 0; i < _data_arr.Size(); ++i) {
        rapidjson::Value &_value = _data_arr[i];

        const char *_url = cocostudio::DICTOOL->getStringValue_json(_value, "url");

        auto gift = CCGameGift::create(i, i, "gift/heart.png");
        gift->setPosition(getGiftPosition());

        if (_scene) {
            _scene->addChild(gift, 10000);
        }
        _giftHolder.pushBack(gift);
    }
}

void CCRoomDelegate::updateSelfAvatar(const char *json) {
    if (_scene) {
        auto _child = _scene->getChildByTag(_TAG_SELF_AVATAR);
        if (_child) {
            _child->removeFromParent();
        }
    }

    rapidjson::Document _document;
    _document.Parse<rapidjson::kParseDefaultFlags>(json);
    if (_document.HasParseError()) {
        log("parse self avatar json error %s\n", _document.GetParseError());
        return;
    }
    if (!_document.IsObject()) {
        log("self json is not object %s\n", json);
        return;
    }
    rapidjson::Value& _value = _document;
    const char *_url = cocostudio::DICTOOL->getStringValue_json(_value, "url");
    const char *_uid = cocostudio::DICTOOL->getStringValue_json(_value, "uid");
    const char *_name = cocostudio::DICTOOL->getStringValue_json(_value, "name");

    int _rare = cocostudio::DICTOOL->getIntValue_json(_value, "rare");


    auto _self_avatar = CCGameAvatar::create(_RANK_SELF_DEFAULT, _RANK_SELF_DEFAULT, _uid, _url, _name);

    _self_avatar->setUid(_uid);
    _self_avatar->updateElement(_name, _url, _rare);
    _self_avatar->updateRank(_RANK_SELF_DEFAULT);

    auto _self_position = this->getSelfPosition();
    _self_avatar->setPosition(Vec2(_visibleOrigin.x - _self_avatar->getContentSize().width, _self_position.y));

    if (_scene) {
        _scene->addChild(_self_avatar, _RANK_SELF_DEFAULT, _TAG_SELF_AVATAR);
    }
    _self_avatar->jumpToPosition(_self_position);

    if (_scene) {
        auto _child = _scene->getChildByTag(_TAG_SELF_APERTURE);

        auto _position = _self_avatar->getCenterPosition();
        if (nullptr == _child) {
            auto _aperture = CCBaseSprite::create();
            _aperture->setAnchorPoint(Point::ANCHOR_MIDDLE);
            _aperture->setTexture("aperture.png");

            _aperture->setPosition(_position);
            _scene->addChild(_aperture, 1, _TAG_SELF_APERTURE);
        } else {
            _child->setPosition(_position);
        }
    }
}

void CCRoomDelegate::updateStageAvatars(const char* json) {

    if (_stageSteps.empty()) {


        for (int i = 0; i < _STAGE_BLOCK_COUNT; ++i) {
            auto _stage_step = CCGameStep::create(i, i, "");
            auto _stage_pos = this->getStepPosition(i);
            _stage_step->setPosition(Vec2(_stage_pos.x, _stage_pos.y));
            if (_scene) {
                _scene->addChild(_stage_step);
            }
            _stageSteps.pushBack(_stage_step);
        }
    }

    rapidjson::Document _document;
    _document.Parse<rapidjson::kParseDefaultFlags>(json);
    if (_document.HasParseError()) {
        log("parse stage avatar json error %s\n", _document.GetParseError());
        return;
    }
    if (!_document.IsArray()) {
        log("stage json is not array %s\n", json);
        return;
    }
    rapidjson::Value& _data_arr = _document;

    Vector<CCGameAvatar*> _new_stage_avatars;
    int length = MIN(_data_arr.Size(), _STAGE_BLOCK_COUNT);
    for (int i = 0; i < length; ++i) {
        auto _position = getStagePosition(i);
        if (_position.isZero()) continue;

        rapidjson::Value& _value = _data_arr[i];
        const char *_url = cocostudio::DICTOOL->getStringValue_json(_value, "url");
        const char *_uid = cocostudio::DICTOOL->getStringValue_json(_value, "uid");
        const char *_name = cocostudio::DICTOOL->getStringValue_json(_value, "name");

        int _rare = cocostudio::DICTOOL->getIntValue_json(_value, "rare");

        bool _mute = cocostudio::DICTOOL->getBooleanValue_json(_value, "mute");

        if (auto _cur_self_avatar = this->findSelfAvatar(_uid)) {
            _new_stage_avatars.pushBack(_cur_self_avatar);
        } else if (auto _old_stage_avatar = this->findStageAvatar(_uid)) {
            _new_stage_avatars.pushBack(_old_stage_avatar);
        } else {
            auto _old_stand_avatar = this->findStandAvatar(_uid);
            if (nullptr != _old_stand_avatar) {
                _new_stage_avatars.pushBack(_old_stand_avatar);
            } else {

                auto _new_stage_avatar = createAvatar(i + 1, _uid, _name, _url, _position);
                _new_stage_avatars.pushBack(_new_stage_avatar);
            }
        }

        auto _new_stage_avatar = _new_stage_avatars.back();
        _standAvatars.eraseObject(_new_stage_avatar);
        _new_stage_avatar->updateElement(_name, _url, _rare);


        auto _stageStep = _stageSteps.at(i);
        _stageStep->setUid(_uid);
        _stageStep->setMute(_mute);
    }

    for (int i = 0; i < _stageAvatars.size(); ++i) {
        auto _avatar = _stageAvatars.at(i);
        if (!_new_stage_avatars.contains(_avatar)) {
            if (_scene) {
                _scene->removeChild(_avatar);
            }
        }
    }
    _stageAvatars.clear();
    _stageAvatars.pushBack(_new_stage_avatars);
    reorganizeStageAvatars();
    reorganizeStandAvatars();
    reorganizeSelfAvatar();
}

void CCRoomDelegate::updateStandAvatars(const char* json) {
    rapidjson::Document _document;
    _document.Parse<rapidjson::kParseDefaultFlags>(json);
    if (_document.HasParseError()) {
        log("parse stand avatar json error %s\n", _document.GetParseError());
        return;
    }
    if (!_document.IsArray()) {
        log("stand json is not array %s\n", json);
        return;
    }
    rapidjson::Value& _data_arr = _document;

    Vector<CCGameAvatar*> _new_stand_avatars;
    for (int i = 0; i < _data_arr.Size(); ++i) {
        auto _position = getStandPosition(i);
        if (_position.isZero()) continue;

        rapidjson::Value& _value = _data_arr[i];

        const char *_url = cocostudio::DICTOOL->getStringValue_json(_value, "url");
        const char *_uid = cocostudio::DICTOOL->getStringValue_json(_value, "uid");
        const char *_name = cocostudio::DICTOOL->getStringValue_json(_value, "name");
        int _rare = cocostudio::DICTOOL->getIntValue_json(_value, "rare");

        auto _cur_self_avatar = this->findSelfAvatar(_uid);

        if (nullptr != _cur_self_avatar && !_cur_self_avatar->isOnStage) {
            _new_stand_avatars.pushBack(_cur_self_avatar);
        } else if ( auto _old_stand_avatar = this->findStandAvatar(_uid)) {
            _new_stand_avatars.pushBack(_old_stand_avatar);
        } else {
            auto _new_stand_avatar = createAvatar(i + 1, _uid, _name, _url, _position);
            _new_stand_avatars.pushBack(_new_stand_avatar);
        }

        _new_stand_avatars.back()->updateElement(_name, _url, _rare);
    }


    for (int i = 0; i < _standAvatars.size(); ++i) {
        auto _avatar = _standAvatars.at(i);
        if (!_new_stand_avatars.contains(_avatar)) {
            if (_scene) {
                _scene->removeChild(_avatar);
            }
        }
    }

    _standAvatars.clear();
    _standAvatars.pushBack(_new_stand_avatars);
    reorganizeStandAvatars();
    reorganizeSelfAvatar();
}


void CCRoomDelegate::backOffStageAvatar(const char* uid) {

}

void CCRoomDelegate::backOffStandAvatar(const char* uid) {

}

void CCRoomDelegate::receiveGiftMessage(const char* uid, const char* url) {

    auto _avatar = this->findAvatar(uid);

    if (nullptr != _avatar) {
        _avatar->jumpByPresent();
        createAndPresentGift(_avatar->getPosition(), url);
    } else {
        createAndPresentGift(this->getNonePosition(), url);
    }
}

void CCRoomDelegate::receiveChatMessage(const char* uid, const char* content) {
    auto _avatar = this->findAvatar(uid);
    if (nullptr == _avatar) return;

    _avatar->popChatBubble(content);
}

void CCRoomDelegate::receiveVoiceWave(const char *uids) {

    log("receive voice wave uids: %s", uids);
    std::vector<string> _uid_arr;
    std::string _raw = uids, _tmp;
    std::stringstream input(_raw);
    while (getline(input, _tmp, ',')) _uid_arr.push_back(_tmp);

    for (int i = 0; i < _stageSteps.size(); ++i) {
        auto _stage_step = _stageSteps.at(i);
        const char *_stage_uid = _stage_step->getUid();
        if(std::find(_uid_arr.begin(), _uid_arr.end(), _stage_uid) != _uid_arr.end()) {
            _stage_step->runVoiceWave();
        }
    }
}


void CCRoomDelegate::releaseResource() {

    _giftHolder.clear();
    _standAvatars.clear();
    _stageAvatars.clear();
    _stageSteps.clear();

    if (_scene) {
        _scene->removeAllChildren();
    }
}


const Vec2 CCRoomDelegate::getStepPosition(int index) const {
    log("_scaleFactor:%f", _scaleFactor);
    if (index >= _STAGE_BLOCK_COUNT) return Vec2::ZERO;
    float _target_x = _STAGE_BLOCK_LEFT / _scaleFactor + index * _STAGE_BLOCK_WIDTH / _scaleFactor;
    float _target_y = _centerPosition.y - _STAGE_STEP_TOP / _scaleFactor;
    return Vec2(_target_x , _target_y);
}


const Vec2 CCRoomDelegate::getStagePosition(int index) const {
    if (index >= _STAGE_BLOCK_COUNT) return Vec2::ZERO;
    float _target_x = _STAGE_BLOCK_LEFT / _scaleFactor + index * _STAGE_BLOCK_WIDTH / _scaleFactor;
    float _target_y = _centerPosition.y - _STAGE_BLOCK_TOP / _scaleFactor;
    return Vec2(_target_x , _target_y);
}

const Vec2 CCRoomDelegate::getStandPosition(int index) const {

    int _row = -1;
    for (int i = 0; i < _STAND_MAX_ROW_COUNT; ++i) {
        if (index < _standRowCount[i]) {
            _row = i;
            break;
        }
    }
    if (_row < 0) return Vec2::ZERO;
    int _column = _row == 0 ? index : index - _standRowCount[_row - 1];

    int _count = _row == 0 ? _standRowCount[0] : _standRowCount[_row] - _standRowCount[_row - 1];
    float _block_w = (float) _visibleSize.width / _count;

    float _refer_x = _centerPosition.x;
    float _refer_y = _centerPosition.y - _STAND_FRONT_ROW_TOP / _scaleFactor -
            (_row == 0 ? 0 : (_STAND_FRONT_ROW_HEIGHT + (_row - 1) * _STAND_ROW_HEIGHT) / _scaleFactor);
    float _arc_seg = _STAND_ARC_HEIGHT / (_count / 2) / _scaleFactor;


    int _direct_x = (float) pow(-1, _column % 2);
    float _offset_x = _count % 2 == 0 ? _block_w / 2 : 0;
    float _target_x = _refer_x + (_offset_x + ((_count % 2 + _column) / 2) * _block_w) * _direct_x;
    float _target_y = _refer_y + (_column / 2) * _arc_seg;

    return Vec2(_target_x , _target_y);
}

const Vec2 CCRoomDelegate::getSelfPosition() const {
    return Vec2(_centerPosition.x ,  _SELF_GROUND_BOTTOM / _scaleFactor);
}

const Vec2 CCRoomDelegate::getGiftPosition() const {
    float _coord_x = _GIFT_TABLE_WIDTH / _scaleFactor / 2;
    float _coord_y = _GIFT_TABLE_HEIGHT_MAX / _scaleFactor / 2;
    float _space_x = 40 / _scaleFactor;
    float _space_y = 5 / _scaleFactor;

    float _rand_x = rand() % (int)((_coord_x - _space_x) * 2) - (_coord_x - _space_x);
    float _result_y = std::sqrt((1 - _rand_x * _rand_x / _coord_x / _coord_x) * _coord_y * _coord_y);

    float _rand_y = rand() % (int)((_result_y - _space_y) * 2) - (_result_y);

//    float _coord_y_l = _GIFT_TABLE_HEIGHT_MIN / _scaleFactor / 2;
//    float _result_y_l = std::sqrt((1 - _rand_x * _rand_x / _coord_x / _coord_x) * _coord_y_l * _coord_y_l);

    float _target_x = _rand_x + _centerPosition.x;
    float _target_y = _centerPosition.y - (_GIFT_TABLE_TOP / _scaleFactor + _rand_y);

    return Vec2(_target_x , _target_y);
}

const Vec2 CCRoomDelegate::getNonePosition() const {
    int _where = cocos2d::RandomHelper::random_int(1, 2);
    float _target_x = - _NONE_SPACE_X / _scaleFactor;
    if (_where == 2) {
        _target_x =  _visibleSize.width +  _NONE_SPACE_X / _scaleFactor;
    }
    float _target_y = _centerPosition.y - _NONE_SPACE_Y / _scaleFactor;
    return Vec2(_target_x , _target_y);
}


CCGameAvatar* CCRoomDelegate::findStageAvatar(const char *uid) {
    for (int i = 0; i < _stageAvatars.size(); ++i) {
        auto _avatar = _stageAvatars.at(i);
        if (strcmp(uid, _avatar->getUid()) == 0) return _avatar;
    }
    return nullptr;
}

CCGameAvatar* CCRoomDelegate::findStandAvatar(const char *uid) {
    for (int i = 0; i < _standAvatars.size(); ++i) {
        auto _avatar = _standAvatars.at(i);
        if (strcmp(uid, _avatar->getUid()) == 0) return _avatar;
    }
    return nullptr;
}

CCGameAvatar* CCRoomDelegate::findSelfAvatar(const char *uid) {

    if (_scene) {
        auto _child = _scene->getChildByTag(_TAG_SELF_AVATAR);
        if (auto _avatar = dynamic_cast<CCGameAvatar*>(_child)) {
            if ((strcmp(uid, _avatar->getUid()) == 0)) {
                return _avatar;
            }
        }
    }

    return nullptr;
}

CCGameAvatar* CCRoomDelegate::findAvatar(const char *uid) {

    if (auto _avatar = this->findSelfAvatar(uid)) {
        return _avatar;
    } else if (auto _avatar = this->findStageAvatar(uid)) {
        return _avatar;
    } else {
        return this->findStandAvatar(uid);
    }
}


CCGameAvatar* CCRoomDelegate::createAvatar(int rank, const char* uid, const char* name, const char* url, const Vec2 &pos) {

    auto _avatar = CCGameAvatar::create(rank, rank, uid, url, name);
    if (pos.x < _centerPosition.x) {
        _avatar->setPosition(Vec2(_visibleOrigin.x - _avatar->getContentSize().width, pos.y));
    } else {
        _avatar->setPosition(Vec2(_visibleOrigin.x + _visibleSize.width + _avatar->getContentSize().width, pos.y));
    }
    if (_scene) {
        _scene->addChild(_avatar, 1);
    }
    return _avatar;
}


CCGameAvatar* CCRoomDelegate::removeAvatar(const char *uid) {

    auto _avatar = findStageAvatar(uid);
    if (nullptr != _avatar) {
        _stageAvatars.eraseObject(_avatar);
        if(_scene) {
            _scene->removeChild(_avatar);
        }
    } else {
        _avatar = findStandAvatar(uid);
        if (nullptr != _avatar) {

            _standAvatars.eraseObject(_avatar);
            if(_scene) {
                _scene->removeChild(_avatar);
            }
        }
    }
    reorganizeStandAvatars();
    return _avatar;
}

void CCRoomDelegate::createAndPresentGift(const Vec2& pos, const char* url) {

//    AudioManager::getInstance()->playBG();

    float start_x = pos.x;
    float start_y = pos.y + 60;

    int _gift_index = _giftHolder.size();

    int _star_index = cocos2d::RandomHelper::random_int(1, 5);
    std::string _star_path = "gift/star_" + std::to_string(_star_index) + ".png";

    auto gift = CCGameGift::create(_gift_index, _gift_index, _star_path);
    gift->setPosition(Vec2(start_x, start_y));

    auto _position = getGiftPosition();


    gift->present(_position);
    if (_scene) {
        _scene->addChild(gift, 10000);
    }
    _giftHolder.pushBack(gift);
    limitGiftHolderSize();
}


void CCRoomDelegate::limitGiftHolderSize() {
    int _length = _giftHolder.size();
    int _limit = _GIFT_HOLDER_SIZE;
    if(_length > _limit * 2) {
        auto _removeList = Vector<CCGameGift*>(_limit);

        for (int i = 0; i < _length; ++i) {
            if (i < _limit) {
                _removeList.pushBack(_giftHolder.at(i));
            } else {
                break;
            }
        }

        for (int i = 0; i < _limit; ++i) {
            auto _gift = dynamic_cast<CCGameGift*>(_removeList.at(i));

            auto _removeFunc = CallFunc::create([_gift](){
                _gift->removeFromParentAndCleanup(true);
            });
            auto _action = Spawn::createWithTwoActions(
                    MoveBy::create(0.5, Vec2(0, 40)),
                    FadeOut::create(0.5)
                    );
            auto _sequence = Sequence::create(_action, _removeFunc, nullptr);
            _gift->runAction(_sequence);
            _giftHolder.eraseObject(_gift);
        }
        _removeList.clear();
    }
}

void CCRoomDelegate::reorganizeStageAvatars() {
    for (int i = 0; i < _stageAvatars.size(); ++i) {
        auto _position = this->getStagePosition(i);
        if (_position.isZero()) continue;
        auto _stage_avatar = _stageAvatars.at(i);
        _stage_avatar->updateRank(_RANK_STAGE_DEFAULT + i);
        _stage_avatar->jumpToPosition(_position);
        _stage_avatar->isOnStage = true;
    }
}

void CCRoomDelegate::reorganizeStandAvatars() {
    for (int i = 0; i < _standAvatars.size(); ++i) {
        auto _position = this->getStandPosition(i);
        if (_position.isZero()) continue;
        auto _stand_avatar = _standAvatars.at(i);
        _stand_avatar->updateRank(_RANK_STAND_DEFAULT + i);
        _stand_avatar->jumpToPosition(_position);
        _stand_avatar->isOnStage = false;
    }
}

void CCRoomDelegate::reorganizeSelfAvatar() {

    if (_scene) {

        auto _child = _scene->getChildByTag(_TAG_SELF_AVATAR);
        if (auto _self_avatar = dynamic_cast<CCGameAvatar *>(_child)) {
            if (!_standAvatars.contains(_self_avatar) && !_stageAvatars.contains(_self_avatar)) {
                auto _position = this->getSelfPosition();
                _self_avatar->updateRank(_RANK_SELF_DEFAULT);
                _self_avatar->jumpToPosition(_position);
                _self_avatar->isOnStage = false;
            }


            auto _aperture = _scene->getChildByTag(_TAG_SELF_APERTURE);
            if (_aperture) {
                _aperture->setPosition(_self_avatar->getCenterPosition());
                _aperture->setVisible(!_self_avatar->isOnStage);
            }


        }
    }
}





void onTouchAvatar(const char* uid) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    Java_onTouchAvatar(uid);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    OCCallback::getInstance()->onTouchAvatar(_avatar->getUid());
#endif
}

void onTouchScene() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    Java_onTouchScene();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#endif
}