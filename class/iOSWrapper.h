//
//  iOSWrapper.h
//  videoPlayer
//
//  Created by radio8712 on 2/24/14.
//  Copyright (c) 2014 Dill Pixel. All rights reserved.
//

#ifndef __videoPlayer__iOSWrapper__
#define __videoPlayer__iOSWrapper__

#include "cocos2d.h"
#include "AudioManager.h"
#include "Config.h"

using namespace cocos2d;

class iOSWrapper: public Object {
public:
	// Instance Methods
	void playVideo();

	// Static Methods
	static iOSWrapper *getInstance();
};

#endif /* defined(__videoPlayer__iOSWrapper__) */
