//
// Created by David on 2020/10/21.
//

#ifndef __VOICE_ROOM_STEP_H_
#define __VOICE_ROOM_STEP_H_


#include <utility>
#include "cocos2d.h"
#include "CCBaseSprite.h"

using namespace std;
using namespace cocos2d;

class CCGameStep : public CCBaseSprite {
private:
    static const int _TAG_WAVE_ANIM_ACTION = 1001;
    static const int _TAG_STEP_PLACE = 100;

    static const int _CONTENT_SIZE_WIDTH = 120;
    static const int _CONTENT_SIZE_HEIGHT = 195;

    string _skin;
    string _uid;
    bool _mute;
    void initStep();

protected:
    CCGameStep(int id, int ranking, string skin, int priority)
            : CCBaseSprite(id, ranking, priority)
            , _skin(move(skin)) {}

public:
    virtual ~CCGameStep() = default;
    static CCGameStep* create(int id, int ranking, string skin, int priority = 0);
    void runVoiceWave();
    void setMute(bool mute);
    void setUid(const char *uid);
    const char* getUid();

    bool onTouchBegan(Touch *touch, Event *event) override;

    void onTouchMoved(Touch *touch, Event *event) override;

    void onTouchEnded(Touch *touch, Event *event) override;
};


#endif //__VOICE_ROOM_STEP_H_
