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

#include <windows.h>
#include <winsock.h>
#include "core.h"

bool quit = false;

const int WIDTH	 = 800;
const int HEIGHT = 600;

const char * TITLE = "Harsh Engine";	
LARGE_INTEGER timeFreq, startTime;

int getTime() {
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return int(1000 * (time.QuadPart - startTime.QuadPart) / timeFreq.QuadPart);
}

#ifndef NO_SOUND
#define SND_SIZE 8192

HWAVEOUT waveOut;
WAVEFORMATEX waveFmt = {WAVE_FORMAT_PCM, 2, 22050, 22050 * 4, 4, 16, sizeof(waveFmt)};
WAVEHDR waveBuf[2];
char *waveData;
CRITICAL_SECTION waveCS;

void CALLBACK soundFill(HWAVEOUT waveOut, WAVEHDR *waveBuf) {
	EnterCriticalSection(&waveCS);
	waveOutUnprepareHeader(waveOut, waveBuf, sizeof(WAVEHDR));
	Sound::fill((SoundFrame*)(waveBuf->lpData), SND_SIZE / 4);
	waveOutPrepareHeader(waveOut, waveBuf, sizeof(WAVEHDR));
	waveOutWrite(waveOut, waveBuf, sizeof(WAVEHDR));
	LeaveCriticalSection(&waveCS);
}

void soundInit(HWND hwnd) {
	InitializeCriticalSection(&waveCS);
	if (waveOutOpen(&waveOut, 0xffffffff, &waveFmt, (DWORD)hwnd, 0, CALLBACK_WINDOW) == 0) {
		waveData = new char[SND_SIZE * 2];
		memset(&waveBuf, 0, sizeof(waveBuf));
		for (int i = 0; i < 2; i++) {
			waveBuf[i].dwBufferLength = SND_SIZE;
			waveBuf[i].lpData = (char*)((int)waveData + SND_SIZE * i);
			soundFill(waveOut, &waveBuf[i]);
		}
	} else
		waveData = NULL;
}

void soundFree() {
	EnterCriticalSection(&waveCS);
	waveOutUnprepareHeader(waveOut, &waveBuf[0], sizeof(WAVEHDR));
	waveOutUnprepareHeader(waveOut, &waveBuf[1], sizeof(WAVEHDR));
	waveOutReset(waveOut);
	waveOutClose(waveOut);
	delete[] waveData;
	waveData = NULL;
	LeaveCriticalSection(&waveCS);
	DeleteCriticalSection(&waveCS);
}
#endif

