#include <iostream>
#include <vector>
#include <math.h>
#include <AS3/AS3.h>
#include <Flash++.h>
#include <sys/time.h>

// as3wig.jar was used to create a C++ wrapper for the AS3 code in the
// AGALMiniAssemblerwhich we use to compile AGAL asm into AGAL bytecode that can
// be uploaded to the GPU. Look at the makefile to see how to invoke as3wig.jar
// You can use it to wrap any AS3 code you want to access from C++.
#include "AGAL.h"
#include "core.h"

// We're going to be using UI worker references in this example (see sample 2
// for more information on the difference between UI and loca worker
// references).
using namespace AS3::ui;

// Some global vars we'll use in various functions.
//flash::display3D::IndexBuffer3D i3dbuffer;
//flash::display3D::VertexBuffer3D v3dbuffer;

/*
IndexBuffer *iBuffer;
VertexBuffer *vBuffer;
Texture *tex;
*/

size_t startTime;

int getTime() {
	timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000) + (time.tv_usec / 1000) - startTime;
}

extern "C" void touchProc(int id, int state, int x, int y) {
	LOG("touch %d %d %d %d\n", id, state, x, y );
	Core::touch(id, state, x, y);
}


// This function will be attached to the ENTER_FRAME event to drive the
// animation.
static var enterFrame(void *arg, var as3Args) {
    static int tc = 0;
    try {
	/*
		Render::clear(CLEAR_ALL, 0, 1, 0, 0);
		mat4 m = mat4(Core::getTime() * 0.01, vec3(0, 0, 1));
		Render::context3D->setProgramConstantsFromByteArray(AS3::ui::flash::display3D::Context3DProgramType::VERTEX, 0, 16, AS3::ui::internal::get_ram(), (unsigned)&m, (void*)&m);
		if (tex->bind(0)) 
			Render::drawTriangles(iBuffer, vBuffer, 0, 1);
		else
			LOG("no texture\n");
	*/
		Core::update();
		Core::render();
        Render::context3D->present();
    } catch(var e) {
        char *err = internal::utf8_toString(e);
        std::cout << "Exception: " << err << std::endl;
        free(err);
    }
    return internal::_undefined;
}

/*
			stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
			stage.addEventListener(KeyboardEvent.KEY_UP, onKeyUp);
			stage.addEventListener(MouseEvent.MOUSE_MOVE, onMouseMove);
			stage.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDown);
			stage.addEventListener(MouseEvent.MOUSE_UP, onMouseUp);
			
    public function bufferMouseMove(me:MouseEvent) {
      me.stopPropagation()
      mx = me.stageX
      my = me.stageY
    }
    
    public function bufferMouseDown(me:MouseEvent) 
    {
      me.stopPropagation();
      mx = me.stageX;
      my = me.stageY;
      button = 1;
    }
    
    public function bufferMouseUp(me:MouseEvent) 
    {
      me.stopPropagation();
      mx = me.stageX;
      my = me.stageY;
      button = 0;
    }

    public function bufferKeyDown(ke:KeyboardEvent) 
    {
      if(Keyboard.capsLock || ke.keyCode >= 127)
        return;

      keybytes.writeByte(int(ke.keyCode & 0x7F));
    }
    
    public function bufferKeyUp(ke:KeyboardEvent) 
    {
      if(Keyboard.capsLock || ke.keyCode > 128)
        return;

      keybytes.writeByte(int(ke.keyCode | 0x80));
    }
*/

// If we fail to create the Context3D we display a warning
static var context3DError(void *arg, var as3Args)
{
    flash::text::TextFormat fmt = flash::text::TextFormat::_new();
    fmt->size = internal::new_int(24);
    fmt->align = flash::text::TextFormatAlign::CENTER;

    flash::text::TextField tf = flash::text::TextField::_new();
    tf->defaultTextFormat = fmt;
    tf->width = Render::stage->stageWidth;
    tf->height = Render::stage->stageHeight;
    tf->multiline = true;
    tf->wordWrap = true;
    tf->text = 
        "\nUnable to create a Stage3D context. Usually this means you ran the swf "
        "directly in a web browser, use the HTML wrapper instead so the wmode "
        "gets set correctly to 'direct'.";

    Render::stage->addChild(tf);
}

