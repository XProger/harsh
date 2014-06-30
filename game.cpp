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

#include <gl/GL.h>

Mesh *mesh, *edge;

Game::Game() {
// add camera
	camera = new Camera();
	camera->FOV   = 45.0f;
	camera->zNear = 0.1f;
	camera->zFar  = 100;
	camera->mode  = CAMERA_MODE_GAME;
	camera->pos   = vec3(0.0f, 50.0f, 0.0f);
	camera->angle = vec3(-_PI*0.5f, 0, 0);
	camera->dist  = 2.0f;

	Core::scene->camera = camera;

	mesh = new Mesh(Core::scene, Stream::getHash("mesh/fury.xms"));
	mesh->material = new Material(Stream::getHash("material/fury.xmt"));

	edge = new Mesh(Core::scene, Stream::getHash("mesh/fury_edge.xms"));
	edge->material = Core::scene->mEdge;

	Core::scene->add(edge);
	Core::scene->add(mesh);

	Render::params.light.pos = vec3(100, 1000, 100);
	Render::params.edge = vec4(0, 0, 0, 0.05);

//	Core::scene->load("scene.xsc");
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
	//mesh->matrix.rotate(Core::deltaTime * _PI/4.0f, vec3(0, 1, 0));
}

void Game::render() {
	Render::clear(CLEAR_ALL, 0.0f, 0.5f, 0.8f, 1.0f);
	Core::scene->render();
	
	Render::setShader(0);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)&Core::scene->camera->mProj);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&Core::scene->camera->mView);
	glBegin(GL_LINES);
		glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(1, 0, 0);
		glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 1, 0);
		glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1);
	glEnd();
}

void Game::touch(int id, int state, int x, int y) {
	if (state == TOUCH_KEYUP && id == 32)
		Core::scene->camera->mode = Core::scene->camera->mode == CAMERA_MODE_FREE ? CAMERA_MODE_GAME : CAMERA_MODE_FREE;
	if (id == 0 && Core::scene->camera->mode == CAMERA_MODE_GAME) {
		mesh->matrix.setPos(Core::scene->camera->toWorld(vec2(x, Render::height - y), vec4(0, 1, 0, 0)));
		edge->matrix = mesh->matrix;
	}
}
