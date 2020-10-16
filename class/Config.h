//
//  Config.h
//  videoPlayer
//
//  Created by radio8712 on 2/24/14.
//  Copyright (c) 2014 Dill Pixel. All rights reserved.
//

#ifndef __videoPlayer__Config__
#define __videoPlayer__Config__

#include "cocos2d.h"

using namespace cocos2d;

class Config {
public:
	// Instance Properties

	// DEVICE SPECIFIC
	// ***************
	float screenWidth;
	float screenHeight;
	float scaleFactor;


	// Static Methods
	static Config *getInstance();
};


#endif /* defined(__videoPlayer__Config__) */
