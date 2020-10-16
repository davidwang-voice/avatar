package com.iandroid.allclass.ccgame.room;

import android.util.Log;

import java.lang.ref.WeakReference;

/**
 * Created by david on 2020/10/14.
 */
public class CCGameRoomJNI {
    public final static String TAG = "CCGameRoomJNI";

    static {
        try {
            System.loadLibrary("ccgameroom");
        } catch (UnsatisfiedLinkError usl) {
            Log.e(TAG, usl.getMessage());
        }
    }

    private static class SingletonHolder {
        private static final CCGameRoomJNI sInstance = new CCGameRoomJNI();
    }

    public static CCGameRoomJNI getInstance() {
        return SingletonHolder.sInstance;
    }


    public interface CCListener {
        void onTouchedAvatar(int id);

        void onTouchedScene();
    }


    private WeakReference<CCListener> reference;

    public void setCCListener(CCListener listener) {
        this.reference = new WeakReference<>(listener);
    }

    public CCListener getCCListener() {
        if (reference != null && reference.get() != null)
            return reference.get();
        return null;
    }

    //native call.
    public static void onTouchedAvatar(int id) {
        Log.i(TAG, "onTouchedAvatar id: " + id);
        if (getInstance().getCCListener() != null)
            getInstance().getCCListener().onTouchedAvatar(id);
    }

    public static void onTouchedScene() {
        Log.i(TAG, "onTouchedScene");
        if (getInstance().getCCListener() != null)
            getInstance().getCCListener().onTouchedScene();
    }



    public native int[] getGLContextAttrs();

    public native void setDesignResolution(float width, float height);

    public native void receiveGiftMessage(int id, String path);

    public native void receiveChatMessage(int id, String content);

    public native void updateStageAvatars(String json);

    public native void updateStandAvatars(String json);

//    public native void onVoiceWaveHappened(String json);

    public native void backOffStageAvatar(int id);
    public native void backOffStandAvatar(int id);

    public native void releaseResource();

}
