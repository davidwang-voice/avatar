//
// Created by Rayan on 2019/2/15.
//

#ifndef __KKSTAGE_H__
#define __KKSTAGE_H__

#include "cocos2d.h"
#include "Utils.h"
#include "network/HttpClient.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "../proj.android/libccgameroom/jni/cpp/com_iandroid_allclass_ccgame_room_CCGameRoomJNI.h"
#endif

using namespace std;
using namespace cocos2d;
using namespace cocos2d::network;

class CCBaseSprite : public Sprite {
private:
    void sendResourceRequest(const char* url, const char* tag);
    void onRequestCompleted(HttpClient *sender, HttpResponse *response);

public:
    static CCBaseSprite* create(int id = 0, int ranking = 0xffff, int priority = 0);

    void onEnter() override;
    void onExit() override;

    inline EventListener* getListener() { return _listener; };

    int getId();
    int getRanking();
    void updateRanking(int ranking);

    virtual void onTouchesBegan(const vector<Touch*>& touches, Event  *event);
    virtual void onTouchesMoved(const vector<Touch*>& touches, Event  *event);
    virtual void onTouchesEnded(const vector<Touch*>& touches, Event  *event);

    virtual bool onTouchBegan(Touch* touch, Event  *event);
    virtual void onTouchMoved(Touch* touch, Event  *event);
    virtual void onTouchEnded(Touch* touch, Event  *event);

    void loadTexture(const char *name, const char* def = nullptr);

protected:
    CCBaseSprite(int id, int ranking, int priority)
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
