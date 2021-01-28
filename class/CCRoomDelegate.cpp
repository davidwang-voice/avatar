//
// Created by David on 2020/10/3.
//

#include <cmath>
#include <utility>

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <cocos/editor-support/cocostudio/DictionaryHelper.h>
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
//#include "DictionaryHelper.h"
#include <cocos/editor-support/cocostudio/DictionaryHelper.h>
#endif

#include "AppDelegate.h"
#include "CCRoomDelegate.h"
#include "AudioManager.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include <cocos/editor-support/spine/extension.h>

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

void CCRoomDelegate::init() {
    if (!this->_is_released) {
        log("delegate: exec last release.");
        releaseResource();
    }
    this->_is_released = false;
    this->_is_attached = false;
    log("delegate: init.");
}

void CCRoomDelegate::attachScene(Scene* scene) {
    log("delegate: attach scene.");
    this->_is_attached = true;
    this->_scene = scene;
    this->_visibleOrigin = Director::getInstance()->getVisibleOrigin();
    this->_visibleSize = Director::getInstance()->getVisibleSize();
    this->_scaleFactor = Director::getInstance()->getContentScaleFactor();
    this->_centerPosition = Vec2(_visibleOrigin.x + _visibleSize.width / 2, _visibleOrigin.y + _visibleSize.height);

//    auto size = Director::getInstance()->getOpenGLView()->getFrameSize();


    for (int i = 0; i < _STAND_MAX_ROW_COUNT; ++i) {
        if (i == 0) {
            _standRowCount[0] = _STAND_MAX_COLUMN_COUNT;
        } else {
            _standRowCount[i] = _standRowCount[i - 1] + _STAND_MAX_COLUMN_COUNT;
        }
    }
    this->resumeFromCache();
}

void CCRoomDelegate::ensureStageSteps() {
    if (_stageSteps.empty()) {
        for (int i = 0; i < _STAGE_BLOCK_COUNT; ++i) {
            auto _stage_step = CCGameStep::create(i, i, "");
            auto _stage_pos = this->getStepPosition(i);
            _stage_step->setPosition(Vec2(_stage_pos.x, _stage_pos.y));
            if (_scene) {
                _scene->addChild(_stage_step, 1);
            }
            _stageSteps.pushBack(_stage_step);
        }
    }
}

void CCRoomDelegate::resumeFromCache() {
    if (isApplicationInvalid("delegate:resumeFromCache by Foreground or Attach")) {
        return;
    }
    if (!_bgCache.empty()) {
        string json(_bgCache.c_str());
        setStageBackground(json.c_str());
        log("delegate:resumeFromCache->setStageBackground");
    }
    if (!_heapCache.empty()) {
        string json(_heapCache.c_str());
        setupStageGiftHeap(json.c_str());
        log("delegate:resumeFromCache->setupStageGiftHeap");
    }
    if (!_selfCache.empty()) {
        string json(_selfCache.c_str());
        updateSelfAvatar(json.c_str());
        log("delegate:resumeFromCache->updateSelfAvatar");
    }
    if (!_standCache.empty()) {
        string json(_standCache.c_str());
        updateStandAvatars(json.c_str());
        log("delegate:resumeFromCache->updateStandAvatars");
    }
    if (!_stageCache.empty()) {
        string json(_stageCache.c_str());
        updateStageAvatars(json.c_str());
        log("delegate:resumeFromCache->updateStageAvatars");
    }
    if (!_giftCache.empty()) {
        tryPresentCacheGift();
        log("delegate:resumeFromCache->tryPresentCacheGift");
    }

    if (!_targetCache.empty()) {
        tryRefreshCacheAvatar();
        log("delegate:resumeFromCache->tryRefreshCacheAvatar");
    }
}

