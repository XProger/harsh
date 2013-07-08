#import "AppDelegate.h"
#import "ViewController.h"
#include "core.h"
#include "resource.h"
#include <AudioToolbox/AudioQueue.h>
#include <AudioToolbox/AudioFile.h>
#include <AudioToolbox/AudioServices.h>
#include <AudioToolbox/AudioConverter.h>
#include <AudioToolbox/AudioToolbox.h>
#include <mach/mach_time.h>

uint64_t startTime = 0;
mach_timebase_info_data_t s_timebase_info;

static int getTime() {
	return int((double)mach_absolute_time() * (double)s_timebase_info.numer / (double)s_timebase_info.denom / 1e6);
}

#define SND_BUF_SIZE 8192	// можно и 4096, что в 2 раза сократит latency, но в симуляторе будут слышны щелчки

static AudioQueueRef audioQueue;

void soundFill(void* inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer) {
	void* frames = inBuffer->mAudioData;
	UInt32 count = inBuffer->mAudioDataBytesCapacity / 4;
	Sound::fill((SoundFrame*)frames, count);
	inBuffer->mAudioDataByteSize = count * 4;
	AudioQueueEnqueueBuffer(audioQueue, inBuffer, 0, NULL);
}

void soundInit() {
	AudioStreamBasicDescription deviceFormat;
	deviceFormat.mSampleRate 		= 22050;
	deviceFormat.mFormatID 			= kAudioFormatLinearPCM;
	deviceFormat.mFormatFlags 		= kLinearPCMFormatFlagIsSignedInteger;
	deviceFormat.mBytesPerPacket	= 4;
	deviceFormat.mFramesPerPacket	= 1;
	deviceFormat.mBytesPerFrame		= 4;
	deviceFormat.mChannelsPerFrame	= 2;
	deviceFormat.mBitsPerChannel	= 16;
	deviceFormat.mReserved			= 0;

	AudioQueueNewOutput(&deviceFormat, soundFill, NULL, CFRunLoopGetCurrent(),
						kCFRunLoopCommonModes, 0, &audioQueue);

	for (int i = 0; i < 2; i++) {
		AudioQueueBufferRef mBuffer;
		AudioQueueAllocateBuffer(audioQueue, SND_BUF_SIZE, &mBuffer);
		soundFill(NULL, audioQueue, mBuffer);
	}
	AudioQueueStart(audioQueue, NULL);
}

@implementation AppDelegate

@synthesize window = _window;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
	[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
	
	NSString *packPath = [[NSBundle mainBundle] pathForResource:@"data" ofType:@"jet"];	

	mach_timebase_info(&s_timebase_info);
	startTime = mach_absolute_time();
	Core::init([packPath UTF8String], getTime);
	soundInit();
	
	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	self.window.rootViewController = [[ViewController alloc] init];
	[self.window makeKeyAndVisible];
	
	return YES;
}
							
- (void)applicationWillResignActive:(UIApplication *)application {
	//
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
	Core::pause();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
	//
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
	Core::reset();
	Core::resume();
}

- (void)applicationWillTerminate:(UIApplication *)application {
	AudioQueueDispose(audioQueue, true);
}

@end
