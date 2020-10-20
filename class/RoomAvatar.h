//
// Created by David on 2020/10/3.
//

#ifndef __VOICE_ROOM_AVATAR_H_
#define __VOICE_ROOM_AVATAR_H_


#include <utility>
#include "cocos2d.h"
#include "BaseSprite.h"
#include "network/HttpClient.h"

using namespace std;
using namespace cocos2d;
using namespace cocos2d::network;

class RoomAvatar : public BaseSprite {
private:
    string _skin;
    string _name;
    string _uid;
    float _positionX;
    float _positionY;
    bool _loaded;

    void initAvatar();
    void shakingBody();

    void setImageTexture();
    void loadImage(const char* url);
    void onLoadImageCompleted(HttpClient *sender, HttpResponse *response);
    void drawRoundRect(DrawNode *drawNode, const Vec2 &origin, const Vec2 &destination, float radius, unsigned int segments, const Color4F &color);

    const Vec2 roundPoint(const const Vec2 &origin) const ;

protected:
    RoomAvatar(int id, int ranking, string uid, string skin, string name, int priority)
            : BaseSprite(id, ranking, priority)
            , _uid(move(uid))
            , _skin(move(skin))
            , _name(move(name)) {}

public:
    virtual ~RoomAvatar() = default;

    static RoomAvatar* create(int id, int ranking, string uid, string skin, string name, int priority = 0);

    void jumpTo(const Vec2& target);

    void setPosition(const Vec2& pos) override;
    void jumpPresent();
    void popChatBubble(const char* content);

    const char* getUid();
};


#endif //__VOICE_ROOM_AVATAR_H_
