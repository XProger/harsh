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
#ifndef CORE_H
#define CORE_H

#include "game.h"
#include "utils.h"

typedef int (*getTimePtr)(void);

enum TouchState { TOUCH_DOWN = 0, TOUCH_UP, TOUCH_MOVE, TOUCH_KEYDOWN, TOUCH_KEYUP };

struct Touch {
	vec2 start, pos;
	bool down;
};

struct Mouse {
	vec2 pos;
};

struct Input {
	Touch	touch[8];
	Mouse	mouse;
	bool	key[256];

	void reset();
	void process(int id, int state, int x, int y);
};

struct Core {
    static float deltaTime;
    static int lastTime, fpsTime, fps;

    static Input *input;
	static Scene *scene;
    static Game *game;

	static getTimePtr getTime;
    static void init(const char *path, getTimePtr getTime);
    static void deinit();
    static void pause();
    static void resume();
    static void reset();
    static void resize(int width, int height);
    static void update();
    static void render();
    static void touch(int id, int state, int x, int y);
};

#endif // CORE_H
