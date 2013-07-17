  /*=================================================================}
 /  <<<<<<<<<<<<<<<<<<<<<--- Harsh Engine --->>>>>>>>>>>>>>>>>>>>>>  }
/  e-mail  : xproger@list.ru                                         }
{  github  : https://github.com/xproger/harsh                        }
{====================================================================}
{ LICENSE:                                                           }
{ Copyright (c) 2013, Timur "XProger" Gagiev                         }
{ All rights reserved.                                               }
{                                                                    }
{ Redistribution and use in source and binary forms, with or without /
{ modification, are permitted under the terms of the BSD License.   /
{=================================================================*/
#include <stdio.h>
#include <Carbon/Carbon.h>
#include <OpenGL/OpenGL.h>
#include <AGL/agl.h>
#include "core.h"
#include "game.h"

int WIDTH = 800;
int HEIGHT = 600;

WindowRef window;
bool isQuit = false;
size_t startTime = 0;

int remapKey(uint32 key) {
	switch (key) {
		case 0x24 : return 13;
		case 0x30 : return 9;
		case 0x31 : return 32;
		case 0x33 : return 8;
		case 0x35 : return 27;
		case 0x7B : return 37;
		case 0x7C : return 39;
		case 0x7D : return 40;
		case 0x7E : return 38;
		case 0x72 : return 45;
		case 0x75 : return 46;
		case 0x73 : return 36;
		case 0x77 : return 35;
		case 0x74 : return 33;
		case 0x79 : return 34;
		case 0x3B : return 17;
		case 0x3A : return 18;
		case 0x38 : return 16;
		case 0x00 : return 'A';
		case 0x0B : return 'B';
		case 0x08 : return 'C';
		case 0x02 : return 'D';
		case 0x0E : return 'E';
		case 0x03 : return 'F';
		case 0x05 : return 'G';
		case 0x04 : return 'H';
		case 0x22 : return 'I';
		case 0x26 : return 'J';
		case 0x28 : return 'K';
		case 0x25 : return 'L';
		case 0x2E : return 'M';
		case 0x2D : return 'N';
		case 0x1F : return 'O';
		case 0x23 : return 'P';
		case 0x0C : return 'Q';
		case 0x0F : return 'R';
		case 0x01 : return 'S';
		case 0x11 : return 'T';
		case 0x20 : return 'U';
		case 0x09 : return 'V';
		case 0x0D : return 'W';
		case 0x07 : return 'X';
		case 0x10 : return 'Y';
		case 0x06 : return 'Z';
	}
	return 255;
}

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
			
		case kEventClassMouse :
			EventMouseButton mouseButton;
			CGPoint mousePos;
			Rect wndRect;
			
			GetEventParameter(event, kEventParamMouseLocation, typeHIPoint, NULL, sizeof(mousePos), NULL, &mousePos);
			GetEventParameter(event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(mouseButton), nil, &mouseButton);			
			
			GetWindowBounds(window, kWindowContentRgn, &wndRect);
			mousePos.x -= wndRect.left;
			mousePos.y -= wndRect.top;
			
			switch (eventKind) {
				case kEventMouseDown	:
				case kEventMouseUp		:
					Core::touch(mouseButton - 1, (eventKind == kEventMouseUp) ? TOUCH_UP : TOUCH_DOWN, (int)mousePos.x, (int)mousePos.y);
					break;
				case kEventMouseDragged	:
					if (mouseButton == 1)
						Core::touch(0, TOUCH_MOVE, (int)mousePos.x, (int)mousePos.y);
					break;
			}
			break;
			
		case kEventClassKeyboard :

			uint32 key = 255;
			GetEventParameter(event, kEventParamKeyCode, typeUInt32, NULL, sizeof(key), NULL, &key);

			switch (eventKind) {
				case kEventRawKeyDown	:
				case kEventRawKeyUp		:
					Core::touch(remapKey(key), (eventKind == kEventRawKeyUp) ? TOUCH_KEYUP : TOUCH_KEYDOWN, 0, 0);
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
	kEventClassKeyboard, kEventRawKeyDown,
	kEventClassKeyboard, kEventRawKeyUp,
	kEventClassMouse, kEventMouseDown,
	kEventClassMouse, kEventMouseUp,
	kEventClassMouse, kEventMouseDragged,
};

int getTime() {
	UInt64 t;
	Microseconds((UnsignedWide*)&t);
	return (int)(t / 1000 - startTime);
}

int main() {
// init window
	Rect rect = {0, 0, (short)HEIGHT, (short)WIDTH};
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
	InstallEventHandler(GetApplicationEventTarget(), (EventHandlerUPP)eventHandler, 8, (EventTypeSpec*)&events, NULL, NULL);
	
	char path[1024];
	CFBundleRef bundle	= CFBundleGetMainBundle();
	CFURLRef bundleURL	= CFBundleCopyBundleURL(bundle);
	CFStringRef pathStr	= CFURLCopyFileSystemPath(bundleURL, kCFURLPOSIXPathStyle);
	CFStringGetFileSystemRepresentation(pathStr, (char*)&path, 1024);
	strcat(path, "/Contents/Resources/data.jet");
	
	startTime = getTime();
	Core::init((char*)&path, getTime);
	Core::resize(WIDTH, HEIGHT);
	Core::reset();
	Core::resume();

	EventRecord event;
	while (!isQuit) {
		while (GetNextEvent(0xffff, &event)) {};
		Core::update();
		Core::render();
		aglSwapBuffers(context);
	}
	
	Core::deinit();
	
	aglSetCurrentContext(NULL);
	ReleaseWindow(window);
	
	return 0;
}
