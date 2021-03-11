//
// Created by David on 2020/10/3.
//

#include "CCGameGift.h"


CCGameGift *CCGameGift::create(int id, int ranking, int type, string skin, int priority) {
    auto gift = new (nothrow) CCGameGift(id, ranking, type, move(skin), priority);
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
    this->setScale(getMaxScale());
}

void CCGameGift::initGift() {
    this->_scale_factor = Director::getInstance()->getContentScaleFactor();

    std::vector<string> _url_arr;
    std::string _raw = _skin.c_str(), _tmp;
    std::stringstream input(_raw);
    while (getline(input, _tmp, ',')) _url_arr.push_back(_tmp);
    int _random_count = _url_arr.size();

    if (_random_count <= 0) {
        int _star_index = cocos2d::RandomHelper::random_int(1, 5);
        std::string _star_path = "cocos/gift/star_" + std::to_string(_star_index) + ".png";
        setTexture(_star_path);
    } else {
        int _gift_index = cocos2d::RandomHelper::random_int(1, _random_count) - 1;
        loadTexture(_url_arr[_gift_index].c_str());
    }
    setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    if (_type == _GIFT_TYPE_BIGGER) {
        setRotation((rand() % (20)) - 10);
    } else {
        setRotation((rand() % (40)) - 20);
    }

}

float CCGameGift::getMaxScale() {
    float _max_width = _CONTENT_SIZE_WIDTH_MAX / _scale_factor;
    float _max_height = _CONTENT_SIZE_HEIGHT_MAX / _scale_factor;
    if (getContentSize().width > _max_width  || getContentSize().height > _max_height) {
        float _width_ratio = getContentSize().width / _max_width;
        float _height_ratio = getContentSize().height / _max_height;
        return 1 / MAX(_width_ratio, _height_ratio);
    }
    return 1.0;
}

float CCGameGift::getMinScale() {
    float _min_width = _CONTENT_SIZE_WIDTH_MIN / _scale_factor;
    float _min_height = _CONTENT_SIZE_HEIGHT_MIN / _scale_factor;
    if (getContentSize().width > _min_width  || getContentSize().height > _min_height) {
        float _width_ratio = getContentSize().width / _min_width;
        float _height_ratio = getContentSize().height / _min_height;
        return 1 / MAX(_width_ratio, _height_ratio);
    }
    return 1.0;
}

void CCGameGift::present(const Vec2 &target) {


    auto position = getPosition();

    int angle = 60;
    int height = 20;

    // 把角度转换为弧度
    float radian = angle*3.14159/180.0;
    // 第一个控制点为抛物线左半弧的中点

//    float q1x = position.x + (target.x - position.x) / 4.0;


    float _target_y = target.y;
    if (_type == CCGameGift::_GIFT_TYPE_BIGGER) {
        _target_y = _target_y + 60 / _scale_factor;
    }

    float q1x = position.x + (rand() % (40)) - 20;

    float h1 = (_target_y - position.y) / 2 ;

//    CCPoint q1 = Vec2(q1x, height + position.y + cos(radian) * q1x);
    CCPoint q1 = Vec2(q1x, position.y + (_target_y - position.y) * 0.8);
    // 第二个控制点为整个抛物线的中点
    float q2x = position.x + ((target.x - position.x) / 3.0) * 1.0;
//    CCPoint q2 = ccp(q2x,height + position.y + cos(radian) * q2x);
    CCPoint q2 = ccp(q2x, MAX(_target_y + 20, position.y + 320) );

    //曲线配置
    ccBezierConfig cfg;
    cfg.controlPoint_1 = q1;
    cfg.controlPoint_2 = q2;
    cfg.endPosition = Vec2(target.x, _target_y);

//    float _percent = _type == _GIFT_TYPE_SMALL ? 1.2 : 1.0;
//
//    float _distance = position.distance(target);
//    float _duration = (_percent / _MAX_GIFT_THROW_DISTANCE) * _distance;
    float _duration = _type == _GIFT_TYPE_SMALL ? 0.8 : 0.6;

    //使用CCEaseInOut让曲线运动有一个由慢到快的变化，显得更自然
    auto easeOut = CCEaseOut::create(CCBezierTo::create(_duration , cfg),1.0f);

//    auto rotateBy = RotateBy::create(1, (rand() % (90)) - 45);


    if (_type == _GIFT_TYPE_BIGGER) {
        this->setScale(getMinScale());

        auto _sprite = Sprite::create();
        _sprite->setAnchorPoint(Point::ANCHOR_MIDDLE);
        _sprite->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
        this->addChild(_sprite, 1);

        auto _target = Vec2(target.x, target.y);
        auto _self = this;

        auto _action_func = CallFunc::create([_self, _sprite, _target](){
            int _angle = (rand() % (20)) - 10;

            _self->runAction(CCEaseBackOut::create(ScaleTo::create(0.2, _self->getMaxScale())));
            _self->runAction(RotateTo::create(0.2, _angle));
            _self->runAction(CCEaseOut::create(MoveTo::create(0.3, _target),0.8));
            _self->setLocalZOrder(-1);
        });

        auto _anim_func = CallFunc::create([_self, _sprite](){

            Animation *_animation = Animation::create();
            for (int i = 0; i <= 29; i++) {
                auto imagePath = "cocos/anim/gift_ani_" + std::to_string(i) + ".png";
                _animation->addSpriteFrameWithFileName(imagePath.c_str());
            }
            _animation->setDelayPerUnit(0.040f);
            _animation->setRestoreOriginalFrame(false);
            Animate *_animate = Animate::create(_animation);

            _sprite->runAction(_animate);
        });


        auto _callback = CallFunc::create([_self, _sprite](){
            _sprite->setVisible(false);
        });

        auto _sequence = Sequence::create(easeOut, _action_func, DelayTime::create(0.2), _anim_func, DelayTime::create(1.2), _callback, nullptr);
        runAction(_sequence);
    } else {
        auto _callback = CallFunc::create([&](){
            setLocalZOrder(0);
        });

        auto _sequence = Sequence::create(easeOut, _callback, nullptr);
        runAction(_sequence);
    }
}

bool CCGameGift::isIllegalGiftType(int type) {
    return type != _GIFT_TYPE_SMALL && type != _GIFT_TYPE_MIDDLE && type != _GIFT_TYPE_BIGGER;
}