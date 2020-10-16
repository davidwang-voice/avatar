//
// Created by Rayan on 2019/2/15.
//

#ifndef __KKSTAGE_H__
#define __KKSTAGE_H__

#include "cocos2d.h"
#include "RoomScene.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "../proj.android/libccgameroom/jni/cpp/com_iandroid_allclass_ccgame_room_CCGameRoomJNI.h"
//#include "../proj.android/kkcocos2dx/jni/kkcocos2dxcpp/Java_com_lang_kkcocos2dx_game_GameEngine.h"
#endif

using namespace std;
using namespace cocos2d;

class BaseSprite : public Sprite {
public:
    static BaseSprite* create(int id = 0, int ranking = 0xffff, int priority = 0);

    void onEnter() override;
    void onExit() override;

    inline EventListener* getListener() { return _listener; };

    int getId();
    int getRanking();
    void updateRanking(int ranking);

    void onTouchesBegan(const vector<Touch*>& touches, Event  *event);
    void onTouchesMoved(const vector<Touch*>& touches, Event  *event);
    void onTouchesEnded(const vector<Touch*>& touches, Event  *event);

protected:
    BaseSprite(int id, int ranking, int priority)
            : _id(id)
            , _ranking(ranking)
            , _fixedPriority(priority)
            , _listener(nullptr) {}

protected:
    int _id;     // 0xffff means this sprite is stage
    int _ranking;
    int _fixedPriority;
    EventListener* _listener;
};


#endif //__KKSTAGE_H__
