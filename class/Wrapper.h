//
//  Wrapper.h
//  videoPlayer
//
//  Created by radio8712 on 2/24/14.
//  Copyright (c) 2014 Dill Pixel. All rights reserved.
//

#ifndef __videoPlayer__Wrapper__
#define __videoPlayer__Wrapper__

#include "cocos2d.h"
#include "AudioManager.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "iOSWrapper.h"
#endif

using namespace cocos2d;

class Wrapper {
public:
	// Instance Methods
	void playVideo();

	// Static Methods
	static Wrapper *getInstance();
	
};

#endif /* defined(__videoPlayer__Wrapper__) */
