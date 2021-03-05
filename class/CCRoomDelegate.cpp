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

static int _big_gift_cursor = 0;

static CCRoomDelegate *instance = NULL;

CCRoomDelegate *CCRoomDelegate::getInstance() {
    if (!instance) {
        instance = new CCRoomDelegate();
    }
    return instance;
}

CCRoomDelegate::~CCRoomDelegate() {
    releaseResource();
}

void CCRoomDelegate::init() {
    if (!this->_is_released) {
        log("delegate: exec last release.");
        releaseResource();
    }
    this->_is_released = false;
    this->_is_attached = false;
    log("delegate: init.");

    _big_gift_cursor = 0;
    std::random_shuffle(this->_randomPosition.begin(), this->_randomPosition.end());
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
    if (!_s_giftCache.empty() || !_m_giftCache.empty() || !_b_giftCache.empty()) {
        tryPresentCacheGift();
        log("delegate:resumeFromCache->tryPresentCacheGift");
    }

    if (!_clearGift.empty()) {
        tryExecuteClearGift();
        log("delegate:resumeFromCache->tryExecuteClearGift");
    }

    if (!_targetCache.empty()) {
        tryRefreshCacheAvatar();
        log("delegate:resumeFromCache->tryRefreshCacheAvatar");
    }
}

