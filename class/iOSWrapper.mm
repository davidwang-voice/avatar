//
//  iOSWrapper.cpp
//  videoPlayer
//
//  Created by radio8712 on 2/24/14.
//  Copyright (c) 2014 Dill Pixel. All rights reserved.
//

// Obj-C headers
#import <MediaPlayer/MediaPlayer.h>
#import "EAGLView.h"
#import "iOSWrapper.h"
#import "iOSPlayer.h"

static iOSWrapper *instance = NULL;

void iOSWrapper::playVideo() {
//	NSString *stringPath = [NSString stringWithUTF8String:vidPath];
//	NSString *string = [[NSBundle mainBundle] pathForResource:stringPath ofType:@"mp4"];
//	NSURL *url;
	// Local File
//	url = [NSURL fileURLWithPath:string];

	// HTTP Download Link
	//	url = [NSURL URLWithString:@"http://ff1d56741af732f1488c-5d3ef947316355c15b99158374bf2058.r4.cf2.rackcdn.com/video.mp4"];

	// HTTPS Download Link
	//	url = [NSURL URLWithString:@"https://34b9c2f4f9bb74a4743b-5d3ef947316355c15b99158374bf2058.ssl.cf2.rackcdn.com/video.mp4"];

	// Streaming Link
	//	url = [NSURL URLWithString:@"http://ad8179187e2cf7e7cbe3-5d3ef947316355c15b99158374bf2058.r4.stream.cf2.rackcdn.com/video.mp4"];

	// iOS Streaming Link
	//	url = [NSURL URLWithString:@"http://0be1262fe8a25a0164ca-5d3ef947316355c15b99158374bf2058.iosr.cf2.rackcdn.com/video.mp4"];

//	[[DPiOSPlayer getInstance] playVideo:url withView:[CCEAGLView sharedEGLView]];
};

iOSWrapper *iOSWrapper::getInstance() {
	if (!instance) {
		instance = new iOSWrapper();
	}
	return instance;
};

