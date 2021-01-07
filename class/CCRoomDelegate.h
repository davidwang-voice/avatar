//
// Created by David on 2020/10/3.
//

#ifndef __VOICE_ROOM_MANAGER_H_
#define __VOICE_ROOM_MANAGER_H_

#include "cocos2d.h"
#include "CCGameAvatar.h"
#include "CCGameStep.h"
#include "CCGameGift.h"



#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "../proj.android/libccgameroom/jni/cpp/com_iandroid_allclass_ccgame_room_CCGameRoomJNI.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "OCCallback.h"
#endif



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
    static const int _TAG_SELF_AVATAR = 102;
    static const int _TAG_SELF_APERTURE = 103;

    static const int _RANK_STAND_DEFAULT = 1;
    static const int _RANK_STAGE_DEFAULT = 101;
    static const int _RANK_SELF_DEFAULT = 100;

    static const int _SELF_GROUND_BOTTOM = 100;

    static const int _STAND_ARC_HEIGHT = 66;
    static const int _STAND_MAX_ROW_COUNT = 6;
    static const int _STAND_MAX_COLUMN_COUNT = 11;
    static const int _STAND_ROW_HEIGHT = 135;
    static const int _STAND_FRONT_ROW_HEIGHT = 250;
    static const int _STAND_FRONT_ROW_TOP = 1120;

    static const int _STAGE_BLOCK_COUNT = 3;
    static const int _STAGE_BLOCK_WIDTH = 144;
    static const int _STAGE_BLOCK_TOP = 546;
    static const int _STAGE_BLOCK_LEFT = 758;
    static const int _STAGE_STEP_TOP = 592;

    static const int _STAGE_TABLE_WIDTH = 1329;
    static const int _STAGE_TABLE_HEIGHT = 320;

    static const int _GIFT_HOLDER_SIZE = 100;
    static const int _GIFT_TABLE_WIDTH = 978;
    static const int _GIFT_TABLE_HEIGHT_MIN = 100;
    static const int _GIFT_TABLE_HEIGHT_MAX = 174;
    static const int _GIFT_TABLE_TOP = 912;

    static const int _NONE_SPACE_X = 100;
    static const int _NONE_SPACE_Y = 1600;

    bool _is_released = false;
    bool _is_bg_init = false;
    Scene* _scene;
    Vec2 _visibleOrigin = Vec2::ZERO;
    CCSize _visibleSize = CCSize::ZERO;
    Vec2 _centerPosition;
    float _scaleFactor;
    int _standRowCount[_STAND_MAX_ROW_COUNT];
    Vector<CCGameAvatar*> _standAvatars;
    Vector<CCGameAvatar*> _stageAvatars;
    Vector<CCGameStep*> _stageSteps;
    Vector<CCGameGift*> _giftHolder;


    map<std::string, unsigned int> _randomWheres;
    std::string _bgCache;
    std::string _selfCache;

    std::vector<std::string> _targetCache;

    std::string _heapCache;
    std::string _standCache;
    std::string _stageCache;
    std::vector<std::string> _giftCache;

    void ensureStageSteps();

    const Vec2 getStepPosition(int index) const;
    const Vec2 getStagePosition(int index) const;
    const Vec2 getStandPosition(int index) const;
    const Vec2 getSelfPosition() const;
    const Vec2 getGiftPosition() const;
    const Vec2 getNonePosition(unsigned int where) const;


    CCGameAvatar* findStageAvatar(const char* uid);
    CCGameAvatar* findStandAvatar(const char* uid);
    CCGameAvatar* findSelfAvatar(const char* uid);
    CCGameAvatar* findAvatar(const char* uid);
    CCGameAvatar* createAvatar(int rank, const char* uid, const char* name, const char* url, const Vec2 &pos);
    CCGameAvatar* removeAvatar(const char* uid);


    CCGameAvatar* getSelfAvatar(const char* uid);

    void reorganizeStageAvatars();
    void reorganizeStandAvatars();
    void reorganizeSelfAvatar();

    void refreshTargetAvatar(const char* json);
    void tryRefreshCacheAvatar();

    void createAndPresentGift(const Vec2& pos, const char* url);
    void tryPresentCacheGift();
    void limitGiftHolderSize();
    bool isInBackgroundState(const char* tag);
    bool isApplicationReleased(const char* tag);

public:
    virtual ~CCRoomDelegate();
    void init();
    void attachScene(Scene* scene);
    void resumeFromCache();

    void setStageBackground(const char* url);
    void setupStageGiftHeap(const char* json);
    void updateSelfAvatar(const char* json);
    void updateTargetAvatar(const char* json);

    void updateStageAvatars(const char* json);
    void updateStandAvatars(const char* json);
    void backOffStageAvatar(const char* uid);
    void backOffStandAvatar(const char* uid);

    void receiveGiftMessage(const char* uid, const char* url);
    void receiveChatMessage(const char* uid, const char* content);
    void receiveVoiceWave(const char* uids);
    void receiveRandomSnore(const char* uids);
    void releaseResource();

    // Static Methods
    static CCRoomDelegate *getInstance();

protected:


};

void onTouchStageAvatar(const char* uid);

void onTouchStandAvatar(const char* uid);

void onTouchScene();

#endif //__VOICE_ROOM_MANAGER_H_
