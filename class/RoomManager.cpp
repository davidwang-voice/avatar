//
// Created by David on 2020/10/3.
//

#include <cmath>
#include <utility>
#include <cocos/editor-support/cocostudio/DictionaryHelper.h>
#include "AppDelegate.h"
#include "RoomManager.h"
#include "AudioManager.h"

#include "json/rapidjson.h"
#include "json/document.h"

using namespace rapidjson;

USING_NS_CC;

static RoomManager *instance = NULL;

RoomManager *RoomManager::getInstance() {
    if (!instance) {
        instance = new RoomManager();
    }
    return instance;
}

RoomManager::~RoomManager() {
    _giftHolder.clear();
    _standAvatars.clear();
    _stageAvatars.clear();
}

void RoomManager::init(Scene* scene) {
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


void RoomManager::updateStageAvatars(const char* json) {
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

    Vector<RoomAvatar*> _new_stage_avatars;
    for (int i = 0; i < _data_arr.Size(); ++i) {
        auto _position = getStagePosition(i);
        if (_position.isZero()) continue;

        rapidjson::Value& _value = _data_arr[i];
        const char *_path = cocostudio::DICTOOL->getStringValue_json(_value, "path");
        const char *_uid = cocostudio::DICTOOL->getStringValue_json(_value, "uid");
        const char *_name = cocostudio::DICTOOL->getStringValue_json(_value, "name");

        bool _ssr = cocostudio::DICTOOL->getBooleanValue_json(_value, "ssr");

        bool _mute = cocostudio::DICTOOL->getBooleanValue_json(_value, "mute");
        bool _wave = cocostudio::DICTOOL->getBooleanValue_json(_value, "wave");

        auto _old_stage_avatar = this->findStageAvatar(_uid);
        if (nullptr != _old_stage_avatar) {
            _new_stage_avatars.pushBack(_old_stage_avatar);
        } else {
            auto _old_stand_avatar = this->findStandAvatar(_uid);
            if (nullptr != _old_stand_avatar) {
                _old_stand_avatar->jumpTo(_position);

                _standAvatars.eraseObject(_old_stand_avatar);

                _new_stage_avatars.pushBack(_old_stand_avatar);
            } else {

                auto _new_stage_avatar = createAvatar(i + 1, _uid, _name, _path, _position);
                _new_stage_avatars.pushBack(_new_stage_avatar);
            }
        }
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
}

void RoomManager::updateStandAvatars(const char* json) {
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

    Vector<RoomAvatar*> _new_stand_avatars;
    for (int i = 0; i < _data_arr.Size(); ++i) {
        auto _position = getStandPosition(i);
        if (_position.isZero()) continue;

        rapidjson::Value& _value = _data_arr[i];

        const char *_path = cocostudio::DICTOOL->getStringValue_json(_value, "path");
        const char *_uid = cocostudio::DICTOOL->getStringValue_json(_value, "uid");
        const char *_name = cocostudio::DICTOOL->getStringValue_json(_value, "name");

        auto _old_stand_avatar = this->findStandAvatar(_uid);
        if (nullptr != _old_stand_avatar) {
            _new_stand_avatars.pushBack(_old_stand_avatar);
        } else {
            auto _new_stand_avatar = createAvatar(i + 1, _uid, _name, _path, _position);
            _new_stand_avatars.pushBack(_new_stand_avatar);
        }
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
}


void RoomManager::backOffStageAvatar(const char* uid) {

}

void RoomManager::backOffStandAvatar(const char* uid) {

}

void RoomManager::receiveGiftMessage(const char* uid, const char* imagePath) {
    auto _avatar = this->findAvatar(uid);
    if (nullptr == _avatar) return;
    _avatar->jumpPresent();
    createAndPresentGift(_avatar->getPosition(), imagePath);
}

void RoomManager::receiveChatMessage(const char* uid, const char* content) {
    auto _avatar = this->findAvatar(uid);
    if (nullptr == _avatar) return;
    _avatar->popChatBubble(content);
}

void RoomManager::releaseResource() {

    
}


const Vec2 RoomManager::getStagePosition(int index) const {
    if (index >= _STAGE_BLOCK_COUNT) return Vec2::ZERO;
    float _right_x = _visibleOrigin.x + _visibleSize.width;
    float _target_x = _right_x - (_STAGE_BLOCK_COUNT - index) * _STAGE_BLOCK_WIDTH / _scaleFactor;
    float _target_y = _centerPosition.y - _STAGE_BLOCK_TOP / _scaleFactor;
    return Vec2(_target_x , _target_y);
}

const Vec2 RoomManager::getStandPosition(int index) const {

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

RoomAvatar* RoomManager::findStageAvatar(const char *uid) {
    for (int i = 0; i < _stageAvatars.size(); ++i) {
        auto _avatar = _stageAvatars.at(i);
        if (strcmp(uid, _avatar->getUid()) == 0) return _avatar;
    }
    return nullptr;
}

RoomAvatar* RoomManager::findStandAvatar(const char *uid) {
    for (int i = 0; i < _standAvatars.size(); ++i) {
        auto _avatar = _standAvatars.at(i);
        if (strcmp(uid, _avatar->getUid()) == 0) return _avatar;
    }
    return nullptr;
}

RoomAvatar* RoomManager::findAvatar(const char *uid) {

    auto _avatar = this->findStageAvatar(uid);
    if (nullptr != _avatar) {
        return _avatar;
    } else {
        return this->findStandAvatar(uid);
    }
}


RoomAvatar* RoomManager::createAvatar(int rank, const char* uid, const char* name, const char* path, const Vec2 &pos) {

    auto _avatar = RoomAvatar::create(rank, rank, uid, path, name);
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


RoomAvatar* RoomManager::removeAvatar(const char *uid) {
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

void RoomManager::createAndPresentGift(const Vec2& pos, const char* imagePath) {

//    AudioManager::getInstance()->playBG();

    float start_x = pos.x;
    float start_y = pos.y + 60;

    int index = _giftHolder.size();
    auto gift = RoomGift::create(index, index, "gift/heart.png");
    gift->setPosition(Vec2(start_x, start_y));

    float _coord_x = _GIFT_TABLE_WIDTH / 2;
    float _coord_y = _GIFT_TABLE_HEIGHT / 2;
    float _space_x = 40;
    float _space_y = 0;

    float _rand_x = rand() % (int)((_coord_x - _space_x) * 2) - (_coord_x - _space_x);
    float _rand_y = std::sqrt((1 - _rand_x * _rand_x / _coord_x / _coord_x) * _coord_y * _coord_y);

    float _target_x = _rand_x + _centerPosition.x;
    float _target_y = _centerPosition.y - (_GIFT_TABLE_TOP + rand() % (int)((_rand_y - _space_y) * 2) - (_rand_y - _space_y));

    gift->present(Vec2(_target_x, _target_y));
    if (_scene) {
        _scene->addChild(gift, 10000);
    }
    _giftHolder.pushBack(gift);
    limitGiftHolderSize();
}


void RoomManager::limitGiftHolderSize() {
    int _length = _giftHolder.size();
    int _limit = 200;
    if(_length > _limit * 2) {
        auto _removeList = Vector<RoomGift*>(_limit);

        for (int i = 0; i < _length; ++i) {
            if (i < _limit) {
                _removeList.pushBack(_giftHolder.at(i));
            } else {
                break;
            }
        }

        for (int i = 0; i < _limit; ++i) {
            auto _gift = dynamic_cast<RoomGift*>(_removeList.at(i));

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

void RoomManager::reorganizeStageAvatars() {
    for (int i = 0; i < _stageAvatars.size(); ++i) {
        auto _position = getStagePosition(i);
        if (_position.isZero()) continue;
        auto _avatar = _stageAvatars.at(i);
        _avatar->jumpTo(_position);
    }
}

void RoomManager::reorganizeStandAvatars() {
    for (int i = 0; i < _standAvatars.size(); ++i) {
        auto _position = getStandPosition(i);
        if (_position.isZero()) continue;
        auto _avatar = _standAvatars.at(i);
        _avatar->jumpTo(_position);
    }
}
