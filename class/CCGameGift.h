//
// Created by David on 2020/10/3.
//

#ifndef __VOICE_ROOM_GIFT_H_
#define __VOICE_ROOM_GIFT_H_


#include <utility>
#include "cocos2d.h"
#include "CCBaseSprite.h"

using namespace std;
using namespace cocos2d;

class CCGameGift : public CCBaseSprite {

private:
    static const int _CONTENT_SIZE_WIDTH_MAX = 500;
    static const int _CONTENT_SIZE_HEIGHT_MAX = 500;

    string _skin;
    int _type;
    float _scale_factor;
    void initGift();


protected:
    CCGameGift(int id, int ranking, int type, string skin, int priority)
            : CCBaseSprite(id, ranking, priority)
            , _type(type), _skin(move(skin)) {}

public:
    static const int _GIFT_TYPE_SMALL = 1;
    static const int _GIFT_TYPE_MIDDLE = 2;
    static const int _GIFT_TYPE_BIGGER = 3;

    virtual ~CCGameGift() = default;
    static CCGameGift* create(int id, int ranking, int type, string skin, int priority = 0);
    void setTexture(const std::string &filename) override ;
    void present(const Vec2& target);
};


#endif //__VOICE_ROOM_GIFT_H_
