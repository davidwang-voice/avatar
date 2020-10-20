#include <RoomAvatar.h>
#include <RoomManager.h>
#include "SimpleAudioEngine.h"
#include "RoomScene.h"
#include "AppDelegate.h"
#include "BaseSprite.h"

USING_NS_CC;

static const char* g_stagePath = "bg-night.jpg";
static PanZoomState g_state = PanZoomState::None;
static PanZoomState g_prevState = PanZoomState::None;

Scene* RoomScene::createScene() {
    return RoomScene::create();
}

void RoomScene::setStage(const char* stagePath) {
    g_stagePath = stagePath;
}

bool RoomScene::init() {
    if (!Scene::init()) {
        return false;
    }

    _stage = nullptr;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add "LiveViewerLand" splash screen"
    _stage = BaseSprite::create();
    _stage->setTexture(g_stagePath);
    if (_stage == nullptr) {
        problemLoading(g_stagePath);
    } else {
        // position the sprite on the center of the screen
        float x = visibleSize.width/2 + origin.x;
        float y = origin.y + visibleSize.height - _stage->getContentSize().height/2;
        _stage->setPosition(Vec2(x, y));

        // add the sprite as a child to this layer
        this->addChild(_stage, 0);
    }

//    this->scheduleUpdate();

    // avatar
    auto avatarManager = RoomManager::getInstance();
    avatarManager->init(this);

    auto size = Director::getInstance()->getOpenGLView()->getFrameSize();
    bool isPortrait = size.width < size.height;

    log("last orientation = %d", AppDelegate::g_lastOrientation);

//    for(int index = 1; index <= 100; index++) {
//        auto imagePath = "avatar/" + std::to_string(index%10 + 1) + ".png";
//        auto name = "叫我第" + std::to_string(index);
//        avatarManager->addAvatar(index, index, imagePath.c_str(), name.c_str());
//    }

    AppDelegate::g_lastOrientation = isPortrait ? 1 : 2;

    return true;
}