// After a Context3D is created this function will be called.
static var initContext3D(void *arg, var as3Args)
{
	printf("initContext3D\n");
		
	Render::context3D = Render::stage3D->context3D;
	Render::context3D->enableErrorChecking = true;
    Render::context3D->configureBackBuffer(Render::stage->stageWidth, Render::stage->stageHeight, 0, true, false);

	Core::init("data.jet", getTime);
	Core::resize(Render::stage->stageWidth, Render::stage->stageHeight);
	Core::reset();
	Core::resume();


    com::adobe::utils::AGALMiniAssembler vasm = com::adobe::utils::AGALMiniAssembler::_new(false);
    vasm->assemble(flash::display3D::Context3DProgramType::VERTEX,
                   "m44 vt0, va0, vc0\n \
                   mov vt1.xy, va3.xy\n \
				   mul vt1.zw, va3.zw, vc8.zw\n \
				   add vt1.zw, vt1.zw, vc8.xy\n \
				   mov v0, vc8\n \
				   mov op, vt0\n");

    com::adobe::utils::AGALMiniAssembler fasm = com::adobe::utils::AGALMiniAssembler::_new(false);
    fasm->assemble(flash::display3D::Context3DProgramType::FRAGMENT,
                   "tex ft0, v0.xy, fs0 <2d,dxt1,repeat,linear,miplinear>\n \
				    tex ft2, v0.zw, fs2 <2d,dxt1,repeat,linear,miplinear>\n \
					mov ft3.x, v0.y\n \
					mov ft3.yzw, fc0.zzz\n \
					mov oc, ft3\n");
					//mul oc, ft0, ft2\n");

    flash::display3D::Program3D program = Render::context3D->createProgram();
    program->upload(vasm->agalcode, fasm->agalcode);
    Render::context3D->setProgram(program);
/*	
	short idata[] = {
		2,1,0
	};
	
	float vdata[] = {
		-0.5, -0.5, 0, 0, 0, 0, 0,
		0, 0.5, 0, 1, 0, 0, 0,
		0.5, -0.5, 0, 1, 1, 0, 0
	};
	

	iBuffer = new IndexBuffer(idata, 3, IF_SHORT);
	vBuffer = new VertexBuffer(vdata, 3, VF_PT34);

	tex = new Texture("texture/check.pvr");
*/

	/*
	
    i3dbuffer = Render::context3D->createIndexBuffer(3);
    int indicies[] = {
    	2,1,0
    };
    i3dbuffer->uploadFromByteArray(internal::get_ram(), (int)&indicies[0], 0, 3, (void*)&indicies[0]);

    v3dbuffer = Render::context3D->createVertexBuffer(3, 3);
    float verticies[] = {
    	-0.5, -0.5, 0,
        0, 0.5, 0,
        0.5, -0.5, 0,
    };
    v3dbuffer->uploadFromByteArray(internal::get_ram(), (int)&verticies[0], 0, 3, (void*)&verticies[0]);
    Render::context3D->setVertexBufferAt(0, v3dbuffer, 0, flash::display3D::Context3DVertexBufferFormat::FLOAT_3);

	com::adobe::utils::PerspectiveMatrix3D projection =
        com::adobe::utils::PerspectiveMatrix3D::_new(internal::_null);
    projection->perspectiveFieldOfViewLH(45.0*M_PI/180.0, 1.2, 0.1, 512);
    Render::context3D->setProgramConstantsFromMatrix(flash::display3D::Context3DProgramType::VERTEX, 0, projection, false);
*/



	vec4 v = vec4(0.0, 0.5, 1.0, 2.0);
	void *value = &v;
	Render::context3D->setProgramConstantsFromByteArray(AS3::ui::flash::display3D::Context3DProgramType::FRAGMENT, 0, 1, AS3::ui::internal::get_ram(), (unsigned)value, (void*)value);

	/*
    var vc = internal::new_Vector_Number();
    vc[0] = internal::new_Number(0.5);
    vc[1] = internal::new_Number(0.5);
    vc[2] = internal::new_Number(0.5);
    vc[3] = internal::new_Number(0.5);
    Render::context3D->setProgramConstantsFromVector(flash::display3D::Context3DProgramType::FRAGMENT, 0, vc, -1);
	*/
    Render::stage->addEventListener(flash::events::Event::ENTER_FRAME, Function::_new(enterFrame, NULL));

    return internal::_undefined;
}

int main() {
    Render::stage = internal::get_Stage();
    Render::stage->scaleMode = flash::display::StageScaleMode::NO_SCALE;
    Render::stage->align = flash::display::StageAlign::TOP_LEFT;
    Render::stage->frameRate = 60;

    Render::stage3D = var(var(Render::stage->stage3Ds)[0]);

//	FILE *f = fopen("data.jet", "rb");
//	if (!f) return 0;
//	fclose(f);
	
    // Ask for a Stage3D context to be created
    Render::stage3D->addEventListener(flash::events::Event::CONTEXT3D_CREATE, Function::_new(initContext3D, NULL));
    Render::stage3D->addEventListener(flash::events::ErrorEvent::ERROR, Function::_new(context3DError, NULL));
    Render::stage3D->requestContext3D(flash::display3D::Context3DRenderMode::AUTO,
                          flash::display3D::Context3DProfile::BASELINE_CONSTRAINED);

	startTime = 0;
	startTime = size_t(getTime());	
    // Suspend main() and return to the Flash runloop
    AS3_GoAsync();
}