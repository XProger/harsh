#import "ViewController.h"
#include "core.h"
#include "resource.h"

@interface ViewController ()
@property (strong, nonatomic) EAGLContext *context;
@end

@implementation ViewController

@synthesize context = _context;

- (id)init
{
    self = [super init];
	[[NSMutableDictionary alloc] init];
    m_touchList = [[NSMutableArray alloc] init];
    for (int i=0; i<10; i++)
        [m_touchList addObject:[NSNull null]];
    return self;
}

- (void)dealloc
{
	//    [touchIDList release];
    [m_touchList release];
    [super dealloc];
}

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
	
	self.view.multipleTouchEnabled = YES;
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

/*
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
 */

- (NSUInteger) getTouchId:(UITouch*)touch {
    NSUInteger i = [m_touchList indexOfObject:touch];
    if (i != NSNotFound)
        return i;
    
    for (i = 0; i < 10; i++) {
        if ([m_touchList objectAtIndex:i] == [NSNull null]) {
            [m_touchList setObject:touch atIndexedSubscript:i];
            return i;
        }
    }
    
    return NSNotFound;
}

- (void) checkTouchId:(UITouch*)touch {
    NSUInteger i = [m_touchList indexOfObject:touch];
    if (i != NSNotFound)
        [m_touchList setObject:[NSNull null] atIndexedSubscript:i];
}

- (void) doTouch:(UIEvent*)event :(int)state {
    NSSet* touchSet = [event allTouches];
    for (UITouch *touch in touchSet) {
        int touchState;
        switch (touch.phase) {
            case UITouchPhaseBegan:
                touchState = int(TOUCH_DOWN);
                break;
            case UITouchPhaseMoved:
                touchState = int(TOUCH_MOVE);
                break;
            case UITouchPhaseEnded:
                touchState = int(TOUCH_UP);
                break;
           // case UITouchPhaseCancelled:
                //touchState = int(TOUCH_CANCEL);
             //   break;
            default:
                touchState = -1;
                break;
        }
        
        if (touchState == state) {
            NSUInteger tid = [self getTouchId:touch];
            float scale = [[UIScreen mainScreen] scale];
            CGPoint p = [touch locationInView:self.view];
            Core::touch(tid, state, p.x * scale, p.y * scale);
            
            if (state == TOUCH_UP)// || state == TOUCH_CANCEL)
                [self checkTouchId:touch];
        }
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	[self doTouch:event:TOUCH_DOWN];
    
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	[self doTouch:event:TOUCH_MOVE];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	[self doTouch:event:TOUCH_UP];
}
/*
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self doTouch:event :TOUCH_CANCEL];
}
*/
@end
