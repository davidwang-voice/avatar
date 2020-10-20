//
// Created by David on 2020/10/3.
//



#include <utility>
#include "RoomAvatar.h"
#include <iomanip>

USING_NS_CC;

using namespace cocos2d::network;

//#include "extensions/cocos-ext.h"
//USING_NS_CC_EXT;

RoomAvatar *RoomAvatar::create(int id, int ranking, string uid, string skin, string name, int priority) {
    auto avatar = new (nothrow) RoomAvatar(id, ranking, uid, move(skin), move(name), priority);
    if (avatar && avatar->init()) {
        avatar->autorelease();
    } else {
        CC_SAFE_DELETE(avatar);
    }
    if (avatar)
        avatar->initAvatar();
    return avatar;
}

void RoomAvatar::initAvatar() {

    // set the avatar skin and other params

    setImageTexture();

    setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);

    float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 1.5;
    auto delay = DelayTime::create(random);
    auto callback = CallFuncN::create(CC_CALLBACK_0(RoomAvatar::shakingBody, this));
    auto seq = Sequence::create(delay, callback, nullptr);
    runAction(seq);

    TTFConfig labelConfig;
    labelConfig.fontFilePath = "font/droid.ttf";
    labelConfig.fontSize = 16;
    labelConfig.glyphs = GlyphCollection::DYNAMIC;
    labelConfig.outlineSize = 0;
    labelConfig.customGlyphs = nullptr;
    labelConfig.distanceFieldEnabled = false;
    auto nameLabel = Label::createWithTTF(labelConfig, _name);
    nameLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    nameLabel->setPosition(getContentSize().width / 2, getContentSize().height);




    Color4B color4B = Color4B(0, 0, 0, 100);

    auto bgLayer = LayerColor::create(color4B, nameLabel->getContentSize().width+10, nameLabel->getContentSize().height);
    bgLayer->setPosition(Vec2(getPosition().x + (getContentSize().width - bgLayer->getContentSize().width)/2, getPosition().y+getContentSize().height));
    addChild(bgLayer);

    addChild(nameLabel);


    _loaded = false;

}


void RoomAvatar::setImageTexture() {

    auto default_skin = "avatar/default_avatar.png";

    auto image_url = "https://avatar.csdn.net/A/6/5/" + _skin;

    string image_path = CCFileUtils::sharedFileUtils()->getWritablePath() + _skin;

    setTexture(default_skin);

//    if (CCFileUtils::sharedFileUtils()->isFileExist(image_path)) {
//        setTexture(image_path);
//    } else {
//        setTexture(default_skin);
//        loadImage(image_url.c_str());
//    }
}


void RoomAvatar::loadImage(const char *url) {


    log("onHttpRequestCompleted In the request");
    HttpRequest* request = new (std::nothrow) HttpRequest();
    request->setUrl(url);
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(RoomAvatar::onLoadImageCompleted, this));
    request->setTag("Post test2");
    HttpClient::getInstance()->sendImmediate(request);
    request->release();
}

void RoomAvatar::onLoadImageCompleted(HttpClient *sender,
                                      HttpResponse *response) {

    if (!response)
    {
        log("onHttpRequestCompleted - No Response");
        return;
    }

    log("onHttpRequestCompleted - Response code: %lu", response->getResponseCode());

    if (!response->isSucceed())
    {
        log("onHttpRequestCompleted - Response failed");
        log("onHttpRequestCompleted - Error buffer: %s", response->getErrorBuffer());
        return;
    }
    log("onHttpRequestCompleted");

//    std::vector<char> *buffer = response->getResponseData();
    Image *image = new Image();
//    image->initWithImageData((unsigned char*)buffer->data(), buffer->size());
//
//    Texture2D *texture = new Texture2D();
//    texture->initWithImage(image);
//
//    image->release();
//
//    setTexture(texture);

    // 数据转存
    unsigned char* pBuffer = NULL;
    unsigned long bufferSize = 0;
    vector<char> *buffer = response->getResponseData();
    string path = CCFileUtils::sharedFileUtils()->getWritablePath() + _skin;
//    pBuffer = CCFileUtils::sharedFileUtils()->getFileData(path.c_str(), "r",
//                                                          reinterpret_cast<ssize_t *>(&bufferSize));
    string buff(buffer->begin(),buffer->end());

    //保存到本地文件
    log("path: %s",path.c_str());
    FILE *fp = fopen(path.c_str(), "wb+");
    fwrite(buff.c_str(), 1,buffer->size(),  fp);
    fclose(fp);

    setTexture(path);




//    log("onHttpRequestCompleted width: %f height: %f", texture->getContentSize().width, texture->getContentSize().height);

}


