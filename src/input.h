#ifndef INPUT_H
#define INPUT_H

#include "utils.h"

enum InputState { 
	IS_DOWN, IS_UP, IS_MOVE
};

enum InputKey {
	IK_NONE, IK_CHAR, IK_MOUSE, IK_TOUCH, IK_JOY, 
	IK_0, IK_1, IK_2, IK_3, IK_4, IK_5, IK_6, IK_7, IK_8, IK_9,
	IK_A, IK_B, IK_C, IK_D, IK_E, IK_F, IK_G, IK_H, IK_I, IK_J, IK_K, IK_L, IK_M,
	IK_N, IK_O, IK_P, IK_Q, IK_R, IK_S, IK_T, IK_U, IK_V, IK_W, IK_X, IK_Y, IK_Z,
	IK_PLUS, IK_MINUS, IK_TILDE, IK_ESC, IK_ENTER, IK_BACK, IK_TAB, IK_SHIFT, IK_CTRL, IK_ALT,
	IK_SPACE, IK_PGUP, IK_PGDOWN, IK_END, IK_HOME, IK_LEFT, IK_UP, IK_RIGHT, IK_DOWN, IK_INS, IK_DEL,
	IK_MAX
};

struct InputMouse {
	vec2 pos;
	float wheel;
	bool down[3];	// left, right, middle
};

struct InputTouch {
	vec2 start, pos;
	bool down;
};

struct InputJoy {
	float pov, x, y, z, r, u, v;
	bool down[16];	// buttons
	bool active;	// gamepad is plugged
};

struct InputEvent {
	InputKey key;
	InputState state;
	
	union {
		char value;
		struct { int btn, x, y; } mouse;
		struct { int id, x, y; } touch;
		struct { int btn; } joy;
	} info;

	InputEvent() {}
	InputEvent(InputState state, InputKey key) : state(state), key(key) {}
	InputEvent(char value) : state(IS_DOWN), key(IK_CHAR) {
		info.value = value;
	}
	InputEvent(InputState state, InputKey key, int id, int x, int y) : state(state), key(key) {
		info.touch.id = id; 
		info.touch.x = x;
		info.touch.y = y;
	}
	InputEvent(InputState state, InputKey key, int btn) : state(state), key(key) {
		info.joy.btn = btn;
	}
};

struct Input {
	char *text;
	bool down[IK_MAX];

	InputMouse		mouse;
	InputTouch		touch[10];
	InputJoy		joy[4];

	void Input::reset() { 
		memset(this, 0, sizeof(Input));
	}

	void Input::inputEvent(const InputEvent &e) {
		switch (e.key) {
			case IK_CHAR :
				//
				break;
			case IK_MOUSE :
				if (e.state != IS_MOVE)
					mouse.down[e.info.mouse.btn] = e.state == IS_DOWN;
				mouse.pos = vec2(e.info.mouse.x, e.info.mouse.y);
				break;
			case IK_TOUCH : {
					InputTouch &t = touch[e.info.touch.id];
					if (e.state != IS_MOVE)
						t.down = e.state == IS_DOWN;
					t.pos = vec2(e.info.touch.x, e.info.touch.y);
					break;
				}
			case IK_JOY :
				//
				break;
			default :
				if (e.state != IS_MOVE)
					down[e.key] = e.state == IS_DOWN;
	/*
			case TOUCH_DOWN :
				touch[id].start = touch[id].pos = vec2((float)x, (float)y);
				touch[id].down = true;
				break;
			case TOUCH_UP :
				touch[id].down = false;
				break;
			case TOUCH_MOVE :
				touch[id].pos = vec2((float)x, (float)y);
				break;
			case TOUCH_KEYDOWN :
			case TOUCH_KEYUP   :
				key[id] = state == TOUCH_KEYDOWN;
				break;
	*/
		}
	}
};

#endif INPUT_H