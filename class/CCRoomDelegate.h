//
// Created by David on 2020/10/3.
//

#ifndef __VOICE_ROOM_MANAGER_H_
#define __VOICE_ROOM_MANAGER_H_

#include "cocos2d.h"
#include "CCGameAvatar.h"
#include "CCGameStep.h"
#include "CCGameGift.h"
#include <map>

using namespace std;
using namespace cocos2d;

typedef struct {
    int rank;
    const char *path;
    const char *uid;
    const char *name;
    bool ssr;

    bool mute;
    bool wave;
} _Avatar_I;


class CCRoomDelegate {

private:
    static const int _TAG_STAGE_BACKGROUND = 101;

    static const int _STAND_ARC_HEIGHT = 60;
    static const int _STAND_MAX_ROW_COUNT = 7;
    static const int _STAND_ROW_HEIGHT = 140;
    static const int _STAND_FRONT_ROW_HEIGHT = 150;
    static const int _STAND_FRONT_ROW_TOP = 1100;

    static const int _STAGE_BLOCK_COUNT = 3;
    static const int _STAGE_BLOCK_WIDTH = 120;
    static const int _STAGE_BLOCK_TOP = 600;


    static const int _GIFT_TABLE_WIDTH = 960;
    static const int _GIFT_TABLE_HEIGHT = 90;
    static const int _GIFT_TABLE_TOP = 915;

    Scene* _scene;
    Vec2 _visibleOrigin = Vec2::ZERO;
    Size _visibleSize = Size::ZERO;
    Vec2 _centerPosition;
    float _scaleFactor;
    int _standRowCount[_STAND_MAX_ROW_COUNT];
    Vector<CCGameAvatar*> _standAvatars;
    Vector<CCGameAvatar*> _stageAvatars;
    Vector<CCGameStep*> _stageSteps;
    Vector<CCGameGift*> _giftHolder;

    const Vec2 getStagePosition(int index) const;
    const Vec2 getStandPosition(int index) const;
    CCGameAvatar* findStageAvatar(const char* uid);
    CCGameAvatar* findStandAvatar(const char* uid);
    CCGameAvatar* findAvatar(const char* uid);
    CCGameAvatar* createAvatar(int rank, const char* uid, const char* name, const char* path, const Vec2 &pos);
    CCGameAvatar* removeAvatar(const char* uid);

    void reorganizeStageAvatars();
    void reorganizeStandAvatars();

    void createAndPresentGift(const Vec2& pos, const char* imagePath);
    void limitGiftHolderSize();

public:
    virtual ~CCRoomDelegate();
    void init(Scene* scene);

    void setStageBackground(const char* path);

    void updateStageAvatars(const char* json);
    void updateStandAvatars(const char* json);
    void backOffStageAvatar(const char* uid);
    void backOffStandAvatar(const char* uid);

    void receiveGiftMessage(const char* uid, const char* imagePath);
    void receiveChatMessage(const char* uid, const char* content);
    void receiveVoiceWave(const char* uids);
    void releaseResource();

    // Static Methods
    static CCRoomDelegate *getInstance();

protected:


};

#endif //__VOICE_ROOM_MANAGER_H_
