package com.iandroid.allclass.ccgame.room;

import android.app.Activity;
import android.app.KeyguardManager;
import android.content.Context;
import android.graphics.PixelFormat;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.os.PowerManager;
import android.widget.FrameLayout;



import org.cocos2dx.lib.Cocos2dxAudioFocusManager;
import org.cocos2dx.lib.Cocos2dxEngineDataManager;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.cocos2dx.lib.Cocos2dxHelper;
import org.cocos2dx.lib.Cocos2dxRenderer;

import java.util.concurrent.CountDownLatch;


/**
 * Created by david on 2020/10/14.
 */
public class CCGameRoomImpl implements CCGameRoomView {


    private Context context;
    private CCGameRoomJNI gameRoomJNI = CCGameRoomJNI.getInstance();

    private Handler mainHandler = new Handler(Looper.getMainLooper());

    private int[] gLContextAttrs;
    private Cocos2dxGLSurfaceView glSurfaceView;

    private boolean hasFocus = false;
    private boolean gainAudioFocus = false;
    private boolean isPaused = false;
    private FrameLayout parentView;


    public CCGameRoomImpl(Context context, FrameLayout parent) {
        this.context = context.getApplicationContext();
        this.parentView = parent;
        Cocos2dxHelper.init((Activity) context);

        gameRoomJNI.setDesignResolution(1125, 1);
        gLContextAttrs = gameRoomJNI.getGLContextAttrs();

        glSurfaceView = new Cocos2dxGLSurfaceView(parent.getContext());
        glSurfaceView.setOpaque(false);

//        glSurfaceView.setZOrderOnTop(true);
//        glSurfaceView.setZOrderMediaOverlay(true);

        // this line is need on some device if we specify an alpha bits
//        if(this.gLContextAttrs[3] > 0)
//            glSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        // use custom EGLConfigureChooser
        CCEGLConfigChooser chooser = new CCEGLConfigChooser(this.gLContextAttrs);
        glSurfaceView.setEGLConfigChooser(chooser);
        glSurfaceView.setCocos2dxRenderer(new Cocos2dxRenderer());
        glSurfaceView.setMultipleTouchEnabled(false);

        runOnGLThread(() -> gameRoomJNI.init());

//        glSurfaceView.setOnTouchDetector(event -> {});

        FrameLayout.LayoutParams layoutParams;
        int parentWidth = parent.getWidth();
        int parentHeight = parent.getHeight();
        if (parentWidth > 0 && parentHeight > 0 && parentHeight > parentWidth) {
            layoutParams = new FrameLayout.LayoutParams(parentWidth, parentHeight);
        } else {
            layoutParams = new FrameLayout.LayoutParams(
                    FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);
        }

        parent.addView(glSurfaceView, layoutParams);

        Cocos2dxEngineDataManager.init(context.getApplicationContext(), glSurfaceView);
    }

    @Override
    public void setCallback(Callback callback) {
        gameRoomJNI.setCCListener(new CCGameRoomJNI.CCListener() {
            @Override
            public void onTouchStageAvatar(String uid) {
                runOnUiThread(() -> {
                    if (callback != null) callback.onTouchStageAvatar(uid);
                });
            }

            @Override
            public void onTouchStandAvatar(String uid) {
                runOnUiThread(() -> {
                    if (callback != null) callback.onTouchStandAvatar(uid);
                });
            }

            @Override
            public void onTouchScene() {
                runOnUiThread(() -> {
                    if (callback != null) callback.onTouchRoomScene();
                });
            }
        });
    }

    @Override
    public void focusChanged(boolean focus) {
        this.hasFocus = focus;
        resumeIfHasFocus();
    }

    @Override
    public void resume() {
        isPaused = false;
        if(gainAudioFocus)
            Cocos2dxAudioFocusManager.registerAudioFocusListener(context);
        resumeIfHasFocus();
//        Cocos2dxHelper.onResume();
//        glSurfaceView.onResume();

        Cocos2dxEngineDataManager.resume();
    }

    @Override
    public void pause() {
        isPaused = true;
        if(gainAudioFocus)
            Cocos2dxAudioFocusManager.unregisterAudioFocusListener(context);
        Cocos2dxHelper.onPause();
        glSurfaceView.onPause();
        Cocos2dxEngineDataManager.pause();
    }

