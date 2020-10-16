//
// Created by David on 2020/10/3.
//

#include <cmath>
#include <utility>
#include "AppDelegate.h"
#include "RoomManager.h"
#include "AudioManager.h"

#include "json/rapidjson.h"
#include "json/document.h"

using namespace rapidjson;



typedef struct {
    int rank;
    char *path;
    char *id;
    char *name;
    bool ssr;
} _Avatar_I;



USING_NS_CC;

static RoomManager *instance = NULL;

RoomManager *RoomManager::getInstance() {
    if (!instance) {
        instance = new RoomManager();
    }
    return instance;
}

void RoomManager::initScene(Scene* scene) {
    this->scene = scene;
    this->visibleOrigin = Director::getInstance()->getVisibleOrigin();
    this->visibleSize = Director::getInstance()->getVisibleSize();
    this->contentFactor = Director::getInstance()->getContentScaleFactor();
    auto centerX = visibleOrigin.x + visibleSize.width/2;
    auto topY = visibleOrigin.y + visibleSize.height;
    this->topMiddlePoint = Vec2(centerX, topY);

    auto size = Director::getInstance()->getOpenGLView()->getFrameSize();
    this->isPortrait = size.width < size.height;

    initRowSeats();
}
//
//VoiceRoomManager::VoiceRoomManager(Scene* scene) {
//    this->scene = scene;
//    this->visibleOrigin = Director::getInstance()->getVisibleOrigin();
//    this->visibleSize = Director::getInstance()->getVisibleSize();
//    this->contentFactor = Director::getInstance()->getContentScaleFactor();
//    auto centerX = visibleOrigin.x + visibleSize.width/2;
//    auto topY = visibleOrigin.y + visibleSize.height;
//    this->topMiddlePoint = Vec2(centerX, topY);
//
//    auto size = Director::getInstance()->getOpenGLView()->getFrameSize();
//    this->isPortrait = size.width < size.height;
//
//    initRowSeats();
//}

RoomManager::~RoomManager() {
    avatarList.clear();
//    map<int, RoomAvatar*>::iterator iter;
//    iter = avatarMap.begin();
//    while (iter != avatarMap.end()) {
//        delete iter->second;
//        iter++;
//    }
//
//    avatarMap.clear();
}

void RoomManager::initRowSeats() {
    int seats = 0;

    // init acc map for portrait
    // 13, 12, 13, 12, 13, 12, 13, 12
    for (int i = 0; i < MAX_ROWS_PORT; i++) {
        int seatsInRow = (i % 2) == 0 ? 13 : 12;
        seats += seatsInRow;
        rowSeatsAccMapPort[i] = seats;
    }

    // init acc map for landscape
    // 13, 22, 23, 22, 20
    rowSeatsAccMapLand[0] = 13;
    rowSeatsAccMapLand[1] = 35;
    rowSeatsAccMapLand[2] = 58;
    rowSeatsAccMapLand[3] = 80;
    rowSeatsAccMapLand[4] = 100;
}


void RoomManager::updateStageAvatars(const char* json) {

    log("updateStageAvatars %s", json);
    
    rapidjson::Document document;
    document.Parse<rapidjson::kParseDefaultFlags>(json);

    if (document.HasParseError()) {

        log("GetParseError %s\n", document.GetParseError());
        return;
    }

    std::vector<_Avatar_I> _avatar_arr;

    if (document.IsArray()) {

        rapidjson::Value& array = document;

        for (int i = 0; i < array.Size(); ++i) {
            rapidjson::Value& _id = array[i];

            _Avatar_I _avatar;
            _avatar.rank = _id["rank"].GetInt();
            _avatar.path = const_cast<char *>(_id["path"].GetString());
            _avatar.id = const_cast<char *>(_id["id"].GetString());
            _avatar.name = const_cast<char *>(_id["name"].GetString());
            _avatar.ssr = _id["ssr"].GetBool();
            _avatar_arr.push_back(_avatar);
        }
    }

    for (int i = 0; i < _avatar_arr.size(); ++i) {
        auto _avatar = _avatar_arr.at(i);
        addAvatar(_avatar.rank, _avatar.rank, _avatar.path, _avatar.name);
    }

}

