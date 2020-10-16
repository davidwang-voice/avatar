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


/**
 * Created by david on 2020/10/14.
 */
public class CCGameRoomImpl implements CCGameRoomView {

    public interface Callback {
        void onTouchAvatar(int id);

        void onTouchScene();
    }

    private Context context;
    private CCGameRoomImpl.Callback callback;
    private CCGameRoomJNI gameRoomJNI = CCGameRoomJNI.getInstance();

    private Handler mainHandler = new Handler(Looper.getMainLooper());

    private int[] gLContextAttrs;
    private Cocos2dxGLSurfaceView glSurfaceView;

    private boolean hasFocus = false;
    private boolean gainAudioFocus = false;


    public CCGameRoomImpl(Context context, FrameLayout parent) {
        this.context = context;

        Cocos2dxHelper.init((Activity) context);
        gameRoomJNI.setDesignResolution(1080, 1920);
        gLContextAttrs = gameRoomJNI.getGLContextAttrs();

        glSurfaceView = new Cocos2dxGLSurfaceView(context);
        glSurfaceView.setZOrderMediaOverlay(true);

        // this line is need on some device if we specify an alpha bits
        if(this.gLContextAttrs[3] > 0) glSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        // use custom EGLConfigureChooser
        CCEGLConfigChooser chooser = new CCEGLConfigChooser(this.gLContextAttrs);
        glSurfaceView.setEGLConfigChooser(chooser);
        glSurfaceView.setCocos2dxRenderer(new Cocos2dxRenderer());
//        glSurfaceView.setOnTouchDetector(event -> {});
        parent.addView(glSurfaceView, new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT));

        Cocos2dxEngineDataManager.init(context.getApplicationContext(), glSurfaceView);

        gameRoomJNI.setCCListener(new CCGameRoomJNI.CCListener() {
            @Override
            public void onTouchedAvatar(int id) {
                runOnUiThread(() -> {
                    if (callback != null) callback.onTouchAvatar(id);
                });
            }

            @Override
            public void onTouchedScene() {
                runOnUiThread(() -> {
                    if (callback != null) callback.onTouchScene();
                });
            }
        });
    }

    public void setCallback(CCGameRoomImpl.Callback callback) {
        this.callback = callback;
    }


    @Override
    public void focusChanged(boolean focus) {
        this.hasFocus = focus;
        resumeIfHasFocus();
    }

    @Override
    public void resume() {
        if(gainAudioFocus)
            Cocos2dxAudioFocusManager.registerAudioFocusListener(context);
        resumeIfHasFocus();
        Cocos2dxEngineDataManager.resume();
    }

    @Override
    public void pause() {
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
        runOnGLThread(() -> gameRoomJNI.releaseResource());
    }

    @Override
    public void presentGift(int userId, String url) {
        runOnGLThread(() -> gameRoomJNI.receiveGiftMessage(userId, url));
    }

    @Override
    public void presentChat(int userId, String content) {
        runOnGLThread(() -> gameRoomJNI.receiveChatMessage(userId, content));
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
    public void leaveStage(int userId) {
        runOnGLThread(() -> gameRoomJNI.backOffStageAvatar(userId));
    }

    @Override
    public void leaveStand(int userId) {
        runOnGLThread(() -> gameRoomJNI.backOffStandAvatar(userId));
    }


    private void resumeIfHasFocus() {
        //It is possible for the app to receive the onWindowsFocusChanged(true) event
        //even though it is locked or asleep
        boolean readyToPlay = !isDeviceLocked(context) && !isDeviceAsleep(context);

        if(hasFocus && readyToPlay) {
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
