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
#ifndef GAME_H
#define GAME_H

#define NO_SOUND

#include "scene.h"

struct Game {
	Camera *camera;

    Game();
    ~Game();
    void pause();
    void update();
    void render();
    void touch(int id, int state, int x, int y);
};

#endif // GAME_H