void RoomScene::onEnter() {
    Node::onEnter();

    auto listener = EventListenerTouchAllAtOnce::create();

    listener->onTouchesBegan = CC_CALLBACK_2(RoomScene::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(RoomScene::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(RoomScene::onTouchesEnded, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    _listener = listener;
}

void RoomScene::onExit() {
    _eventDispatcher->removeEventListener(_listener);

    Node::onExit();
}

void RoomScene::update(float delta) {
    log("RoomScene::update .delta=%f",delta);
    Node::update(delta);
}

void RoomScene::onTouchesBegan(const std::vector<Touch*>& touches, Event *event) {
    if(touches.size() == 1) {
        log("Scene onTouchBegan..x=%f, y=%f", touches[0]->getLocation().x, touches[0]->getLocation().y);
        auto glview = Director::getInstance()->getOpenGLView();
        if (glview->getAllTouches().size() > 1 && g_state == PanZoomState::None)
            g_prevState = g_state = PanZoomState::Zoom;
        if (_startPoint == Vec2::ZERO)
            _startPoint = touches[0]->getStartLocationInView();
        onTouchDown();
    }
}

void RoomScene::onTouchesMoved(const std::vector<Touch*>& touches, Event *event) {
//    if (g_state == PanZoomState::Rebound)
//        return;
//
//    if(touches.size() == 1) {
//        if (g_state == PanZoomState::Zoom)
//            return;
//
//        auto glview = Director::getInstance()->getOpenGLView();
//        if (g_state == PanZoomState::Pan) {
//            auto delta = touches[0]->getLocationInView() - _startPoint;
//            float deltaX = delta.x * glview->getScaleX();
//            float deltaY = delta.y * glview->getScaleY();
//            onTouchMove(deltaX, deltaY);
//            if (auto scene = dynamic_cast<RoomScene*>(event->getCurrentTarget())) {
//                auto finalPos = scene->getPosition() + touches[0]->getDelta();
//                auto curPos = scene->getPosition();
//                scene->setPosition(finalPos);
//
//                // set x and y boundary
//                Vec2 visibleOrigin = Director::getInstance()->getVisibleOrigin();
//                Size visibleSize = Director::getInstance()->getVisibleSize();
//                Rect box = _stage->getBoundingBox();
//                Vec2 minXY = scene->convertToWorldSpace(box.origin);
//                Vec2 maxXY = minXY + Vec2(box.size);
//                // x border
//                if (minXY.x > visibleOrigin.x + _blackBorder ||
//                    maxXY.x < (visibleOrigin.x + visibleSize.width - _blackBorder)) {
//                    finalPos.x = curPos.x;
//                }
//
//                // y border
//                if (maxXY.y > visibleOrigin.y + visibleSize.height + _blackBorder ||
//                    maxXY.y < (visibleOrigin.y + visibleSize.height - _blackBorder)) {
//                    finalPos.y = curPos.y;
//                }
//                scene->setPosition(finalPos);
//            }
//        } else {
//            auto delta = touches[0]->getDelta();
//            if (abs(delta.x) > 10.f || abs(delta.y) > 10.f) {
//                g_prevState = g_state = PanZoomState::Pan;
//                _startPoint = touches[0]->getLocationInView();
//            }
//        }
//    } else if (g_state == PanZoomState::Zoom) {
//        auto target = event->getCurrentTarget();
//        auto distance1 = touches[0]->getPreviousLocation().distance(touches[1]->getPreviousLocation());
//        auto distance2 = touches[0]->getLocation().distance(touches[1]->getLocation());
//        float scale = target->getScale() * (distance2 / distance1);
//        scale = (float) MIN(1.5, MAX(0.8, scale));
//        onTouchesMove(scale);
//        target->setScale(scale);
//    }
}

void RoomScene::onTouchesEnded(const std::vector<Touch*>& touches, Event *event) {
    if (g_state == PanZoomState::Rebound)
        return;

    if (auto scene = dynamic_cast<RoomScene*>(event->getCurrentTarget())) {
        Rect box = _stage->getBoundingBox();
        Vec2 minXY = scene->convertToWorldSpace(box.origin);
        Vec2 maxXY = minXY + Vec2(box.size);
        Vec2 posDelta = Vec2::ZERO;
        posDelta = getDeltaPosition(minXY, maxXY);

        if(touches.size() == 1) {
            auto glview = Director::getInstance()->getOpenGLView();
            if (glview->getAllTouches().size() == 0)
                g_state = PanZoomState::None;
            onTouchUp();
        }

        if (g_prevState == PanZoomState::Pan) {
            Vector<FiniteTimeAction *> actions;
            if (posDelta.x != 0 || posDelta.y != 0) {
                actions.pushBack(MoveBy::create(_reboundTime, posDelta));
            }
            if (actions.size() > 0) {
                scene->runAction(Sequence::createWithTwoActions(
                        Spawn::create(actions),
                        CallFunc::create(RoomScene::onReboundEnd))
                );
                g_state = PanZoomState::Rebound;
            }
        }
        if (g_state == PanZoomState::None)
            g_prevState = g_state;
    }
}

void RoomScene::changeDirection(int width, int height) {
    log("changeDirection: w %d, h %d", width, height);

    log("scene position: x: %f, y: %f", getPosition().x, getPosition().y);

    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    auto currentOrientation = (width < height) ? 1 : 2;
    log("last orientation in changeDirection = %d, current = %d", AppDelegate::g_lastOrientation, currentOrientation);
    if (AppDelegate::g_lastOrientation == currentOrientation) {
        log("orientation not changed, %d", currentOrientation);
        return;
    }

    auto designResolutionSize = AppDelegate::getDesignResolutionSize();

    // orientation change
    glview->setFrameSize(width, height);
    if (width >= height)
        glview->setDesignResolutionSize(designResolutionSize.height, designResolutionSize.width, ResolutionPolicy::FIXED_WIDTH);
    else
        glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_WIDTH);


    director->replaceScene(RoomScene::createScene());


    auto screenSize = glview->getFrameSize();
    auto winSize = Director::getInstance()->getWinSize();
    auto winSizeInPixel = Director::getInstance()->getWinSizeInPixels();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    log("Screen width: %f, screen height: %f", screenSize.width, screenSize.height);
    log("WinSize width: %f, WinSize height: %f", winSize.width, winSize.height);
    log("WinSizeInPixel width: %f, WinSizeInPixel height: %f", winSizeInPixel.width, winSizeInPixel.height);
    log("VisibleSize width: %f, VisibleSize height: %f", visibleSize.width, visibleSize.height);
    log("Visible origin X: %f, Visible origin Y height: %f", Director::getInstance()->getVisibleOrigin().x, Director::getInstance()->getVisibleOrigin().y);
}


Vec2 RoomScene::getDeltaPosition(Vec2 minXY, Vec2 maxXY) {
    Vec2 posDelta(0, 0);
    Vec2 visibleOrigin = Director::getInstance()->getVisibleOrigin();
    Size visibleSize = Director::getInstance()->getVisibleSize();

    if (minXY.x > visibleOrigin.x) {
        posDelta.x = -(minXY.x - visibleOrigin.x);
    } else if (maxXY.x < visibleOrigin.x + visibleSize.width) {
        posDelta.x = visibleOrigin.x + visibleSize.width - maxXY.x;
    }

    if (maxXY.y > visibleOrigin.y + visibleSize.height) {
        posDelta.y = -(maxXY.y - (visibleOrigin.y + visibleSize.height));
    } else if (maxXY.y < visibleOrigin.y + visibleSize.height) {
        posDelta.y = visibleOrigin.y + visibleSize.height - maxXY.y;
    }

    return posDelta;
}


void RoomScene::onReboundEnd() {
    g_state = PanZoomState::None;
}
