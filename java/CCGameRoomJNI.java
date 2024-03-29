package com.iandroid.allclass.ccgame.room;

import android.util.Log;

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
        void onTouchStageAvatar(String uid);

        void onTouchStandAvatar(String uid);

        void onTouchScene();
    }


    private volatile CCListener ccListener;

    public void setCCListener(CCListener listener) {
        this.ccListener = null;
        this.ccListener = listener;
    }

    public CCListener getCCListener() {
        return this.ccListener;
    }

    //native call.
    public static void onTouchStageAvatar(String uid) {
        Log.i(TAG, "onTouchedAvatar id: " + uid);
        if (getInstance().getCCListener() != null)
            getInstance().getCCListener().onTouchStageAvatar(uid);
    }

    public static void onTouchStandAvatar(String uid) {
        Log.i(TAG, "onTouchedAvatar id: " + uid);
        if (getInstance().getCCListener() != null)
            getInstance().getCCListener().onTouchStandAvatar(uid);
    }


    public static void onTouchScene() {
        Log.i(TAG, "onTouchedScene");
        if (getInstance().getCCListener() != null)
            getInstance().getCCListener().onTouchScene();
    }

    public native void init();

    public native int[] getGLContextAttrs();


    /**
     * 设置房间类型
     * @param roomType 房间类型 0 = 音频， 1 = 视频
     * @param topPixel 视频播放器距离屏幕顶部的距离 单位px
     */
    public native void setRoomType(int roomType, float topPixel);

    /**
     * 设置场景分辨率 默认1080 内部自适应屏幕宽度
     */
    public native void setDesignResolution(float width, float height);

    /**
     * 设置房间背景
     */
    public native void setStageBackground(String url);

    /**
     * 初始化房间礼物堆
     * @param json = "[{"type": 1, "urls": "xxx.png,xxx.png,...", "count": 5, "extraUrl":"", "extraType":3}, ...]"
     */
    public native void setupStageGiftHeap(String json);

    /**
     * 当前用户更新avatar形象
     * @param json = "{"uid": "100001", "name": "david", "url": "xxx.png", "rare": 1, "guard": 1}"
     */
    public native void updateSelfAvatar(String json);

    /**
     * 指定用户更新avatar形象
     * @param json = "{"uid": "100001", "name": "david", "url": "xxx.png", "rare": 1, "guard": 1}"
     */
    public native void updateTargetAvatar(String json);

    /**
     * 更新连麦舞台所有avatar形象
     * @param json = "[{"uid":"100001", "name": "david", "url": "xxx.png", "rare": 1, "guard": 1, "offline": 1, "mute": true}, ...]"
     */
    public native void updateStageAvatars(String json);

    /**
     * 更新普通站台所有avatar形象
     * @param json = "[{"uid":"100001", "name": "david", "url": "xxx.png", "rare": 1, "guard": 1}, ...]"
     */
    public native void updateStandAvatars(String json);

    /**
     * 退出连麦舞台
     */
    public native void backOffStageAvatar(String uid);

    /**
     * 退出普通站台
     */
    public native void backOffStandAvatar(String uid);

    /**
     * 清空指定类型礼物池
     * @param type 1=小 2=中 3=大
     */
    public native void clearTargetGiftPool(int type);

    /**
     * 收到用户送礼消息
     * @param json = "{"type": 1, "uid":"100001", "count": 20, "urls": "xxx.png,xxx.png,...", "extraUrl":"", "extraType":3}"
     */
    public native void receiveGiftMessage(String json);

    /**
     * 收到用户聊天消息
     */
    public native void receiveChatMessage(String uid, String content);

    /**
     * 收到用户提醒消息
     */
    public native void receiveInfoMessage(String uid, String content);

    /**
     * 收到用户聊天贴图
     */
    public native void receiveChatPicture(String uid, String url);

    /**
     * 收到麦位主播声纹
     * @param uids = "100001,100002,..."
     */
    public native void receiveVoiceWave(String uids);

    /**
     * 随机用户打呼动效
     * @param uids = "100001,100002,..."
     */
    public native void receiveRandomSnore(String uids);

    /**
     * 释放房间资源
     */
    public native void releaseResource();

}
