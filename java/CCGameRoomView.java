package com.iandroid.allclass.ccgame.room;

import android.content.Context;
import android.widget.FrameLayout;

/**
 * Created by david on 2020/10/15.
 */
public interface CCGameRoomView {

    void focusChanged(boolean focus);

    void resume();

    void pause();

    void release();

    void presentGift(int userId, String url);

    void presentChat(int userId, String content);

    void syncStageUsers(String json);

    void syncStandUsers(String json);

//  void onVoiceWave(String json);

    void leaveStage(int userId);
    void leaveStand(int userId);

    class Creator {
        public static CCGameRoomView create(Context context, FrameLayout parent) {
            return new CCGameRoomImpl(context, parent);
        }

    }
}