void CCRoomDelegate::setStageBackground(const char *url) {
    if (isApplicationReleased("setStageBackground")) return;
    if (isApplicationInvalid("setStageBackground")) {
        _bgCache = url;

        if (_bgCache.empty()) {
            _bgCache = "placeholder";
        }

        return;
    }
    _bgCache.clear();


    CCBaseSprite *_stage_background = nullptr;
    if (_scene) {
        auto _child = _scene->getChildByTag(_TAG_STAGE_BACKGROUND);
        _stage_background = dynamic_cast<CCBaseSprite *>(_child);

        if (nullptr == _stage_background) {
            _stage_background = CCBaseSprite::create();
            _scene->addChild(_stage_background, -1, _TAG_STAGE_BACKGROUND);
        }
    }

    if (nullptr == _stage_background) {
        return;
    }

    if (strcmp("placeholder", url) == 0) {
        _stage_background->loadTexture("", "cocos/bg_game_room.png");
    } else {
        _stage_background->loadTexture(url, "cocos/bg_game_room.png");
    }

    float _target_x = _centerPosition.x;
    float _target_y = _centerPosition.y - _stage_background->getContentSize().height / 2;
    _stage_background->setPosition(Vec2(_target_x, _target_y));

    ensureStageSteps();
}

void CCRoomDelegate::setupStageGiftHeap(const char *json) {
    if (isApplicationReleased("setupStageGiftHeap")) return;
    if (isApplicationInvalid("setupStageGiftHeap")) {
        _heapCache = json;
        return;
    }
    _heapCache.clear();

    rapidjson::Document _document;
    _document.Parse<rapidjson::kParseDefaultFlags>(json);
    if (_document.HasParseError()) {
        log("parse stage gift json error %d\n", _document.GetParseError());
        return;
    }
    if (!_document.IsArray()) {
        log("gift json is not array %s\n", json);
        return;
    }
    rapidjson::Value& _data_arr = _document;

    Vector<CCGameGift*> _new_game_gifts;

    for (int i = 0; i < _data_arr.Size(); ++i) {
        rapidjson::Value &_value = _data_arr[i];

        const char *_url = cocostudio::DICTOOL->getStringValue_json(_value, "url");
        int count = cocostudio::DICTOOL->getIntValue_json(_value, "count");

        for (int i = 0; i < count; ++i) {
            if (_new_game_gifts.size() >= _GIFT_HOLDER_SIZE * 2) {
                break;
            }

            auto gift = CCGameGift::create(i, i, _url);
            gift->setPosition(getGiftPosition());

            if (_scene) {
                _scene->addChild(gift, 0);
            }
            _new_game_gifts.pushBack(gift);

        }
    }

    _new_game_gifts.pushBack(_giftHolder);
    _giftHolder.clear();
    _giftHolder.pushBack(_new_game_gifts);
    limitGiftHolderSize();
}

