package com.iandroid.allclass.ccgame.room;

import android.content.Context;
import android.widget.FrameLayout;

/**
 * Created by david on 2020/10/15.
 */
public interface CCGameRoomView {

    void setCallback(Callback callback);

    void focusChanged(boolean focus);

    void resume();

    void pause();

    void release();

    void setRoomType(int type, float topPixel);

    void setBackground(String url);

    void setupGiftHeap(String json);

    void setSelfAvatar(String json);

    void setTargetAvatar(String json);

    void presentGift(String json);

    void clearGiftPool(int type);

    void presentChat(String userId, String content);

    void presentInfo(String userId, String content);

    void presentPicture(String userId, String url);

    void syncStageUsers(String json);

    void syncStandUsers(String json);

    void onVoiceWave(String userIds);

    void onRandomSnore(String userIds);

    void leaveStage(String userId);
    void leaveStand(String userId);

    interface Callback {
        void onTouchStageAvatar(String uid);
        void onTouchStandAvatar(String uid);

        void onTouchRoomScene();
    }


    class Creator {
        public static CCGameRoomView create(Context context, FrameLayout parent) {
            return new CCGameRoomImpl(context, parent);
        }

    }
}
