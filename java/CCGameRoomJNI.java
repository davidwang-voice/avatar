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
        void onTouchedAvatar(String uid);

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
    public static void onTouchedAvatar(String uid) {
        Log.i(TAG, "onTouchedAvatar id: " + uid);
        if (getInstance().getCCListener() != null)
            getInstance().getCCListener().onTouchedAvatar(uid);
    }

    public static void onTouchedScene() {
        Log.i(TAG, "onTouchedScene");
        if (getInstance().getCCListener() != null)
            getInstance().getCCListener().onTouchedScene();
    }

    public native int[] getGLContextAttrs();

    public native void setDesignResolution(float width, float height);

    public native void setStageBackground(String path);

    public native void setupStageGiftHeap(String json);

    public native void updateSelfAvatar(String json);

    public native void updateStageAvatars(String json);

    public native void updateStandAvatars(String json);

    public native void backOffStageAvatar(String uid);

    public native void backOffStandAvatar(String uid);

    public native void receiveGiftMessage(String uid, String path);

    public native void receiveChatMessage(String uid, String content);

    public native void receiveVoiceWave(String uids);


    public native void releaseResource();

}