void RoomManager::updateStandAvatars(const char* json) {

}

void RoomManager::backOffStageAvatar(int id) {

}

void RoomManager::backOffStandAvatar(int id) {

}

void RoomManager::releaseResource() {

}

RoomAvatar* RoomManager::addAvatar(int id, int ranking, const char *imagePath, const char* name) {

    auto avatar = RoomAvatar::create(id, ranking, imagePath, name);

    // setup position
    auto targetPos = getPosition(ranking, isPortrait);
    if (AppDelegate::g_lastOrientation == 1) {
        // entry action, divided into left/right sides
        if (targetPos.x < visibleOrigin.x + visibleSize.width / 2) {
            avatar->setPosition(Vec2(visibleOrigin.x - avatar->getContentSize().width, targetPos.y));
        } else {
            avatar->setPosition(
                    Vec2(visibleOrigin.x + visibleSize.width + avatar->getContentSize().width,
                         targetPos.y));
        }
        avatar->moveTo(targetPos);
    } else if (AppDelegate::g_lastOrientation == 1 && !isPortrait) {
        // portrait to landscape
        log("portrait to landscape");
        auto startPos = getPosition(ranking, !isPortrait);
        avatar->setPosition(startPos);
        avatar->jumpTo(targetPos);
    } else if (AppDelegate::g_lastOrientation == 2 && isPortrait) {
        // landscape to portrait
        log("landscape to portrait");
        auto startPos = getPosition(ranking, !isPortrait);
        avatar->setPosition(startPos);
        avatar->jumpTo(targetPos);
    }

    if (scene) {
        avatarList.pushBack(avatar);
//        avatarList.insert(id - 1, avatar);
//        avatarMap.insert(pair<int, RoomAvatar*>(id, avatar));
        //avatarMap.insert(std::pair<int, VoiceRoomAvatar*>(id, avatar)); // TODO: fix this
        scene->addChild(avatar, 1);
    }
    return avatar;
}

void RoomManager::removeAvatar(int id) {


    auto avatar = findAvatar(id);
    if (nullptr != avatar) {
        avatarList.eraseObject(avatar);
        if(scene) {
            scene->removeChild(avatar);
        }
    }
    reorganizeAvatars();

}

RoomAvatar* RoomManager::findAvatar(int id) {

    int length = avatarList.size();
    for (int i = 0; i < length; ++i) {
        auto avatar = avatarList.at(i);
        if (id == avatar->getId())
            return avatar;
    }

    return nullptr;

//    map<int, RoomAvatar*>::iterator iter;
//    iter = avatarMap.find(id);
//    if (iter != avatarMap.end()) {
//        return iter->second;
//    } else {
//        return nullptr;
//    }
}

void RoomManager::receiveGiftMessage(int id, const char* imagePath) {
    auto avatar = findAvatar(id);
    if (nullptr != avatar) {
        avatar->jumpPresent();
        presentGift(avatar->getPosition(), imagePath);
    }
}

void RoomManager::receiveChatMessage(int id, const char* content) {
    auto avatar = findAvatar(id);

    if (nullptr != avatar) {
        avatar->popChatBubble(content);
    }
}

void RoomManager::presentGift(const Vec2& pos, const char* imagePath) {

    AudioManager::getInstance()->playBG();

    float start_x = pos.x;
    float start_y = pos.y + 60;


    int index = giftList.size();
    auto gift = RoomGift::create(index, index, "gift/heart.png");
    gift->setPosition(Vec2(start_x, start_y));


    float target_x = (rand() % (600)) + topMiddlePoint.x - 300;
    float target_y = (rand() % (30)) + topMiddlePoint.y - RANK_1_MARGIN_TOP/contentFactor + 120;


    gift->present(Vec2(target_x, target_y));
    if (scene) {
        giftList.pushBack(gift);
        scene->addChild(gift, 10000);
    }

    tryRemoveGift();
}


