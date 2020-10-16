//
//  iOSPlayer.m
//  videoPlayer
//
//  Created by radio8712 on 2/24/14.
//  Copyright (c) 2014 Dill Pixel. All rights reserved.
//

#import <MediaPlayer/MediaPlayer.h>
#import "iOSPlayer.h"


DPiOSPlayer *playerInstance = nil;

@implementation DPiOSPlayer

- (void)playVideo:(NSURL *)url withView:(UIView *) view{
	dpPlayer = [[MPMoviePlayerController alloc] initWithContentURL:url];
	[[dpPlayer view] setFrame: [view bounds]];
	[dpPlayer setFullscreen:YES];
	[dpPlayer setScalingMode:MPMovieScalingModeNone];
	[dpPlayer setControlStyle:MPMovieControlStyleFullscreen];
	[view addSubview:[dpPlayer view]];
	[dpPlayer play];

	[[NSNotificationCenter defaultCenter] addObserver:self
																					 selector:@selector(exitFullscreen)
																							 name:MPMoviePlayerDidExitFullscreenNotification
																						 object:nil];

	[[NSNotificationCenter defaultCenter] addObserver:self
																					 selector:@selector(videoEnded)
																							 name:MPMoviePlayerPlaybackDidFinishNotification
																						 object:nil];
}

- (void)exitFullscreen {
	[self removeVideo];
}

- (void)videoEnded {
	[self removeVideo];
}

- (void)removeVideo {
	if ([dpPlayer playbackState] == MPMoviePlaybackStatePaused) {
		[[dpPlayer view] removeFromSuperview];
		dpPlayer = nil;

		[[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerDidExitFullscreenNotification object:nil];
		[[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerPlaybackDidFinishNotification object:nil];
		AudioManager::getInstance()->playBG();
	}
}

+ (DPiOSPlayer *)getInstance {
	if (!playerInstance) {
		playerInstance = [[DPiOSPlayer alloc] init];
	}
	return playerInstance;
}

@end
