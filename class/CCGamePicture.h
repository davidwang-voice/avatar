//
// Created by David on 2021/4/19.
//

#ifndef PROJ_ANDROID_CCGAMEPICTURE_H
#define PROJ_ANDROID_CCGAMEPICTURE_H

#include <utility>
#include "cocos2d.h"
#include "CCBaseSprite.h"
#include "Gif/CacheGif.h"
#include "Gif/CacheGifData.h"



using namespace std;
using namespace cocos2d;


class CCGamePicture: public CCBaseSprite {

private:
    static const int _CONTENT_SIZE_WIDTH_MAX = 200;
    static const int _CONTENT_SIZE_HEIGHT_MAX = 200;

    static const int _CONTENT_SIZE_WIDTH_MIN = 120;
    static const int _CONTENT_SIZE_HEIGHT_MIN = 120;

    string _skin;


    CacheGif *_picture_gif;
    Sprite *_picture_png;

    float _scale_factor;
    void initPicture();
    float getMaxScale(Node* node);



protected:
    CCGamePicture(int id, int ranking, string skin, int priority)
            : CCBaseSprite(id, ranking, priority)
            , _skin(move(skin)) {}

public:
    void setOpacity(GLubyte opacity) override;

public:
    virtual ~CCGamePicture() = default;
    static CCGamePicture* create(int id, int ranking, string skin, int priority = 0);
    void setTexture(const std::string &filename) override ;
};


#endif //PROJ_ANDROID_CCGAMEPICTURE_H