void RoomManager::tryRemoveGift() {
    int length = giftList.size();
    int limit = 20;
    if(length > limit * 2) {
        auto removeList = Vector<RoomGift*>(limit);

        for (int i = 0; i < length; ++i) {
            if (i < limit) {
                removeList.pushBack(giftList.at(i));
            } else {
                break;
            }
        }

        auto moveBy = MoveBy::create(1, Vec2(0, 20));
        auto fadeTo = FadeTo::create(1, 0);
        auto action = Spawn::createWithTwoActions(moveBy, fadeTo);

        for (int i = 0; i < limit; ++i) {
            auto object = removeList.at(i);
            auto gift = dynamic_cast<RoomGift*>(object);

            auto removeCall = CallFunc::create([&](){
                if (scene) {
                    scene->removeChild(gift);
                }
            });
            auto sequence = Sequence::create(action->clone(), removeCall, nullptr);
            gift->runAction(sequence);

            giftList.eraseObject(gift);
        }
        removeList.clear();
    }
}


void RoomManager::reorganizeAvatars() {

    int length = avatarList.size();
    for (int i = 0; i < length; ++i) {
        auto avatar = avatarList.at(i);
        avatar->jumpTo(getPosition(i + 1, true));
    }

}


const Vec2 RoomManager::getPosition(int rank, bool isPortrait) const {
    if (rank < 1) {
        log("invalid rank = %d", rank);
        return Vec2::ZERO;
    }

    auto rank1Pos = Vec2(topMiddlePoint.x,
                         topMiddlePoint.y - RANK_1_MARGIN_TOP/contentFactor);

    if (rank == 1) {
        return rank1Pos;
    }

    int row = this->getRowByRank(rank, isPortrait);
    int rankInRow = this->getRankInRow(rank, row, isPortrait);
    if (row == 0) {
        float offsetX = ((rankInRow / 2) * ROLE_OFFSET_FIRST_ROW * (float)pow(-1, (rankInRow+1)%2)) / contentFactor;
        float offsetY = (-24 + 8 * ((rankInRow - 2)/2)) / contentFactor;
        rank1Pos.add(Vec2(offsetX, offsetY));
        return rank1Pos;
    }

    int rowOffset = isPortrait ? ROW_OFFSET_PORT : ROW_OFFSET_LAND;
    int roleOffsetX = isPortrait ? ROLE_OFFSET_PORT : ROLE_OFFSET_LAND;

    float offsetX = (rankInRow/2) * roleOffsetX * (float)pow(-1, (rankInRow)%2) / contentFactor;
    float offsetY = -rowOffset * row / contentFactor;
    float additionalX = (row%2 == 0 ? 0: -40)/ contentFactor;
    rank1Pos.add(Vec2(offsetX + additionalX, offsetY));
    return rank1Pos;
}

const int RoomManager::getRowByRank(int rank, bool isPortrait) const {
    if (isPortrait) {
        if (rank <= rowSeatsAccMapPort[0]) {
            return 0;
        } else if (rank <= rowSeatsAccMapPort[1]) {
            return 1;
        } else if (rank <= rowSeatsAccMapPort[2]) {
            return 2;
        } else if (rank <= rowSeatsAccMapPort[3]) {
            return 3;
        } else if (rank <= rowSeatsAccMapPort[4]) {
            return 4;
        } else if (rank <= rowSeatsAccMapPort[5]) {
            return 5;
        } else if (rank <= rowSeatsAccMapPort[6]) {
            return 6;
        } else if (rank <= rowSeatsAccMapPort[7]) {
            return 7;
        }
        return MAX_ROWS_PORT;
    } else {
        if (rank <= rowSeatsAccMapLand[0]) {
            return 0;
        } else if (rank <= rowSeatsAccMapLand[1]) {
            return 1;
        } else if (rank <= rowSeatsAccMapLand[2]) {
            return 2;
        } else if (rank <= rowSeatsAccMapLand[3]) {
            return 3;
        } else if (rank <= rowSeatsAccMapLand[4]) {
            return 4;
        }
        return MAX_ROWS_LAND;
    }
}

const int RoomManager::getRankInRow(int rank, int rowIndex, bool isPortrait) const {
    if (rowIndex == 0) {
        return rank;
    }

    if (isPortrait) {
        return rank - rowSeatsAccMapPort[rowIndex - 1];
    } else {
        return rank - rowSeatsAccMapLand[rowIndex - 1];
    }
}