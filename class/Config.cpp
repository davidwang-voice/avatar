//
//  Config.cpp
//  videoPlayer
//
//  Created by radio8712 on 2/24/14.
//  Copyright (c) 2014 Dill Pixel. All rights reserved.
//

#include "Config.h"

static Config *instance = NULL;

Config *Config::getInstance() {
	if (!instance) {
		instance = new Config();
	}
	return instance;
};
