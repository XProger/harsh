#import "ViewController.h"
#include "core.h"
#include "resource.h"

@interface ViewController ()
@property (strong, nonatomic) EAGLContext *context;
@end

@implementation ViewController

@synthesize context = _context;

- (void)viewDidLoad {	
	[super viewDidLoad];

	self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

	if (!self.context)
		NSLog(@"Failed to create ES context");

	GLKView *view = (GLKView *)self.view;
	view.context = self.context;
	self.preferredFramesPerSecond = 60;
	view.drawableDepthFormat = GLKViewDrawableDepthFormat16;
	view.drawableColorFormat = GLKViewDrawableColorFormatRGB565;
	[EAGLContext setCurrentContext:self.context];
	
	self.view.multipleTouchEnabled = NO;
}

- (void)viewDidUnload {	
	[super viewDidUnload];
	Core::deinit();
	if ([EAGLContext currentContext] == self.context)
		[EAGLContext setCurrentContext:nil];
	self.context = nil;
}

- (void)didReceiveMemoryWarning {
	[super didReceiveMemoryWarning];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	return (interfaceOrientation == UIDeviceOrientationPortrait) |
		   (interfaceOrientation == UIDeviceOrientationPortraitUpsideDown);
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
	float scale = [[UIScreen mainScreen] scale];
	Core::resize(self.view.bounds.size.width * scale, self.view.bounds.size.height * scale);
	Core::update();
	Core::render();
}
- (void)getTouch:(UIEvent *)event :(int)state {
	NSSet* touchSet = [event allTouches];
	float scale = [[UIScreen mainScreen] scale];
	int i = 0;
	for (id touch in touchSet) {
		CGPoint p = [touch locationInView:self.view];
		Core::touch(i++, state, p.x * scale, p.y * scale);
	}	
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {	
	[self getTouch:event:TOUCH_DOWN];	
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	[self getTouch:event:TOUCH_MOVE];	
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {	
	[self getTouch:event:TOUCH_UP];	
}

@end