void CCRoomDelegate::pause() {
    Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
    cacheWillPresentGift();
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
            _scene->addChild(_stage_background, -2, _TAG_STAGE_BACKGROUND);
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

    Vector<CCGameGift*> _new_s_gifts;
    Vector<CCGameGift*> _new_m_gifts;
    Vector<CCGameGift*> _new_b_gifts;
    for (int i = 0; i < _data_arr.Size(); ++i) {
        rapidjson::Value &_value = _data_arr[i];

        const char *_urls = cocostudio::DICTOOL->getStringValue_json(_value, "urls", "");
        int _count = cocostudio::DICTOOL->getIntValue_json(_value, "count");
        int _type = cocostudio::DICTOOL->getIntValue_json(_value, "type");

        Vector<CCGameGift*>& _new_gifts = _new_s_gifts;
        if (_type == CCGameGift::_GIFT_TYPE_SMALL) {
            _new_gifts = _new_s_gifts;
        } else if (_type == CCGameGift::_GIFT_TYPE_MIDDLE) {
            _new_gifts = _new_m_gifts;
        } else if (_type == CCGameGift::_GIFT_TYPE_BIGGER) {
            _new_gifts = _new_b_gifts;
        } else {
            break;
        }

        for (int i = 0; i < _count; ++i) {
            if (_new_s_gifts.size() >= _S_GIFT_HOLDER_SIZE
                || _new_m_gifts.size() >= _M_GIFT_HOLDER_SIZE
                || _new_b_gifts.size() >= _B_GIFT_HOLDER_SIZE) {
                break;
            }

            auto _gift = CCGameGift::create(i, i, _type, _urls);
            _gift->setPosition(getGiftPosition(_type));

            if (_scene) {
                _scene->addChild(_gift, _type == CCGameGift::_GIFT_TYPE_BIGGER ? -1 :0);
            }
            _new_gifts.pushBack(_gift);

        }
    }

    _s_giftHolder.pushBack(_new_s_gifts);
    _new_s_gifts.clear();

    _m_giftHolder.pushBack(_new_m_gifts);
    _new_m_gifts.clear();

    _b_giftHolder.pushBack(_new_b_gifts);
    _new_b_gifts.clear();

    limitAllGiftHolder();
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
    const char *_url = cocostudio::DICTOOL->getStringValue_json(_value, "url", "");
    const char *_uid = cocostudio::DICTOOL->getStringValue_json(_value, "uid", "");
    const char *_name = cocostudio::DICTOOL->getStringValue_json(_value, "name", "");

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
        const char *_url = cocostudio::DICTOOL->getStringValue_json(_value, "url", "");
        const char *_uid = cocostudio::DICTOOL->getStringValue_json(_value, "uid", "");
        const char *_name = cocostudio::DICTOOL->getStringValue_json(_value, "name", "");

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

        const char *_url = cocostudio::DICTOOL->getStringValue_json(_value, "url", "");
        const char *_uid = cocostudio::DICTOOL->getStringValue_json(_value, "uid", "");
        const char *_name = cocostudio::DICTOOL->getStringValue_json(_value, "name", "");
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

void CCRoomDelegate::clearTargetGiftPool(int type) {
    if (CCGameGift::isIllegalGiftType(type)) return;
    if (isApplicationReleased("clearTargetGiftPool")) return;
    if (isApplicationInvalid("clearTargetGiftPool")) {
        _clearGift.push_back(type);
        return;
    }

    getGiftCache(type).clear();
    getGiftScheduleMap(type).clear();
    auto& _gift_holder = getGiftHolder(type);

    for (int i = 0; i < _gift_holder.size(); ++i) {
        auto _gift = dynamic_cast<CCGameGift*>(_gift_holder.at(i));

        auto _removeFunc = CallFunc::create([_gift](){
            _gift->removeFromParentAndCleanup(true);
        });
        auto _action = Spawn::createWithTwoActions(
                JumpBy::create(0.6f, Vec2(0, 0), 80  / _scaleFactor, 1),
                FadeOut::create(0.5f)
        );
        auto _sequence = Sequence::create(_action, _removeFunc, nullptr);
        _gift->runAction(_sequence);
    }

    _gift_holder.clear();
}

void CCRoomDelegate::tryExecuteClearGift() {
    for (int i = 0; i < _clearGift.size(); ++i) {
        clearTargetGiftPool(_clearGift.at(i));
    }
    _clearGift.clear();
}

void CCRoomDelegate::receiveGiftMessage(const char *json) {
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
    const char *_urls = cocostudio::DICTOOL->getStringValue_json(_value, "urls", "");
    const char *_uid = cocostudio::DICTOOL->getStringValue_json(_value, "uid", "");
    const char *_big = cocostudio::DICTOOL->getStringValue_json(_value, "bigUrl", "");
    int _count = cocostudio::DICTOOL->getIntValue_json(_value, "count");
    int _type = cocostudio::DICTOOL->getIntValue_json(_value, "type");

    std::string _big_url(_big);

    if (CCGameGift::isIllegalGiftType(_type)) return;

    if (isApplicationReleased("receiveGiftMessage")) return;
    if (isApplicationInvalid("receiveGiftMessage")) {
        for (int i = 0; i < _count; ++i) {
            cacheBackPresentGift(_type, _urls);
        }
        if (!_big_url.empty()) {
            cacheBackPresentGift(CCGameGift::_GIFT_TYPE_BIGGER, _big_url.c_str());
        }
        return;
    }

    float _interval = 60.0 / 1000.0;
    scheduleHandleGift(_type, _uid, _urls, _count, 0.0, _interval);
    if (!_big_url.empty()) {
        scheduleHandleGift(CCGameGift::_GIFT_TYPE_BIGGER, _uid, _big_url.c_str(), 1, _interval * _count, _interval);
    }

}

void CCRoomDelegate::scheduleHandleGift(int type, const char *uid, const char *urls, int count, float delay, float interval) {
    double _current_ms = cocos2d::utils::gettime();
    std::string _schedule_key("present_gift_bundle_");
    _schedule_key.append(to_string(_current_ms));

    auto& _scheduleMap = getGiftScheduleMap(type);

    _scheduleMap[_schedule_key];
    for (int i = 0; i < count; ++i) {
        _scheduleMap[_schedule_key].push_back(urls);
    }

    std::string _uid_str(uid);

    log("delegate:receiveGiftMessage, scheduleKey:%s, size:%d", _schedule_key.c_str(), count);
    std::string _urls_str(urls);
    Director::getInstance()->getScheduler()->schedule(
            CC_CALLBACK_0(CCRoomDelegate::schedulePresentGift, this, type, _schedule_key, _uid_str, _urls_str),
            this, interval, MAX(count - 1, 0), delay, false, _schedule_key);


}

void CCRoomDelegate::schedulePresentGift(int type, const std::string &key, const std::string &uid, const std::string &urls) {

    auto& _scheduleMap = getGiftScheduleMap(type);
    std::map<std::string, std::vector<std::string>>::iterator _iterator;
    _iterator = _scheduleMap.find(key.c_str());
    if (_iterator != _scheduleMap.end())  {
        std::vector<std::string>& _urls = _scheduleMap[key];
        if (!_urls.empty()) {
            _urls.pop_back();
        }
        if(_urls.empty()) {
            _scheduleMap.erase(key);
        }
    }

    if (isApplicationReleased("receiveGiftMessage")) return;
    if (isApplicationInvalid("receiveGiftMessage")) {
        cacheBackPresentGift(type, urls.c_str());
        return;
    }


    auto _avatar = this->findAvatar(uid.c_str());
    if (nullptr != _avatar && _avatar->offline != 1) {
        _avatar->jumpByPresent();
        createAndPresentGift(type, _avatar->getPosition(), urls.c_str());
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

        createAndPresentGift(type,this->getNonePosition(_where), urls.c_str());
    }


    limitTargetGiftHolder(type, 0);
}

void CCRoomDelegate::cacheBackPresentGift(int type, const char *urls) {
    if (type == CCGameGift::_GIFT_TYPE_SMALL) {
        if (_s_giftCache.size() >= _S_GIFT_HOLDER_SIZE) {
            _s_giftCache.erase(_s_giftCache.begin());
        }
        _s_giftCache.push_back(urls);
    } else if (type == CCGameGift::_GIFT_TYPE_MIDDLE) {
        if (_m_giftCache.size() >= _M_GIFT_HOLDER_SIZE) {
            _m_giftCache.erase(_m_giftCache.begin());
        }
        _m_giftCache.push_back(urls);
    } else if (type == CCGameGift::_GIFT_TYPE_BIGGER) {
        if (_b_giftCache.size() >= _B_GIFT_HOLDER_SIZE) {
            _b_giftCache.erase(_b_giftCache.begin());
        }
        _b_giftCache.push_back(urls);
    }
}

void CCRoomDelegate::cacheWillPresentGift() {
    cachePresentTargetGift(CCGameGift::_GIFT_TYPE_SMALL);
    cachePresentTargetGift(CCGameGift::_GIFT_TYPE_MIDDLE);
    cachePresentTargetGift(CCGameGift::_GIFT_TYPE_BIGGER);
}

void CCRoomDelegate::cachePresentTargetGift(int type) {
    auto& _schedule_map = getGiftScheduleMap(type);
    log("delegate:cacheWillPresentGift, type:%d, scheduleMap total size:%d", type, _schedule_map.size());
    std::map<std::string, std::vector<std::string>>::iterator _map_iter;
    _map_iter = _schedule_map.begin();
    while(_map_iter != _schedule_map.end()) {
        std::vector<std::string>& _url_vec = _map_iter->second;
        log("delegate:cacheWillPresentGift, scheduleKey:%s, size:%d", (_map_iter->first).c_str(), _url_vec.size());
        if (!_url_vec.empty()) {
            std::vector<std::string>::iterator _vec_iter;
            _vec_iter = _url_vec.begin();
            while(_vec_iter != _url_vec.end()) {
                cacheBackPresentGift(type, _vec_iter->c_str());
                _vec_iter++;
            }
            _url_vec.clear();
        }
        _map_iter++;
    }

    _schedule_map.clear();
}

map<std::string, std::vector<std::string>>& CCRoomDelegate::getGiftScheduleMap(int type) {
    if (type == CCGameGift::_GIFT_TYPE_SMALL) {
        return _s_scheduleMap;
    } else if (type == CCGameGift::_GIFT_TYPE_MIDDLE) {
        return _m_scheduleMap;
    } else if (type == CCGameGift::_GIFT_TYPE_BIGGER) {
        return _b_scheduleMap;
    }
    return _s_scheduleMap;
}

Vector<CCGameGift*>& CCRoomDelegate::getGiftHolder(int type) {
    if (type == CCGameGift::_GIFT_TYPE_SMALL) {
        return _s_giftHolder;
    } else if (type == CCGameGift::_GIFT_TYPE_MIDDLE) {
        return _m_giftHolder;
    } else if (type == CCGameGift::_GIFT_TYPE_BIGGER) {
        return _b_giftHolder;
    }
    return _s_giftHolder;
}

std::vector<std::string> & CCRoomDelegate::getGiftCache(int type) {
    if (type == CCGameGift::_GIFT_TYPE_SMALL) {
        return _s_giftCache;
    } else if (type == CCGameGift::_GIFT_TYPE_MIDDLE) {
        return _m_giftCache;
    } else if (type == CCGameGift::_GIFT_TYPE_BIGGER) {
        return _b_giftCache;
    }
    return _s_giftCache;
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

    _s_giftHolder.clear();
    _m_giftHolder.clear();
    _b_giftHolder.clear();
    _s_giftCache.clear();
    _m_giftCache.clear();
    _b_giftCache.clear();
    _s_scheduleMap.clear();
    _m_scheduleMap.clear();
    _b_scheduleMap.clear();
    _clearGift.clear();

    _standAvatars.clear();
    _stageAvatars.clear();
    _stageSteps.clear();
    _randomWheres.clear();

    _bgCache.clear();
    _selfCache.clear();
    _heapCache.clear();
    _standCache.clear();
    _stageCache.clear();
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

const Vec2 CCRoomDelegate::getGiftPosition(int type) const {

    if (type == CCGameGift::_GIFT_TYPE_BIGGER) {
        int _index = _big_gift_cursor % _B_GIFT_HOLDER_SIZE;
        _big_gift_cursor = (_big_gift_cursor + 1) % _B_GIFT_HOLDER_SIZE;

        auto _position =_randomPosition[_index];
        return Vec2(_position.x , _centerPosition.y - _position.y);
    }

    float _coord_x = _GIFT_TABLE_WIDTH / _scaleFactor / 2;
    float _coord_y = _GIFT_TABLE_HEIGHT_MAX / _scaleFactor / 2;
    float _space_x = 20 / _scaleFactor;
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

void CCRoomDelegate::createAndPresentGift(int type, const Vec2& pos, const char* urls) {

//    AudioManager::getInstance()->playBG();

    float start_x = pos.x;
    float start_y = pos.y + 60 / _scaleFactor;


    auto& _gift_holder = getGiftHolder(type);

    int _gift_index = _gift_holder.size();
    auto _gift = CCGameGift::create(_gift_index, _gift_index, type, urls);
    _gift->setPosition(Vec2(start_x, start_y));

    auto _position = getGiftPosition(type);

    _gift->present(_position);
    if (_scene) {
        _scene->addChild(_gift, 399);
    }
    _gift_holder.pushBack(_gift);
}

void CCRoomDelegate::tryPresentCacheGift() {
    presentTargetGift(CCGameGift::_GIFT_TYPE_SMALL);
    presentTargetGift(CCGameGift::_GIFT_TYPE_MIDDLE);
    presentTargetGift(CCGameGift::_GIFT_TYPE_BIGGER);
    limitAllGiftHolder();
}

void CCRoomDelegate::presentTargetGift(int type) {
    auto& _gift_cache = getGiftCache(type);
    auto& _gift_holder = getGiftHolder(type);

    for (int i = 0; i < _gift_cache.size(); ++i) {
        int _gift_index = _gift_cache.size();
        auto _cache_gift = CCGameGift::create(_gift_index, _gift_index, type, _gift_cache.at(i));
        _cache_gift->setPosition(getGiftPosition(type));
        if (_scene) {
            _scene->addChild(_cache_gift, type == CCGameGift::_GIFT_TYPE_BIGGER ? -1 : 0);
        }
        _gift_holder.pushBack(_cache_gift);
    }
    _gift_cache.clear();
}

int CCRoomDelegate::getLimitHolderSize(int type) {
    if (type == CCGameGift::_GIFT_TYPE_SMALL) {
        return _S_GIFT_HOLDER_SIZE;
    } else if (type == CCGameGift::_GIFT_TYPE_MIDDLE) {
        return _M_GIFT_HOLDER_SIZE;
    } else if (type == CCGameGift::_GIFT_TYPE_BIGGER) {
        return _B_GIFT_HOLDER_SIZE;
    }
    return 0;
}

void CCRoomDelegate::limitAllGiftHolder() {
    limitTargetGiftHolder(CCGameGift::_GIFT_TYPE_SMALL, 0);
    limitTargetGiftHolder(CCGameGift::_GIFT_TYPE_MIDDLE, 0);
    limitTargetGiftHolder(CCGameGift::_GIFT_TYPE_BIGGER, 0);
}

void CCRoomDelegate::limitTargetGiftHolder(int type, int count) {
    auto& _gift_holder = getGiftHolder(type);
    int _total_size = _gift_holder.size();
    int _limit_size = getLimitHolderSize(type);

    int _limit_real = MIN((_total_size + count - _limit_size), _total_size);
    if (_limit_real > 0) {

        auto _remove_list = Vector<CCGameGift*>(_limit_real);
        for (int i = 0; i < _limit_real; ++i) {
            _remove_list.pushBack(_gift_holder.at(i));
        }

        for (int i = 0; i < _limit_real; ++i) {
            auto _gift = dynamic_cast<CCGameGift*>(_remove_list.at(i));

            auto _removeFunc = CallFunc::create([_gift](){
                _gift->removeFromParentAndCleanup(true);
            });
            auto _action = Spawn::createWithTwoActions(
                    JumpBy::create(0.6f, Vec2(0, 0), 80  / _scaleFactor, 1),
                    FadeOut::create(0.5f)
            );
            auto _sequence = Sequence::create(_action, _removeFunc, nullptr);
            _gift->runAction(_sequence);
            _gift_holder.eraseObject(_gift);
        }
        _remove_list.clear();
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
    const char *_url = cocostudio::DICTOOL->getStringValue_json(_value, "url", "");
    const char *_uid = cocostudio::DICTOOL->getStringValue_json(_value, "uid", "");
    const char *_name = cocostudio::DICTOOL->getStringValue_json(_value, "name", "");

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