    @Override
    public void release() {
        if(gainAudioFocus)
            Cocos2dxAudioFocusManager.unregisterAudioFocusListener(context);
        Cocos2dxEngineDataManager.destroy();

        CountDownLatch lock = new CountDownLatch(1);
        runOnGLThread(() -> {
            gameRoomJNI.releaseResource();
            lock.countDown();
        });
        try {
            lock.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        if (parentView != null && glSurfaceView != null) {
            parentView.removeView(glSurfaceView);
        }
    }

    @Override
    public void setRoomType(int type, float topPixel) {
        runOnGLThread(() -> gameRoomJNI.setRoomType(type, topPixel));
    }

    @Override
    public void setBackground(String url) {
        runOnGLThread(() -> gameRoomJNI.setStageBackground(url));
    }

    @Override
    public void setupGiftHeap(String json) {
        runOnGLThread(() -> gameRoomJNI.setupStageGiftHeap(json));
    }

    @Override
    public void setSelfAvatar(String json) {
        runOnGLThread(() -> gameRoomJNI.updateSelfAvatar(json));
    }

    @Override
    public void setTargetAvatar(String json) {
        runOnGLThread(() -> gameRoomJNI.updateTargetAvatar(json));
    }

    @Override
    public void presentGift(String json) {
        runOnGLThread(() -> gameRoomJNI.receiveGiftMessage(json));
    }

    @Override
    public void clearGiftPool(int type) {
        runOnGLThread(() -> gameRoomJNI.clearTargetGiftPool(type));
    }

    @Override
    public void presentChat(String userId, String content) {
        runOnGLThread(() -> gameRoomJNI.receiveChatMessage(userId, content));
    }

    @Override
    public void presentInfo(String userId, String content) {
        runOnGLThread(() -> gameRoomJNI.receiveInfoMessage(userId, content));
    }

    @Override
    public void presentPicture(String userId, String url) {
        runOnGLThread(() -> gameRoomJNI.receiveChatPicture(userId, url));
    }

    @Override
    public void syncStageUsers(String json) {
        runOnGLThread(() -> gameRoomJNI.updateStageAvatars(json));
    }

    @Override
    public void syncStandUsers(String json) {
        runOnGLThread(() -> gameRoomJNI.updateStandAvatars(json));
    }

    @Override
    public void onVoiceWave(String userIds) {
        runOnGLThread(() -> gameRoomJNI.receiveVoiceWave(userIds));
    }

    @Override
    public void onRandomSnore(String userIds) {
        runOnGLThread(() -> gameRoomJNI.receiveRandomSnore(userIds));
    }

    @Override
    public void leaveStage(String userId) {
        runOnGLThread(() -> gameRoomJNI.backOffStageAvatar(userId));
    }

    @Override
    public void leaveStand(String userId) {
        runOnGLThread(() -> gameRoomJNI.backOffStandAvatar(userId));
    }

    private void resumeIfHasFocus() {
        //It is possible for the app to receive the onWindowsFocusChanged(true) event
        //even though it is locked or asleep
        boolean readyToPlay = !isDeviceLocked(context) && !isDeviceAsleep(context);

        if(hasFocus && readyToPlay && !isPaused) {
            Cocos2dxHelper.onResume();
            glSurfaceView.onResume();
        }
    }


    private void runOnGLThread(Runnable runnable) {
        this.glSurfaceView.queueEvent(runnable);
    }

    private void runOnUiThread(Runnable runnable) {
        this.mainHandler.post(runnable);
    }


    private static boolean isDeviceLocked(Context context) {
        KeyguardManager keyguardManager = (KeyguardManager) context.getSystemService(Context.KEYGUARD_SERVICE);
        boolean locked = keyguardManager.inKeyguardRestrictedInputMode();
        return locked;
    }

    private static boolean isDeviceAsleep(Context context) {
        PowerManager powerManager = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        if (powerManager == null) {
            return false;
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT_WATCH) {
            return !powerManager.isInteractive();
        } else {
            return !powerManager.isScreenOn();
        }
    }
}
