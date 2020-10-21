#ifndef __AvatarManager_H__
#define __AvatarManager_H__

#include <map>

using namespace std;

class CCBaseSprite;

class AvatarManager {
public:
    AvatarManager();
    virtual ~AvatarManager();

    void addAvatar(int id, int ranking, char* imagePath);
    void removeAvatar(int id);
    void updateFirstAvatar(int id);

private:
    int firstAvatar;
    int avatarCount;

    map<int, CCBaseSprite*> avatars; // (id, avatar sprite)

    //////////////////////
    // internal methods //
    //////////////////////

    void reorganizeAvatars();
};

#endif // __AvatarManager_H__