void CCRoomDelegate::updateSelfAvatar(const char *json) {
    if (isApplicationReleased("updateSelfAvatar")) return;
    if (isApplicationInvalid("updateSelfAvatar")) {
        _selfCache = json;
        return;
    }
    _selfCache.clear();

    rapidjson::Document _document;
    _document.Parse<rapidjson::kParseDefaultFlags>(json);
    if (_document.HasParseError()) {
        log("parse self avatar json error %d\n", _document.GetParseError());
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
    int _guard = cocostudio::DICTOOL->getIntValue_json(_value, "guard");


    CCGameAvatar* _self_avatar = nullptr;

    if (_scene) {
        auto _child = _scene->getChildByTag(_TAG_SELF_AVATAR);
        _self_avatar = dynamic_cast<CCGameAvatar*>(_child);
    }

    if (nullptr == _self_avatar) {
        _self_avatar = CCGameAvatar::create(_RANK_SELF_DEFAULT, _RANK_SELF_DEFAULT, _uid, _url, _name);

        auto _self_position = this->getSelfPosition();
        _self_avatar->setPosition(Vec2(_visibleOrigin.x - _self_avatar->getContentSize().width, _self_position.y));
        if (_scene) {
            _scene->addChild(_self_avatar, _RANK_SELF_DEFAULT, _TAG_SELF_AVATAR);
        }
        _self_avatar->jumpToPosition(_self_position);
    }

    _self_avatar->setUid(_uid);
    _self_avatar->updateElement(_name, _url, _rare, _guard);
    _self_avatar->updateRank(_RANK_SELF_DEFAULT);


    if (_scene) {
        auto _child = _scene->getChildByTag(_TAG_SELF_APERTURE);

        auto _position = _self_avatar->getCenterPosition();
        if (nullptr == _child) {
            auto _aperture = CCBaseSprite::create();
            _aperture->setAnchorPoint(Point::ANCHOR_MIDDLE);
            _aperture->setTexture("cocos/aperture.png");

            _aperture->setPosition(_position);
            _scene->addChild(_aperture, _RANK_SELF_DEFAULT - 1, _TAG_SELF_APERTURE);
        } else {
            _child->setPosition(_position);
        }
    }
}

void CCRoomDelegate::updateTargetAvatar(const char *json) {
    if (isApplicationReleased("updateTargetAvatar")) return;
    if (isApplicationInvalid("updateTargetAvatar")) {
        if (_targetCache.size() > 20) {
            _targetCache.erase(_targetCache.begin());
        }
        _targetCache.push_back(json);
        return;
    }
    refreshTargetAvatar(json);
}

void CCRoomDelegate::updateStageAvatars(const char* json) {
    if (isApplicationReleased("updateStageAvatars")) return;
    if (isApplicationInvalid("updateStageAvatars")) {
        _stageCache = json;
        return;
    }
    _stageCache.clear();
    ensureStageSteps();
    rapidjson::Document _document;
    _document.Parse<rapidjson::kParseDefaultFlags>(json);
    if (_document.HasParseError()) {
        log("parse stage avatar json error %d\n", _document.GetParseError());
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
        int _guard = cocostudio::DICTOOL->getIntValue_json(_value, "guard");

        bool _mute = cocostudio::DICTOOL->getBooleanValue_json(_value, "mute");


        std::string _uid_str(_uid);

        if (!_uid_str.empty()) {

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
            _new_stage_avatar->stageIndex = i;
            _standAvatars.eraseObject(_new_stage_avatar);
            _new_stage_avatar->updateElement(_name, _url, _rare, _guard);

        }

        auto _stageStep = _stageSteps.at(i);
        _stageStep->setUid(_uid);
        _stageStep->setMute(_mute);
    }

    for (int i = 0; i < _stageAvatars.size(); ++i) {
        auto _avatar = _stageAvatars.at(i);
        if (_avatar->getTag() == _TAG_SELF_AVATAR) continue;
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
    if (isApplicationReleased("updateStandAvatars")) return;
    if (isApplicationInvalid("updateStandAvatars")) {
        _standCache = json;
        return;
    }
    _standCache.clear();
    rapidjson::Document _document;
    _document.Parse<rapidjson::kParseDefaultFlags>(json);
    if (_document.HasParseError()) {
        log("parse stand avatar json error %d\n", _document.GetParseError());
        return;
    }
    if (!_document.IsArray()) {
        log("stand json is not array %s\n", json);
        return;
    }

    rapidjson::Value& _data_arr = _document;

    int _index_offset = 0;
    Vector<CCGameAvatar*> _new_stand_avatars;
    for (int i = 0; i < _data_arr.Size(); ++i) {
//        auto _position = getStandPosition(i);
//        if (_position.isZero()) continue;

        rapidjson::Value& _value = _data_arr[i];

        const char *_url = cocostudio::DICTOOL->getStringValue_json(_value, "url");
        const char *_uid = cocostudio::DICTOOL->getStringValue_json(_value, "uid");
        const char *_name = cocostudio::DICTOOL->getStringValue_json(_value, "name");
        int _rare = cocostudio::DICTOOL->getIntValue_json(_value, "rare");
        int _guard = cocostudio::DICTOOL->getIntValue_json(_value, "guard");
        int _offline = cocostudio::DICTOOL->getIntValue_json(_value, "offline");

        auto _cur_self_avatar = this->findSelfAvatar(_uid);


        std::string _uid_str(_uid);
        if (_uid_str.empty()) continue;

        int _index_real = i;
        if (_offline == 1 && _index_offset == 0) {
            if (_index_real < _STAND_MAX_COLUMN_COUNT) {
                _index_offset = _STAND_MAX_COLUMN_COUNT - _index_real;
            }
        }

        if (_index_offset > 0) {
            _index_real = _index_real + _index_offset;
        }

        auto _position = this->getStandPosition(_index_real);
        if (_position.isZero()) continue;



        if (nullptr != _cur_self_avatar && !_cur_self_avatar->isOnStage) {
            _new_stand_avatars.pushBack(_cur_self_avatar);
        } else if ( auto _old_stand_avatar = this->findStandAvatar(_uid)) {
            _new_stand_avatars.pushBack(_old_stand_avatar);
        } else {
            auto _new_stand_avatar = createAvatar(_index_real + 1, _uid, _name, _url, _position);
            _new_stand_avatars.pushBack(_new_stand_avatar);
        }

        _new_stand_avatars.back()->updateElement(_name, _url, _rare, _guard, _offline);
    }


    for (int i = 0; i < _standAvatars.size(); ++i) {
        auto _avatar = _standAvatars.at(i);
        if (_avatar->getTag() == _TAG_SELF_AVATAR) continue;
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
    std::string _uid_str(uid);
    if (_uid_str.empty()) return;

    if (isApplicationReleased("receiveGiftMessage")) return;
    if (isApplicationInvalid("receiveGiftMessage")) {
        if (_giftCache.size() >= _GIFT_HOLDER_SIZE * 2) {
            _giftCache.erase(_giftCache.begin());
        }

        _giftCache.push_back(url);
        return;
    }

    auto _avatar = this->findAvatar(uid);

    if (nullptr != _avatar && _avatar->offline != 1) {
        _avatar->jumpByPresent();
        createAndPresentGift(_avatar->getPosition(), url);
    } else {

        if (_randomWheres.size() >= 10) {
            _randomWheres.erase(_randomWheres.begin());
        }

        unsigned int _where;
        map<std::string, unsigned int>::iterator _iterator;
        _iterator = _randomWheres.find(uid);
        if (_iterator != _randomWheres.end())  {
            _where = _iterator->second;
        } else {
            _where = cocos2d::RandomHelper::random_int(1, 2);
            _randomWheres.insert(pair<std::string, unsigned int>(uid, _where));
        }

        createAndPresentGift(this->getNonePosition(_where), url);
    }
}

void CCRoomDelegate::receiveChatMessage(const char* uid, const char* content) {
    if (isApplicationReleased("receiveChatMessage")) return;
    if (isApplicationInvalid("receiveChatMessage")) return;
    auto _avatar = this->findAvatar(uid);
    if (nullptr == _avatar || _avatar->offline == 1) return;

    _avatar->popChatBubble(content);
}

void CCRoomDelegate::receiveVoiceWave(const char *uids) {
    if (isApplicationReleased("receiveVoiceWave")) return;
    if (isApplicationInvalid("receiveVoiceWave")) return;
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

void CCRoomDelegate::receiveRandomSnore(const char *uids) {
    if (isApplicationReleased("receiveRandomSnore")) return;
    if (isApplicationInvalid("receiveRandomSnore")) return;
    std::vector<string> _uid_arr;
    std::string _raw = uids, _tmp;
    std::stringstream input(_raw);
    while (getline(input, _tmp, ',')) _uid_arr.push_back(_tmp);

    for (int i = 0; i < _uid_arr.size(); ++i) {
        auto _avatar = findAvatar(_uid_arr.at(i).c_str());
        if (nullptr != _avatar) {
            _avatar->runSnoreAnim();
        }
    }
}

void CCRoomDelegate::releaseResource() {
    log("delegate: release resource.");
    _is_released = true;
    _is_attached = false;

    _giftHolder.clear();
    _standAvatars.clear();
    _stageAvatars.clear();
    _stageSteps.clear();
    _randomWheres.clear();

    _bgCache.clear();
    _selfCache.clear();
    _heapCache.clear();
    _standCache.clear();
    _stageCache.clear();
    _giftCache.clear();
    _targetCache.clear();

    if (_scene) {
        _scene->removeAllChildren();
    }
    releaseGLProgramState();


    Director::getInstance()->purgeCachedData();
    Director::getInstance()->purgeDirector();
    CCImageLoader::getInstance()->destroyInstance();
}



const Vec2 CCRoomDelegate::getStepPosition(int index) const {
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
    int _column = (_row == 0 ? index : index - _standRowCount[_row - 1]);

    int _count = _row == 0 ? _standRowCount[0] : _standRowCount[_row] - _standRowCount[_row - 1];
    float _block_w = (float) _visibleSize.width / _count;

    float _refer_x = _centerPosition.x;
    float _refer_y = _centerPosition.y - _STAND_FRONT_ROW_TOP / _scaleFactor -
            (_row == 0 ? 0 : (_STAND_FRONT_ROW_HEIGHT + (_row - 1) * _STAND_ROW_HEIGHT) / _scaleFactor);
    float _arc_seg = _STAND_ARC_HEIGHT / (_count / 2) / _scaleFactor;


    int _direct_x = (float) pow(-1, _column % 2);
    float _offset_x = _count % 2 == 0 ? _block_w / 2 : 0;
    float _target_x = _refer_x + (_offset_x + ((_count % 2 + _column) / 2) * _block_w) * _direct_x;


    float _coord_x = _STAGE_TABLE_WIDTH / _scaleFactor / 2;
    float _coord_y = _STAGE_TABLE_HEIGHT / _scaleFactor / 2;
    float _result_x = _target_x - _refer_x;
    float _result_y = std::sqrt((1 - _result_x * _result_x / _coord_x / _coord_x) * _coord_y * _coord_y);

    float _target_y = _refer_y + (_coord_y - _result_y) + 10 / _scaleFactor;

    return Vec2(_target_x , _target_y);
}

const Vec2 CCRoomDelegate::getSelfPosition() const {
    return Vec2(_centerPosition.x ,  _SELF_GROUND_BOTTOM / _scaleFactor);
}

const Vec2 CCRoomDelegate::getGiftPosition() const {
    float _coord_x = _GIFT_TABLE_WIDTH / _scaleFactor / 2;
    float _coord_y = _GIFT_TABLE_HEIGHT_MAX / _scaleFactor / 2;
    float _space_x = 40 / _scaleFactor;
    float _space_y = 20 / _scaleFactor;

    float _rand_x = RandomHelper::random_real(- (_coord_x - _space_x), (_coord_x - _space_x));
    float _result_y = std::sqrt((1 - _rand_x * _rand_x / _coord_x / _coord_x) * _coord_y * _coord_y);

    float _rand_y = RandomHelper::random_real(- (_result_y - _space_y), (_result_y));
//    float _rand_y = rand() % (int)((_result_y) * 2) - (_result_y);
//    float _coord_y_l = _GIFT_TABLE_HEIGHT_MIN / _scaleFactor / 2;
//    float _result_y_l = std::sqrt((1 - _rand_x * _rand_x / _coord_x / _coord_x) * _coord_y_l * _coord_y_l);

    float _target_x = _rand_x + _centerPosition.x;
    float _target_y = _centerPosition.y - (_GIFT_TABLE_TOP / _scaleFactor + _rand_y);

    return Vec2(_target_x , _target_y);
}

const Vec2 CCRoomDelegate::getNonePosition(unsigned int where) const {
//    int _where = cocos2d::RandomHelper::random_int(1, 2);
    float _target_x = - _NONE_SPACE_X / _scaleFactor;
    if (where == 2) {
        _target_x =  _visibleSize.width +  _NONE_SPACE_X / _scaleFactor;
    }
    float _target_y = MAX((_centerPosition.y - _NONE_SPACE_Y / _scaleFactor), 100 / _scaleFactor);
    return Vec2(_target_x , _target_y);
}


CCGameAvatar* CCRoomDelegate::findStageAvatar(const char *uid) {

    std::string _uid_str(uid);
    if (_uid_str.empty()) return nullptr;

    for (int i = 0; i < _stageAvatars.size(); ++i) {
        auto _avatar = _stageAvatars.at(i);
        if (strcmp(uid, _avatar->getUid()) == 0) return _avatar;
    }
    return nullptr;
}

CCGameAvatar* CCRoomDelegate::findStandAvatar(const char *uid) {
    std::string _uid_str(uid);
    if (_uid_str.empty()) return nullptr;

    for (int i = 0; i < _standAvatars.size(); ++i) {
        auto _avatar = _standAvatars.at(i);
        if (strcmp(uid, _avatar->getUid()) == 0) return _avatar;
    }
    return nullptr;
}

CCGameAvatar* CCRoomDelegate::findSelfAvatar(const char *uid) {

    std::string _uid_str(uid);
    if (_uid_str.empty()) return nullptr;

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

    std::string _uid_str(uid);
    if (_uid_str.empty()) return nullptr;

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
    float start_y = pos.y + 60 / _scaleFactor;

    int _gift_index = _giftHolder.size();
    auto _gift = CCGameGift::create(_gift_index, _gift_index, url);
    _gift->setPosition(Vec2(start_x, start_y));

    auto _position = getGiftPosition();

    _gift->present(_position);
    if (_scene) {
        _scene->addChild(_gift, 399);
    }
    _giftHolder.pushBack(_gift);

//    int _star_index = cocos2d::RandomHelper::random_int(1, 5);
//    std::string _star_path = "gift/star_" + std::to_string(_star_index) + ".png";


    limitGiftHolderSize();
}

void CCRoomDelegate::tryPresentCacheGift() {
    for (int i = 0; i < _giftCache.size(); ++i) {

        int _gift_index = _giftHolder.size();
        auto _cache_gift = CCGameGift::create(_gift_index, _gift_index, _giftCache.at(i));
        _cache_gift->setPosition(getGiftPosition());
        if (_scene) {
            _scene->addChild(_cache_gift, 0);
        }
        _giftHolder.pushBack(_cache_gift);
    }
    _giftCache.clear();
    limitGiftHolderSize();
}


void CCRoomDelegate::limitGiftHolderSize() {
    int _length = _giftHolder.size();
    int _limit = _GIFT_HOLDER_SIZE;
    if(_length > _limit * 2) {
        unsigned int _remove_size = _length - _limit;
        auto _removeList = Vector<CCGameGift*>(_remove_size);

        for (int i = 0; i < _remove_size; ++i) {
            _removeList.pushBack(_giftHolder.at(i));
        }

        for (int i = 0; i < _remove_size; ++i) {
            auto _gift = dynamic_cast<CCGameGift*>(_removeList.at(i));

            auto _removeFunc = CallFunc::create([_gift](){
                _gift->removeFromParentAndCleanup(true);
            });
            auto _action = Spawn::createWithTwoActions(
                    JumpBy::create(0.6f, Vec2(0, 0), 80  / _scaleFactor, 1),
//                    JumpBy::create(0.4, Vec2(0, 100 / _scaleFactor)),
                    FadeOut::create(0.5f)
                    );
            auto _sequence = Sequence::create(_action, _removeFunc, nullptr);
            _gift->runAction(_sequence);
            _giftHolder.eraseObject(_gift);
        }
        _removeList.clear();
    }
}

void CCRoomDelegate::refreshTargetAvatar(const char *json) {
    log("delegate:refreshTargetAvatar json:%s", json);

    rapidjson::Document _document;
    _document.Parse<rapidjson::kParseDefaultFlags>(json);
    if (_document.HasParseError()) {
        log("parse target avatar json error %d\n", _document.GetParseError());
        return;
    }
    if (!_document.IsObject()) {
        log("target json is not object %s\n", json);
        return;
    }
    rapidjson::Value& _value = _document;
    const char *_url = cocostudio::DICTOOL->getStringValue_json(_value, "url");
    const char *_uid = cocostudio::DICTOOL->getStringValue_json(_value, "uid");
    const char *_name = cocostudio::DICTOOL->getStringValue_json(_value, "name");

    int _rare = cocostudio::DICTOOL->getIntValue_json(_value, "rare");
    int _guard = cocostudio::DICTOOL->getIntValue_json(_value, "guard");


    auto _target_avatar = findAvatar(_uid);
    if (_target_avatar) {
        _target_avatar->updateElement(_name, _url, _rare, _guard, _target_avatar->offline);
        _target_avatar->updateRank(_target_avatar->realRanking);
    }
}

void CCRoomDelegate::tryRefreshCacheAvatar() {

    for (int i = 0; i < _targetCache.size(); ++i) {
        refreshTargetAvatar(_targetCache.at(i).c_str());
    }
    _targetCache.clear();
}

void CCRoomDelegate::reorganizeStageAvatars() {
    for (int i = 0; i < _stageAvatars.size(); ++i) {
        auto _stage_avatar = _stageAvatars.at(i);

        auto _position = this->getStagePosition(_stage_avatar->stageIndex);
        if (_position.isZero()) continue;
        _stage_avatar->updateRank(_RANK_STAGE_DEFAULT + i);
        _stage_avatar->jumpToPosition(_position);
        _stage_avatar->isOnStage = true;
    }
}

void CCRoomDelegate::reorganizeStandAvatars() {
    int _index_offset = 0;
    for (int i = 0; i < _standAvatars.size(); ++i) {
        auto _stand_avatar = _standAvatars.at(i);
        int _index_real = i;
        if (_stand_avatar->offline == 1 && _index_offset == 0) {
            if (_index_real < _STAND_MAX_COLUMN_COUNT) {
                _index_offset = _STAND_MAX_COLUMN_COUNT - _index_real;
            }
        }

        if (_index_offset > 0) {
            _index_real = _index_real + _index_offset;
        }
        auto _position = this->getStandPosition(_index_real);
        if (_position.isZero()) continue;

        _stand_avatar->updateRank(_index_real + _RANK_STAND_DEFAULT);
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
                _self_avatar->setOffline(0);
                _self_avatar->jumpToPosition(_position);
                _self_avatar->isOnStage = false;
            }

            auto _aperture = _scene->getChildByTag(_TAG_SELF_APERTURE);
            if (_aperture) {
                _aperture->setPosition(_self_avatar->getCenterPosition());
                int _local_z_order = MAX(_self_avatar->_real_local_z_order - 1, 0);
                _aperture->setLocalZOrder(_local_z_order);
//                _aperture->setVisible(!_self_avatar->isOnStage);
            }


        }
    }
}

bool CCRoomDelegate::isApplicationInvalid(const char* tag) {
    if (!this->_is_attached) {
        log("delegate:%s is ignored, scene is not attached!", tag);
        return true;
    }

    if (Director::getInstance()->isPaused() || !Director::getInstance()->isValid()) {
        log("delegate:%s is cached, game is in background state!", tag);
        return true;
    }

    return false;
}


bool CCRoomDelegate::isApplicationReleased(const char *tag) {
    if (this->_is_released) {
        log("delegate:%s is ignored, game is released!", tag);
        return true;
    }
    return false;
}



void onTouchStageAvatar(const char* uid) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    Java_onTouchStageAvatar(uid);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    OCCallback::getInstance()->onTouchStageAvatar(uid);
#endif
}


void onTouchStandAvatar(const char* uid) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    Java_onTouchStandAvatar(uid);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    OCCallback::getInstance()->onTouchStandAvatar(uid);
#endif
}

void onTouchScene() {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    Java_onTouchScene();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    OCCallback::getInstance()->onTouchScene();
#endif
}

