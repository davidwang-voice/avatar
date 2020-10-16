//
//  Wrapper.cpp
//  videoPlayer
//
//  Created by radio8712 on 2/24/14.
//  Copyright (c) 2014 Dill Pixel. All rights reserved.
//

#include "Wrapper.h"

static Wrapper *instance = NULL;

void Wrapper::playVideo() {
	AudioManager::getInstance()->stopBG();
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
	iOSWrapper::getInstance()->playVideo();
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	Application::getInstance()->playVideo();
#endif
};

Wrapper *Wrapper::getInstance() {
	if (!instance) {
		instance = new Wrapper();
	}
	return instance;
};