InputKey convKey(int key) {
	if (key < 0 || key > 255) return IK_NONE;
	static const unsigned char keys[256] = {
		0,0,0,0,0,0,0,0, IK_BACK, IK_TAB, 0,0,0, IK_ENTER, 0,0,
		IK_SHIFT, IK_CTRL, IK_ALT, 0,0,0,0,0,0,0,0, IK_ESC, 0,0,0,0,
		IK_SPACE, IK_PGUP, IK_PGDOWN, IK_END, IK_HOME, IK_LEFT, IK_UP, IK_RIGHT, IK_DOWN, IK_INS, IK_DEL,
		IK_0, IK_1, IK_2, IK_3, IK_4, IK_5, IK_6, IK_7, IK_8, IK_9, IK_A, IK_B, IK_C, IK_D, IK_E, IK_F,
		IK_G, IK_H, IK_I, IK_J, IK_K, IK_L, IK_M, IK_N, IK_O, IK_P, IK_Q, IK_R, IK_S, IK_T, IK_U, IK_V,
		IK_W, IK_X, IK_Y, IK_Z, 0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0, IK_PLUS, IK_MINUS, 0,0,0,
		IK_TILDE, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};
	return (InputKey)keys[key];
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		#ifndef NO_SOUND
			case MM_WOM_DONE :
				soundFill((HWAVEOUT)wParam, (WAVEHDR*)lParam);
				break;
		#endif
		case WM_ACTIVATEAPP :
			if (short(wParam)) {
				Core::resume();
				#ifndef NO_SOUND
					waveOutRestart(waveOut);
				#endif
			} else {
				Core::pause();
				#ifndef NO_SOUND
					waveOutPause(waveOut);
				#endif
			}
			break;
		case WM_DESTROY :
			quit = true; 
			break;
		case WM_SIZE :
			Render::resize(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_MOUSEMOVE :
			Core::inputEvent( InputEvent(IS_MOVE, IK_MOUSE, wParam & MK_LBUTTON ? 0 : (wParam & MK_RBUTTON ? 1 : 2), LOWORD(lParam), HIWORD(lParam)) );
			break;
		case WM_LBUTTONDOWN :
		case WM_LBUTTONUP :
		case WM_LBUTTONDBLCLK :
			Core::inputEvent( InputEvent(message == WM_LBUTTONUP ? IS_UP : IS_DOWN, IK_MOUSE, 0, LOWORD(lParam), HIWORD(lParam)) );
			break;
		case WM_RBUTTONDOWN :
		case WM_RBUTTONUP :
		case WM_RBUTTONDBLCLK :
			Core::inputEvent( InputEvent(message == WM_RBUTTONUP ? IS_UP : IS_DOWN, IK_MOUSE, 1, LOWORD(lParam), HIWORD(lParam)) );
			break;
		case WM_MBUTTONDOWN :
		case WM_MBUTTONUP :
		case WM_MBUTTONDBLCLK :
			Core::inputEvent( InputEvent(message == WM_MBUTTONUP ? IS_UP : IS_DOWN, IK_MOUSE, 2, LOWORD(lParam), HIWORD(lParam)) );
			break;
		case WM_MOUSEWHEEL :
			Core::input->mouse.wheel += (wParam >> 16) / 120;
			break;
			/*
		case WM_TOUCH :
			count = LOWORD(wParam);
			touch = new TOUCHINPUT[count];
			if (GetTouchInputInfo((HTOUCHINPUT)lParam, count, touch, sizeof(TOUCHINPUT))) {
				for (int i = 0; i < count; i++) {
					TOUCHINPUT &t = touch[i];
					if (!(t.dwFlags & (TOUCHEVENTF_MOVE | TOUCHEVENTF_DOWN | TOUCHEVENTF_UP)))
						continue;
					InputState state = (t.dwFlags & TOUCHEVENTF_MOVE ? IS_MOVE : (
										t.dwFlags & TOUCHEVENTF_DOWN ? IS_DOWN : IS_UP));
					LOG("touch %d %d %d %d\n", (int)state, (int)t.dwID, (int)t.x, (int)t.y);
				}
				CloseTouchInputHandle((HTOUCHINPUT)lParam);
			}
			delete touch;
			break;
			*/
		case WM_CHAR :
			Core::inputEvent( InputEvent((char)wParam) );
			break;
		case WM_KEYDOWN :
		case WM_KEYUP :
			Core::inputEvent( InputEvent(message == WM_KEYUP ? IS_UP : IS_DOWN, convKey(wParam)) );
			break;
		default:
			return DefWindowProcA(hWnd, message, wParam, lParam);
	}
	return 0;
}

int main(int argc, char *argv[]) {
	const long style = WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	RECT r;
	SetRect(&r, 0, 0, WIDTH, HEIGHT);
	AdjustWindowRect(&r, style, false);
	HWND handle = CreateWindowA("STATIC", TITLE, style, 0, 0, r.right - r.left, r.bottom - r.top, NULL, NULL, NULL, NULL);
	SetWindowLongA(handle, GWL_WNDPROC, LONG(&WndProc));

	HDC dc = GetDC(handle);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize			= sizeof(pfd);
	pfd.nVersion		= 1;
	pfd.dwFlags			= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cColorBits		= 32;
	pfd.cAlphaBits		= 8;
	pfd.cDepthBits		= 24;
	pfd.cStencilBits	= 8;

	SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd);
	HGLRC rc = wglCreateContext(dc);
	wglMakeCurrent(dc, rc);

	QueryPerformanceFrequency(&timeFreq);
	QueryPerformanceCounter(&startTime);

	WSAData wData;
	WSAStartup(0x0101, &wData);

	Render::resize(WIDTH, HEIGHT);
	Core::init("data.jet", getTime);
	Core::reset();
	Core::resume();
	Core::resize(WIDTH, HEIGHT);
	Core::update();
#ifndef NO_SOUND
	soundInit(handle);
#endif
	MSG msg;
	while (!quit)
		if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		} else {
		#ifndef NO_SOUND
			EnterCriticalSection(&waveCS);
			Core::update();
			LeaveCriticalSection(&waveCS);
		#else
			Core::update();
		#endif
			Core::render();
			SwapBuffers(dc);
		}
	Core::deinit();
#ifndef NO_SOUND
	soundFree();
#endif
	WSACleanup();

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(rc);
	ReleaseDC(handle, dc);
	DestroyWindow(handle);

	ExitProcess(0);
	return 0;
}
