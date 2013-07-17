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
#include "game.h"
#include "core.h"
#include "render.h"
#include "utils.h"
#include "scene.h"

Game::Game() {
// add camera
	camera = new Camera();
	camera->FOV   = 75.0f;
	camera->zNear = 0.1f;
	camera->zFar  = 1000;
	camera->mode  = CAMERA_MODE_FREE;
	camera->pos   = vec3(0, 5.0f, 0.1f);
	camera->angle = vec3(_PI/6, 0, 0);
	camera->dist  = 2.0f;

	Core::scene->camera = camera;
	Core::scene->load("scene.xsc");
}

Game::~Game() {
	//
}

void Game::pause() {
	//
}

void Game::update() {
	//camera->pos = car->matrix.getPos();
	camera->debugUpdate(10);
}

void Game::render() {
	Render::clear(CLEAR_ALL, 0.0f, 0.5f, 0.8f, 1.0f);
	Core::scene->render();
}

void Game::touch(int id, int state, int x, int y) {
	//
}
