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

//房间舞台类型
enum RoomType {AUDIO, VIDEO};

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
    static const int _VIDEO_STAND_MAX_ROW_COUNT = 4;
    static const int _STAND_MAX_COLUMN_COUNT = 11;
    static const int _STAND_ROW_HEIGHT = 135;
    static const int _STAND_FRONT_ROW_HEIGHT = 135;
    static const int _STAND_FRONT_ROW_TOP = 1120 + 55;

    static const int _STAGE_BLOCK_COUNT = 3;
    static const int _STAGE_BLOCK_WIDTH = 144;
    static const int _STAGE_BLOCK_TOP = 546;
    static const int _STAGE_BLOCK_LEFT = 758;
    static const int _STAGE_STEP_TOP = 592;

    static const int _STAGE_TABLE_WIDTH = 1329;
    static const int _STAGE_TABLE_HEIGHT = 320;

    static const int _S_GIFT_HOLDER_SIZE = 300;
    static const int _M_GIFT_HOLDER_SIZE = 12;
    static const int _B_GIFT_HOLDER_SIZE = 8;

    static const int _GIFT_TABLE_WIDTH = 1125;
    static const int _GIFT_TABLE_HEIGHT_MIN = 100;
    static const int _GIFT_TABLE_HEIGHT_MAX = 200;
    static const int _GIFT_TABLE_TOP = 980 - 30;

    static const int _NONE_SPACE_X = 100;
    static const int _NONE_SPACE_Y = 1600;
    
    static const int _VIDEO_PLAYER_HEIGHT = 630;//视频播放器高度
    static const int _VIDEO_PLAYER_TO_STAND = 75;//视频播放器底部距离礼物舞台顶部的距离
    static const int _AUDIO_GIFT_TABLE_TO_TOP_MARGIN = 840;
    static const int _VIDEO_PLAYER_TO_BG_TOP = 690;//视频播放器距离背景图顶部距离

    std::vector<Vec2> _randomPosition {
            Vec2(186, 897),
            Vec2(294, 882),
            Vec2(402, 900),
            Vec2(510, 909),
            Vec2(618, 900),
            Vec2(726, 888),
            Vec2(834, 900),
            Vec2(942, 909)
    };

    bool _is_attached = false;
    bool _is_released = true;
    float _player_to_top_margin = 0;//播放器距离顶部的距离
    float _bg_to_top_offset = 0;//背景距离顶部的偏移量
    float _video_stage_offset = 0;//视频房舞台相对音频的偏移量
    float _ratio = 1; //缩放比例
    RoomType _roomType = RoomType::AUDIO; //房间类型 默认音频
    Scene* _scene;
    Vec2 _visibleOrigin = Vec2::ZERO;
    CCSize _visibleSize = CCSize::ZERO;
    Vec2 _centerPosition;
    float _scaleFactor;
    int _standRowCount[_STAND_MAX_ROW_COUNT];
    Vector<CCGameAvatar*> _standAvatars;
    Vector<CCGameAvatar*> _stageAvatars;
    Vector<CCGameStep*> _stageSteps;

    Vector<CCGameGift*> _s_giftHolder;
    Vector<CCGameGift*> _m_giftHolder;
    Vector<CCGameGift*> _b_giftHolder;

    map<std::string, unsigned int> _randomWheres;
    std::string _bgCache;
    std::string _selfCache;

    std::vector<std::string> _targetCache;

    std::string _heapCache;
    std::string _standCache;
    std::string _stageCache;
    std::vector<std::string> _s_giftCache;
    std::vector<std::string> _m_giftCache;
    std::vector<std::string> _b_giftCache;
    std::vector<int> _clearGift;

    map<std::string, std::vector<std::string>> _s_scheduleMap;
    map<std::string, std::vector<std::string>> _m_scheduleMap;
    map<std::string, std::vector<std::string>> _b_scheduleMap;

    void ensureStageSteps();

    const Vec2 getStepPosition(int index) const;
    const Vec2 getStagePosition(int index) const;
    const Vec2 getStandPosition(int index) const;
    const Vec2 getSelfPosition() const;
    const Vec2 getGiftPosition(int type) const;
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

    map<std::string, std::vector<std::string>>& getGiftScheduleMap(int type);
    Vector<CCGameGift*>& getGiftHolder(int type);
    std::vector<std::string>& getGiftCache(int type);

    void scheduleHandleGift(int type, const std::string &uid, const std::string &urls, int count, float delay, float interval);
    void schedulePresentGift(int type, const std::string &key, const std::string &uid, const std::string &urls);
    void cacheWillPresentGift();
    void cachePresentTargetGift(int type);
    void cacheBackPresentGift(int type, const char* urls);
    void createAndPresentGift(int type, const Vec2& pos, const char* urls);
    void tryPresentCacheGift();
    void tryExecuteClearGift();
    void presentTargetGift(int type);
    int getLimitHolderSize(int type);
    void limitAllGiftHolder();
    void limitTargetGiftHolder(int type = 0, int count = 0);
    bool isApplicationInvalid(const char* tag);
    bool isApplicationReleased(const char* tag);

public:
    virtual ~CCRoomDelegate();
    void init();
    void attachScene(Scene* scene);
    void resumeFromCache();
    void pause();

    // topMargin 视频播放器距顶部的距离
    void updateRoomType(RoomType type, float topMargin, float scale);
    void setStageBackground(const char* url);
    void setupStageGiftHeap(const char* json);
    void updateSelfAvatar(const char* json);
    void updateTargetAvatar(const char* json);

    void updateStageAvatars(const char* json);
    void updateStandAvatars(const char* json);
    void backOffStageAvatar(const char* uid);
    void backOffStandAvatar(const char* uid);

    void clearTargetGiftPool(int type);

    void receiveGiftMessage(const char* json);
    void receiveChatMessage(const char* uid, const char* content);
    void receiveInfoMessage(const char* uid, const char* content);
    void receiveChatPicture(const char* uid, const char* url);

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
