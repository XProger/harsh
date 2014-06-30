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

#include "utils.h"
#include "input.h"
#include "scene.h"
#include "game.h"

typedef int (*getTimePtr)(void);

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

    static void inputEvent(const InputEvent &e);
};

#endif // CORE_H