void RoomAvatar::shakingBody() {
    auto rotateToR = RotateTo::create(0.9f, 6.0f);
    auto rotateToL = RotateTo::create(0.9f, -6.0f);
    auto seq = Sequence::create(rotateToR, rotateToL, nullptr);
    runAction(RepeatForever::create(seq));
}

void RoomAvatar::setPosition(const Vec2 &position) {

//    const Vec2& _target = roundPoint(position);

    Sprite::setPosition(position);
//    log("sprite x: %f, y: %f", getPosition().x, getPosition().y);
}

const char* RoomAvatar::getUid() {
    return _uid.c_str();
}

void RoomAvatar::jumpTo(const Vec2 &target) {

    auto _cur_position = this->getPosition();
    if (_cur_position.equals(target)) {
        // already at target position
        log("already at target position  %f, %f", target.x, target.y);
        return;
    }


    auto _move_action = getActionByTag(10002);
    if (nullptr != _move_action && !_move_action->isDone()) {
        stopAction(_move_action);
    }

    auto _jump_action = getActionByTag(10003);
    if (nullptr != _jump_action && !_jump_action->isDone()) {
        stopAction(_jump_action);
    }

    auto _callback = CallFunc::create([this, target](){
        this->setPosition(target);
    });

    if (_loaded) {
        auto _jumpTo = JumpTo::create(0.5, target, 80, 1);
        auto _action = Sequence::create(_jumpTo, _callback, nullptr);
        _action->setTag(10002);
        runAction(_action);

    } else {
        auto _moveTo = MoveTo::create(0.3, target);
        auto _action = Sequence::create(_moveTo, _callback, nullptr);
        _action->setTag(10003);
        runAction(_action);
    }
    _loaded = true;
}



void RoomAvatar::jumpPresent() {

    auto action = getActionByTag(10001);

    if (nullptr == action || action->isDone()) {
        auto jump = JumpBy::create(0.5f, Vec2(0, 0), 60, 1);
//    auto action = RepeatForever::create(jumpUp);
        jump->setTag(10001);
        runAction(jump);
    }
}
void RoomAvatar::popChatBubble(const char* content) {


    TTFConfig labelConfig;
    labelConfig.fontFilePath = "font/droid.ttf";
    labelConfig.fontSize = 25;
    labelConfig.glyphs = GlyphCollection::DYNAMIC;
    labelConfig.outlineSize = 0;
    labelConfig.customGlyphs = nullptr;
    labelConfig.distanceFieldEnabled = false;


    auto label = Label::createWithTTF(labelConfig, content);
//    label->setAnchorPoint(Point::ANCHOR_MIDDLE_BOTTOM);
    label->setPosition(10 + label->getContentSize().width / 2 , 10 + label->getContentSize().height / 2 );

    Color4B  textColor(116,116,124,255);

    label->setTextColor(textColor);



    auto bubble = BaseSprite::create();
    bubble->setPosition(getContentSize().width/ 2, getContentSize().height * 2 - 40);
//    bubble->setTexture("gift/heart.png");
    bubble->setContentSize(Size(label->getContentSize().width + 20, label->getContentSize().height + 20));


    auto drawNode = DrawNode::create();
    const Vec2 &origin = Vec2(0,0);
    const Vec2 &destination = Vec2(bubble->getContentSize().width, bubble->getContentSize().height);
    Color4F color4F(1, 1, 1, 1);

    drawRoundRect(drawNode,origin,destination,10, 88,color4F);


    bubble->addChild(drawNode);

//    Color4B  bgColor(0,0,0,0);
//    auto bgLayer = LayerColor::create(bgColor, bubble->getContentSize().width, bubble->getContentSize().height);
//        bgLayer->setPosition(0, 0);
//        bgLayer->setContentSize(label->getContentSize());
//    bubble->addChild(bgLayer);

    bubble->addChild(label);
    bubble->setTag(2000);
    addChild(bubble, 1001);


    auto moveBy = MoveBy::create(2, Vec2(0, 30));
    auto fadeOut = FadeOut::create(2);


    auto fadeCall = CallFunc::create([label, drawNode](){
        label->runAction(FadeOut::create(2));
        drawNode->runAction(FadeOut::create(2));
    });

    auto removeCall = CallFunc::create([bubble](){
        bubble->removeFromParentAndCleanup(true);
    });


    auto sequence = Sequence::create(moveBy, fadeCall, DelayTime::create(2), removeCall, nullptr);
    bubble->runAction(sequence);

    log("userSpeak runAction: %s", content);
}

