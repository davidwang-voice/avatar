#include "AvatarManager.h"
#include "CCRoomScene.h"
#include "CCBaseSprite.h"

AvatarManager::AvatarManager() {
    firstAvatar = 0;
    avatarCount = 0;
}

AvatarManager::~AvatarManager() {
    map<int, CCBaseSprite*>::iterator iter;
    iter = avatars.begin();
    while (iter != avatars.end()) {
        delete iter->second;
        iter++;
    }

    avatars.clear();
}

void AvatarManager::addAvatar(int id, int ranking, char* imagePath) {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto scene = Director::getInstance()->getRunningScene();
    if (scene) {
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        auto avatar = CCBaseSprite::create(1, id);
        if (avatar == nullptr) {
            problemLoading("Path not found");
        } else {
            avatar->setTexture(imagePath);
            avatar->updateRanking(ranking);
            avatar->setAnchorPoint(Vec2(0.5f, 0.0f));

            if (auto viewer = static_cast<CCRoomScene*>(scene)) {
                float posX, posY;
                // position the sprite on the center of the screen
                if (avatarCount == 0) {
                    posX = visibleSize.width/2;
                    posY = origin.y + visibleSize.height - viewer->getContentSize().height/2 - 60.0f;
                } else {
                    map<int, CCBaseSprite*>::iterator iter;
                    iter = avatars.find(firstAvatar);
                    if (iter != avatars.end()) {
                        posX = avatarCount % 2 == 0 ? iter->second->getPosition().x + 20.0f * (avatarCount / 2)
                                                    : iter->second->getPosition().x - 20.0f * ((avatarCount - 1) / 2);
                        posY = iter->second->getPosition().y;
                    } else {
                        return;
                    }
                }
                avatar->setPosition(Vec2(posX, posY));
                avatars.insert(pair<int, CCBaseSprite*>(id, avatar));

                // add the sprite as a child to this layer
                viewer->addChild(avatar, ++avatarCount);
            }
        }
    }
}

void AvatarManager::removeAvatar(int id) {
    map<int, CCBaseSprite*>::iterator iter;
    iter = avatars.find(id);
    if (iter != avatars.end()) {
        if (id != firstAvatar) {
            avatars.erase(iter);
        } else {
            iter = avatars.begin();
            while (iter != avatars.end()) {
                if (iter->second->getRanking() == 2) {
                    firstAvatar = iter->first;
                    break;
                }
            }
            reorganizeAvatars();
        }
    }
}

void AvatarManager::updateFirstAvatar(int id) {
    map<int, CCBaseSprite*>::iterator iter;
    iter = avatars.find(id);
    if (iter != avatars.end()) {
        firstAvatar = id;
        reorganizeAvatars();
    }
}

/**
 * Internal methods
 */

void AvatarManager::reorganizeAvatars() {
    // TODO: update the position of all avatars that need to be reorganized
}
