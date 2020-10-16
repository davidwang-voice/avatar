//
//  AudioManager.cpp
//  videoPlayer
//
//  Created by radio8712 on 2/24/14.
//  Copyright (c) 2014 Dill Pixel. All rights reserved.
//

#include "AudioManager.h"

#define kHMute	"kHMute"

static AudioManager *sharedManager = NULL;

void AudioManager::playBG() {
	if (!isMute) {
		SimpleAudioEngine::getInstance()->playBackgroundMusic("backgroundMusic.mp3", true);
	}
};

void AudioManager::stopBG() {
	if (!isMute) {
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	}
};

void AudioManager::unMute() {
	SimpleAudioEngine::getInstance()->setEffectsVolume(1.0f);
	SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(1.0f);
};

void AudioManager::toggleMute() {
	isMute = !isMute;
	if (isMute) {
		stopBG();
	} else {
		playBG();
	}
	save();
};

void AudioManager::save() {
	UserDefault::getInstance()->setBoolForKey(kHMute, isMute);
	UserDefault::getInstance()->flush();
};

AudioManager *AudioManager::getInstance() {
	if (!sharedManager) {
		sharedManager = new AudioManager();
		UserDefault *uDef;
		bool mute;
		uDef = UserDefault::getInstance();
		mute = uDef->getBoolForKey(kHMute, false);
		sharedManager->isMute = mute;
		sharedManager->isMute = false;
	}
	return sharedManager;
};