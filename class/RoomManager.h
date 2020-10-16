//
// Created by David on 2020/10/3.
//

#ifndef __VOICE_ROOM_MANAGER_H_
#define __VOICE_ROOM_MANAGER_H_

#include "cocos2d.h"
#include "RoomAvatar.h"
#include "RoomGift.h"
#include <map>

using namespace std;
using namespace cocos2d;

class RoomManager {

private:
    const float RANK_1_MARGIN_TOP = 1080; // in pixel according to 1920x1080 map
    const int MAX_ROWS_LAND = 5;
    const int MAX_ROWS_PORT = 8;
    const int ROW_OFFSET_LAND = 110;
    const int ROW_OFFSET_PORT = 130;
    const int ROLE_OFFSET_LAND = 72;
    const int ROLE_OFFSET_PORT = 80;
    const int ROLE_OFFSET_FIRST_ROW = 80;

    Scene* scene;

    Vec2 visibleOrigin = Vec2::ZERO;
    Size visibleSize = Size::ZERO;
    float contentFactor;
    Vec2 topMiddlePoint;
    bool isPortrait;


    Vector<RoomAvatar*> avatarList; // (id, avatar sprite)
//    map<int, RoomAvatar*> avatarMap; // (id, avatar sprite)
    Vector<RoomGift*> giftList; // (gift sprite)


    mutable map<int, int> rowSeatsAccMapLand;
    mutable map<int, int> rowSeatsAccMapPort;

    void initRowSeats();
    const int getRowByRank(int rank, bool isPortrait) const;
    const int getRankInRow(int rank, int rowIndex, bool isPortrait) const;

    void tryRemoveGift();

    void reorganizeAvatars();
    void presentGift(const Vec2& pos, const char* imagePath);

    RoomAvatar* addAvatar(int id, int ranking, const char* imagePath, const char* name);
    void removeAvatar(int id);
    RoomAvatar* findAvatar(int id);

public:
    void initScene(Scene* scene);
    virtual ~RoomManager();
    virtual const Vec2 getPosition(int rank, bool isPortrait) const;



    void receiveGiftMessage(int id, const char* imagePath);
    void receiveChatMessage(int id, const char* content);

    void updateStageAvatars(const char* json);
    void updateStandAvatars(const char* json);
    void backOffStageAvatar(int id);
    void backOffStandAvatar(int id);

    void releaseResource();

    // Static Methods
    static RoomManager *getInstance();

protected:


};

#endif //__VOICE_ROOM_MANAGER_H_
