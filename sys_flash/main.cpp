#include <math.h>
#include <sys/time.h>
#include <iostream>
#include "core.h"

using namespace AS3::ui;
	
int getTime() {
    timeval time;
	gettimeofday(&time, NULL);
    return time.tv_sec * 1000 + time.tv_usec / 1000;
}

static var enterFrame(void *arg, var as3Args) {
    try {
		Core::update();
		Core::render();
		Render::context3D->present();
    } catch(var e) {
		std::cout << "Exception: " << std::string(internal::utf8_toString(e)) << std::endl;
    }
	return internal::_undefined;
}

static var mouseDownProc(void *arg, var as3Args)
{
/*
  static int count = 0;
  flash::display::Sprite mySprite = *(var *)arg;
  mySprite->alpha = mySprite->alpha * 0.9;
*/
	int x = 0, y = 0;
	printf("mouseDown");
	Core::touch(0, TOUCH_DOWN, x, y);
	return internal::_undefined;
}

static var initContext3D(void *arg, var as3Args) {
	Render::context3D = Render::stage3D->context3D;
	Render::context3D->enableErrorChecking = true;
    Render::context3D->configureBackBuffer(Render::stage->stageWidth, Render::stage->stageHeight, 0, false, false);

	Core::init("data.jet", getTime);
	Core::reset();
	Core::resize(Render::stage->stageWidth, Render::stage->stageHeight);
//	Core::resume();
	Render::stage->addEventListener(flash::events::MouseEvent::MOUSE_DOWN, Function::_new(mouseDownProc, NULL), false, 0, false);
    Render::stage->addEventListener(flash::events::Event::ENTER_FRAME, Function::_new(enterFrame, NULL), false, 0, false);

    return internal::_undefined;
}

int main() {
    Render::stage = internal::get_Stage();
    Render::stage->scaleMode = flash::display::StageScaleMode::NO_SCALE;
    Render::stage->align = flash::display::StageAlign::TOP_LEFT;
    Render::stage->frameRate = 60;

    Render::stage3D = var(var(Render::stage->stage3Ds)[0]);
	
    Render::stage3D->addEventListener(flash::events::Event::CONTEXT3D_CREATE,
		Function::_new(initContext3D, NULL), false, 0, false);
	
    Render::stage3D->requestContext3D(flash::display3D::Context3DRenderMode::AUTO,
		flash::display3D::Context3DProfile::BASELINE_CONSTRAINED);

    AS3_GoAsync();
//	Core::deinit();
}