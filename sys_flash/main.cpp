#include <AS3/AS3.h>
#include <Flash++.h>
#include <sys/time.h>
#include "core.h"

using namespace AS3::ui;

size_t startTime;

int getTime() {
	timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000) + (time.tv_usec / 1000) - startTime;
}

extern "C" void touchProc(int id, int state, int x, int y) {
	Core::touch(id, state, x, y);
}

static var enterFrame(void *arg, var as3Args) {
	try {
		Core::update();
		Core::render();
	Render::context3D->present();
	} catch(var e) {
		char *err = internal::utf8_toString(e);
		LOG("Exception: %s\n", err);
		free(err);
	}
	return internal::_undefined;
}

static var context3DError(void *arg, var as3Args) {
	LOG("context3DError\n");
}

static var initContext3D(void *arg, var as3Args)
{
	Render::context3D = Render::stage3D->context3D;
	Render::context3D->enableErrorChecking = true;
	Render::context3D->configureBackBuffer(Render::stage->stageWidth, Render::stage->stageHeight, 0, true, false);

	Core::init("data.jet", getTime);
	Core::resize(Render::stage->stageWidth, Render::stage->stageHeight);
	Core::reset();
	Core::resume();

	Render::stage->addEventListener(flash::events::Event::ENTER_FRAME, Function::_new(enterFrame, NULL));

	return internal::_undefined;
}

int main() {
	Render::stage = internal::get_Stage();
	Render::stage->scaleMode = flash::display::StageScaleMode::NO_SCALE;
	Render::stage->align = flash::display::StageAlign::TOP_LEFT;
	Render::stage->frameRate = 60;

	Render::stage3D = var(var(Render::stage->stage3Ds)[0]);
	Render::stage3D->addEventListener(flash::events::Event::CONTEXT3D_CREATE, Function::_new(initContext3D, NULL));
	Render::stage3D->addEventListener(flash::events::ErrorEvent::ERROR, Function::_new(context3DError, NULL));
	Render::stage3D->requestContext3D(flash::display3D::Context3DRenderMode::AUTO, flash::display3D::Context3DProfile::BASELINE_CONSTRAINED);

	startTime = 0;
	startTime = size_t(getTime());

	AS3_GoAsync();
}