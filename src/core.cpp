#include "core.h"
#include "render.h"
#include "resource.h"
#include "scene.h"

//{ Input ---------------------------------------------
void Input::reset() {
	memset(this, 0, sizeof(Input));
}

void Input::process(int id, int state, int x, int y) {
	switch (state) {
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
	}
}
//}

//{ Core ----------------------------------------------
float Core::deltaTime;
int Core::lastTime, Core::fpsTime, Core::fps;
getTimePtr Core::getTime;
Input	*Core::input;
Scene	*Core::scene;
Game	*Core::game;

void Core::init(const char *path, getTimePtr getTime) {
	LOG("Core::init\n");
    Core::getTime = getTime;
    Stream::init(path);
    Render::init();
    Resource::init();
    Sound::init();
    input = new Input();
    scene = new Scene();
    game = new Game();
    fpsTime = getTime();
    lastTime = fpsTime;
    fps = 0;
}

void Core::deinit() {
	LOG("Core::deinit\n");
    delete game;
    delete scene;
    delete input;
	Sound::deinit();
    Resource::deinit();
    Render::deinit();
}

void Core::pause() {
	game->pause();
}

void Core::resume() {
	LOG("Core::resume\n");
	lastTime = getTime();
	Resource::loading();
}

void Core::reset() {
	LOG("Core::reset\n");
    Resource::invalidate();
    input->reset();
}

void Core::resize(int width, int height) {
    Render::resize(width, height);
}

void Core::update() {
    Resource::gc();

	int time = getTime();
	float dt = (time - lastTime) * 0.001f;
	lastTime = time;

	while (dt > _EPS) {
	    deltaTime = _min(0.004f, dt); // 0.004 s - maximum deltaTime per update
        game->update();
        dt -= deltaTime;
	}
}

void Core::render() {
	Render::resetStates();
    game->render();

// fps meter
    fps++;
    int time = getTime();
    if (time >= fpsTime) {
        fpsTime = time + 1000;
        LOG("fps: %d\n", fps);
        fps = 0;
    }
}

void Core::touch(int id, int state, int x, int y) {
	input->process(id, state, x, y);
    game->touch(id, state, x, y);
//    LOG("state: %d id: %d\n", state, id);
}
//}
