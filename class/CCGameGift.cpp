//
// Created by David on 2020/10/3.
//

#include "CCGameGift.h"


CCGameGift *CCGameGift::create(int id, int ranking, string skin, int priority) {
    auto gift = new (nothrow) CCGameGift(id, ranking, move(skin), priority);
    if (gift && gift->init()) {
        gift->autorelease();
    } else {
        CC_SAFE_DELETE(gift);
    }
    if (gift)
        gift->initGift();
    return gift;
}

void CCGameGift::setTexture(const std::string &filename) {
    Sprite::setTexture(filename);
    float _max_width = _CONTENT_SIZE_WIDTH_MAX / _scale_factor;
    float _max_height = _CONTENT_SIZE_HEIGHT_MAX / _scale_factor;
    if (getContentSize().width > _max_width  || getContentSize().height > _max_height) {
        float _width_ratio = getContentSize().width / _max_width;
        float _height_ratio = getContentSize().height / _max_height;
        this->setScale(1 / MAX(_width_ratio, _height_ratio));
    }
}

void CCGameGift::initGift() {
    this->_scale_factor = Director::getInstance()->getContentScaleFactor();
    int _star_index = cocos2d::RandomHelper::random_int(1, 5);
    std::string _star_path = "gift/star_" + std::to_string(_star_index) + ".png";

    loadTexture(_skin.c_str(), _star_path.c_str());
    setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
}

void CCGameGift::present(const Vec2 &target) {
    auto position = getPosition();

    int angle = 60;
    int height = 20;

    // 把角度转换为弧度
    float radian = angle*3.14159/180.0;
    // 第一个控制点为抛物线左半弧的中点

//    float q1x = position.x + (target.x - position.x) / 4.0;

    float q1x = position.x + (rand() % (40)) - 20;

    float h1 = (target.y - position.y) / 2 ;

//    CCPoint q1 = Vec2(q1x, height + position.y + cos(radian) * q1x);
    CCPoint q1 = Vec2(q1x, position.y + (target.y - position.y) * 0.8);
    // 第二个控制点为整个抛物线的中点
    float q2x = position.x + ((target.x - position.x) / 3.0) * 1.0;
//    CCPoint q2 = ccp(q2x,height + position.y + cos(radian) * q2x);
    CCPoint q2 = ccp(q2x, MAX(target.y + 20, position.y + 320) );

    //曲线配置
    ccBezierConfig cfg;
    cfg.controlPoint_1 = q1;
    cfg.controlPoint_2 = q2;
    cfg.endPosition = target;
    //使用CCEaseInOut让曲线运动有一个由慢到快的变化，显得更自然
    auto easeOut = CCEaseOut::create(CCBezierTo::create(0.6 , cfg),2.0f);

//    auto rotateBy = RotateBy::create(1, (rand() % (90)) - 45);
    setRotation((rand() % (90)) - 45);

    auto callback = CallFunc::create([&](){
        setLocalZOrder(0);
    });
    auto sequence = Sequence::create(easeOut, callback, nullptr);
    runAction(sequence);
}