void RoomAvatar::drawRoundRect(DrawNode *drawNode, const Vec2 &origin, const Vec2 &destination,
                               float radius, unsigned int segments, const Color4F &color)
{
    const float coef = 2.0f * (float)M_PI/(segments - 8);

    Vec2 *vertices = new (std::nothrow) Vec2[segments];
    if( ! vertices )
        return;

    // Draw right edge
    vertices[0].x = destination.x;
    vertices[0].y = origin.y + radius;
    vertices[1].x = destination.x;
    vertices[1].y = destination.y - radius;

    unsigned int quadrant = 1;
    unsigned int radsI = 0;
    for(unsigned int i = 2; i <= segments; i++) {
        float rads = radsI*coef;
        GLfloat j = radius * cosf(rads);
        GLfloat k = radius * sinf(rads);

        if (rads < M_PI_2 || rads > M_PI + M_PI_2) {
            if (quadrant == 3) {
                // Draw bottom edge
                vertices[i].x = origin.x + radius;
                vertices[i].y = origin.y;
                i++;
                vertices[i].x = destination.x - radius;
                vertices[i].y = origin.y;

                quadrant++;
                continue;
            }
            j += destination.x - radius;
        } else {
            if (quadrant == 1) {
                // Draw top edge
                vertices[i].x = destination.x - radius;
                vertices[i].y = destination.y;
                i++;
                vertices[i].x = origin.x + radius;
                vertices[i].y = destination.y;

                quadrant++;
                continue;
            }
            j += origin.x + radius;
        }

        if (rads < M_PI) {
            k += destination.y - radius;
        } else {
            if (quadrant == 2) {
                // Draw left edge
                vertices[i].x = origin.x;
                vertices[i].y = destination.y - radius;
                i++;
                vertices[i].x = origin.x;
                vertices[i].y = origin.y + radius;

                quadrant++;
                continue;
            }
            k += origin.y + radius;
        }

        vertices[i].x = j;
        vertices[i].y = k;

        radsI++;
    }

    drawNode->drawSolidPoly(vertices, segments, color);

    CC_SAFE_DELETE_ARRAY(vertices);
}

const Vec2 RoomAvatar::roundPoint(const Vec2 &origin) const {

    float _target_x = origin.x;
    float _target_y = origin.y;

    std::stringstream _value_x;
    std::stringstream _value_y;
//    cout<<endl;
    _value_x << fixed << setprecision(2) << _target_x;
    _value_y << fixed << setprecision(2) << _target_y;

    float _ret_x = atof(_value_x.str().c_str());
    float _ret_y = atof(_value_y.str().c_str());

    return Vec2(_ret_x, _ret_y);
}