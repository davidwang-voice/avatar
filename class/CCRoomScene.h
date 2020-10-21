#ifndef __LIVEROOMSCENE_H__
#define __LIVEROOMSCENE_H__

#include "cocos2d.h"

// Custom
#include "Wrapper.h"
#include "Config.h"


class CCBaseSprite;

using namespace std;

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename) {
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'assets/' in front of filenames in LiveRoomScene.cpp\n");
}

enum PanZoomState {
    None,
    Pan,
    Zoom,
    Rebound
};

class CCRoomScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();

    static void setStage(const char* stagePath);

    bool init() override;
    void onEnter() override;
    void onExit() override;
    void update(float delta) override;

    void onTouchesBegan(const vector<Touch*>& touches, Event  *event);
    void onTouchesMoved(const vector<Touch*>& touches, Event  *event);
    void onTouchesEnded(const vector<Touch*>& touches, Event  *event);

    void changeDirection(int width, int height);
    CCBaseSprite* getStage() { return _stage; };

    CREATE_FUNC(CCRoomScene);

private:
    CCBaseSprite* _stage;
    EventListener* _listener;
    Vec2 _startPoint = Vec2::ZERO;
    float _blackBorder = 200.f; // in pixels
    float _reboundTime = 0.2f;  // in secs


private:
    Vec2 getDeltaPosition(Vec2 minXY, Vec2 maxXY);

    static void onReboundEnd();
};

#endif // __LIVEROOMSCENE_H__
