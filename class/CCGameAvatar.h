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
    static const int _TAG_JUMP_TO_ACTION = 10001;
    static const int _TAG_MOVE_TO_ACTION = 10002;
    static const int _TAG_JUMP_BY_ACTION = 10003;

    static const int _TAG_RANK_LABEL = 101;
    static const int _TAG_RANK_LAYER = 102;
    static const int _TAG_NAME_LABEL = 103;
    static const int _TAG_NAME_LAYER = 104;

    static const int _LABEL_HEIGHT_DEFAULT = 100;

    static const int _CHAT_LOCAL_Z_ORDER_BASE = 200;

    string _skin;
    string _name;
    string _uid;
    bool _loaded;
    float _target_x;
    float _target_y;
    int _local_z_order;
    int _chat_local_z_order;
    float _scale_factor;

    void initAvatar();
    void shakingBody();
    void drawRoundRect(DrawNode *drawNode, const Vec2 &origin, const Vec2 &destination, float radius, unsigned int segments, const Color4F &color);
    const Vec2 roundPoint(const const Vec2 &origin) const ;

protected:
    CCGameAvatar(int id, int ranking, string uid, string skin, string name, int priority)
            : CCBaseSprite(id, ranking, priority)
            , _uid(move(uid))
            , _skin(move(skin))
            , _name(move(name)) {}

public:
    virtual ~CCGameAvatar() = default;
    bool isOnStage = false;

    static CCGameAvatar* create(int id, int ranking, string uid, string skin, string name, int priority = 0);

    void setPosition(const Vec2& pos) override;
    const Vec2 getCenterPosition() const;

    bool onTouchBegan(Touch *touch, Event *event) override;

    void onTouchMoved(Touch *touch, Event *event) override;

    void onTouchEnded(Touch *touch, Event *event) override;

    void updateRank(int rank);
    void updateElement(const char* name, const char* path, int rare);

    void jumpToPosition(const Vec2& target);
    void jumpByPresent();
    void popChatBubble(const char* content);


    void setUid(const char* uid);
    const char* getUid();
};


#endif //__VOICE_ROOM_AVATAR_H_
