//
// Created by David on 2020/10/3.
//

#ifndef __VOICE_ROOM_AVATAR_H_
#define __VOICE_ROOM_AVATAR_H_

#include <utility>
#include "cocos2d.h"
#include "CCBaseSprite.h"
#include "network/HttpClient.h"

using namespace std;
using namespace cocos2d;
using namespace cocos2d::network;

class CCGameAvatar : public CCBaseSprite {
private:
    static const std::string _BG_NAME_NORMAL_PNG;
    static const std::string _BG_RANK_LABEL_PNG;
    static const std::string _BG_RANK_NAME_PNG;
    static const std::string _BG_SR_NAME_NORMAL_PNG;
    static const std::string _BG_SR_RANK_LABEL_PNG;
    static const std::string _BG_SR_RANK_NAME_PNG;

    static const unsigned int _FRONT_COLUMN_COUNT = 11;

    static const unsigned int _TAG_JUMP_TO_ACTION = 1001;
    static const unsigned int _TAG_MOVE_TO_ACTION = 1002;
    static const unsigned int _TAG_JUMP_BY_ACTION = 1003;
    static const unsigned int _TAG_REST_ORDER_ACTION = 1004;
    static const unsigned int _TAG_SNORE_ANIM_ACTION = 1005;

    static const unsigned int _TAG_RANK_LABEL = 101;
    static const unsigned int _TAG_RANK_LAYER = 102;
    static const unsigned int _TAG_NAME_LABEL = 103;
    static const unsigned int _TAG_NAME_LAYER = 104;
    static const unsigned int _TAG_SSR_MARKER = 105;
    static const unsigned int _TAG_SNORE_ANIM = 106;


    static const unsigned int _NAME_LABEL_MAX_WIDTH = 240;
    static const unsigned int _LABEL_HEIGHT_DEFAULT = 120;
    static const unsigned int _CONTENT_SIZE_WIDTH = 135;
    static const unsigned int _CONTENT_SIZE_HEIGHT = 135;

    static const unsigned int _CHAT_POP_BUBBLE_WIDTH = 200;
    static const unsigned int _CHAT_LOCAL_Z_ORDER_BASE = 200;
    static const unsigned int _PRESENT_LOCAL_Z_ORDER = 400;


    string _skin;
    string _name;
    string _uid;

    unsigned int _last_rare;//avatar等级 1,2,3对应N,R,SR
    unsigned int _rare;//avatar等级 1,2,3对应N,R,SR
    bool _loaded;
    float _target_x;
    float _target_y;
    unsigned int _self_chat_bubble_count;
    float _scale_factor;
    Sprite *_inner_sprite;



    void initAvatar();
    void shakingBody();
    void drawRoundRect(DrawNode *drawNode, const Vec2 &origin, const Vec2 &destination, float radius, unsigned int segments, const Color4F &color);
    void drawRoundRect(DrawNode *drawNode, const Vec2 &origin, const Vec2 &destination, float radius, const Color4F &color, int type);

    const Vec2 roundPoint(const Vec2 &origin) const ;

protected:
    CCGameAvatar(int id, int ranking, string uid, string skin, string name, int priority)
            : CCBaseSprite(id, ranking, priority)
            , _uid(move(uid))
            , _skin(move(skin))
            , _name(move(name)) {}

public:
    virtual ~CCGameAvatar();
    bool isOnStage = false;
    unsigned int _real_local_z_order;
    unsigned int realRanking;

    //for stage.
    unsigned int stageIndex = 0;
    unsigned int offline;

    static CCGameAvatar* create(int id, int ranking, string uid, string skin, string name, int priority = 0);

    void setPosition(const Vec2& pos) override;
    const Vec2 getCenterPosition() const;

    void setTexture(const std::string &filename) override ;
    bool onTouchBegan(Touch *touch, Event *event) override;

    void onTouchMoved(Touch *touch, Event *event) override;

    void onTouchEnded(Touch *touch, Event *event) override;

    void updateRank(int rank);
    void updateElement(const char* name, const char* path, int rare, int guard, int offline = 0);
    void jumpToPosition(const Vec2& target);
    void jumpByPresent();
    void popChatBubble(const char* content);
    void runSnoreAnim();

    void setUid(const char* uid);
    const char* getUid();
};

#endif //__VOICE_ROOM_AVATAR_H_
