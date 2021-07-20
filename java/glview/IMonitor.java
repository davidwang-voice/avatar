package org.cocos2dx.lib.glview;

/**
 * Created by david on 2021/7/20.
 * Interface definition for a callback to be invoked when a alpha video is played or
 * an error occurred during playback
 */
interface IMonitor {
    /**
     * Called when a alpha video is played or an error occurred during playback
     *
     * @param result    the result of alpha video playback
     * @param playType  the type of alpha player impl
     * @param what      the type of error that has occurred:
     * @param extra     an extra code, specific to the error. Typically
     *                  implementation dependent.
     * @param errorInfo detail error information
     */
    void monitor(boolean result, String playType, int what, int extra, String errorInfo);
}

