#include <stdio.h>
#include <Carbon/Carbon.h>
#include <OpenGL/OpenGL.h>
#include <AGL/agl.h>
#include "core.h"
#include "game.h"

int WIDTH = 1280;
int HEIGHT = 720;

WindowRef window;
bool isQuit = false;
int startTime = 0;

OSStatus eventHandler(EventHandlerCallRef handler, EventRef event, void* userData) {
	OSType eventClass	= GetEventClass(event);
	UInt32 eventKind	= GetEventKind(event);
	
	switch (eventClass) {
		case kEventClassWindow :
			switch (eventKind) {
				case kEventWindowClosed :
					isQuit = true;
					break;
				case kEventWindowActivated :
					//Core::resume();
					break;
				case kEventWindowDeactivated :
				case kEventWindowCollapseAll :
					//Core::pause();
					break;
			}
			break;
		case  kEventClassMouse :
			EventMouseButton mouseButton;
			CGPoint mousePos;
			Rect wndRect;
			
			GetEventParameter(event, kEventParamMouseLocation, typeHIPoint, NULL, sizeof(mousePos), NULL, &mousePos);
			GetEventParameter(event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(mouseButton), nil, &mouseButton);			
			
			GetWindowBounds(window, kWindowContentRgn, &wndRect);
			mousePos.x -= wndRect.left;
			mousePos.y -= wndRect.top;
			
			switch (eventKind) {
				case kEventMouseDown :
				case kEventMouseUp :
					Core:touch(mouseButton - 1, (eventKind == kEventMouseUp) ? TOUCH_UP : TOUCH_DOWN, (int)mousePos.x, (int)mousePos.y);
					break;
				case kEventMouseDragged :
					if (mouseButton == 1)
						Core::touch(0, TOUCH_MOVE, (int)mousePos.x, (int)mousePos.y);
					break;
			}

			break;
	}
	
	return CallNextEventHandler(handler, event);
}

const int events[] = {
	kEventClassWindow, kEventWindowClosed,
	kEventClassWindow, kEventWindowActivated,
	kEventClassWindow, kEventWindowDeactivated,
	kEventClassMouse, kEventMouseDown,
	kEventClassMouse, kEventMouseUp,
	kEventClassMouse, kEventMouseDragged,
};

int getTime() {
	UInt64 t;
	Microseconds((UnsignedWide*)&t);
	return t / 1000 - startTime;
}

int main() {
// init window
	Rect rect;
	SetRect(&rect, 0, 0, WIDTH, HEIGHT);
	CreateNewWindow(kDocumentWindowClass, kWindowCloseBoxAttribute | kWindowCollapseBoxAttribute | kWindowStandardHandlerAttribute, &rect, &window);

// init OpenGL context
	GLint attribs[] = {
		AGL_RGBA,
		AGL_DOUBLEBUFFER,
		AGL_SAMPLES_ARB,	0,
		AGL_BUFFER_SIZE,	32,
		AGL_DEPTH_SIZE,		24,
		AGL_STENCIL_SIZE,	8,
		AGL_NONE
	};
	AGLPixelFormat format = aglChoosePixelFormat(NULL, 0, (GLint*)&attribs);
	AGLContext context = aglCreateContext(format, NULL);
	aglDestroyPixelFormat(format);	
	
	aglSetDrawable(context, GetWindowPort(window));
	aglSetCurrentContext(context);	
	
// show window
	SelectWindow(window);
	ShowWindow(window);
	InstallEventHandler(GetApplicationEventTarget(), (EventHandlerUPP)eventHandler, 6, (EventTypeSpec*)&events, NULL, NULL);
	
	char path[1024];
	CFBundleRef bundle	= CFBundleGetMainBundle();
	CFURLRef bundleURL	= CFBundleCopyBundleURL(bundle);
	CFStringRef pathStr	= CFURLCopyFileSystemPath(bundleURL, kCFURLPOSIXPathStyle);
	CFStringGetFileSystemRepresentation(pathStr, (char*)&path, 1024);
	strcat(path, "/Contents/Resources/data.pak");
	
	startTime = getTime();
	Fuse::init((char*)&path, new Game(), getTime);
	Fuse::reset();
	Fuse::resume();
	Fuse::resize(WIDTH, HEIGHT);

	EventRecord event;
	while (!isQuit) {
		while (GetNextEvent(0xffff, &event)) {};
		if (!Fuse::paused) {
			Fuse::render();
			aglSwapBuffers(context);
		}
	}
	
	Fuse::free();
	
	aglSetCurrentContext(NULL);
    ReleaseWindow(window);
	
	return 0;
}
