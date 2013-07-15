#include "render.h"
#include "utils.h"

#ifdef S3D

BlendMode		Render::m_blending;
CullMode		Render::m_culling;
bool			Render::m_depthWrite, Render::m_depthTest, Render::m_alphaTest;
int				Render::width, Render::height, Render::m_active_sampler;
TextureObj		Render::m_active_texture[8];
ShaderObj		Render::m_active_shader;
RenderParams	Render::params;
int				Render::statSetTex, Render::statTriCount;
VertexBuffer	*Render::m_vbuffer;
IndexBuffer		*Render::m_ibuffer;

AS3::ui::flash::display::Stage Render::stage;
AS3::ui::flash::display::Stage3D Render::stage3D;
AS3::ui::flash::display3D::Context3D Render::context3D;

// IndexBuffer -----------------------------------------------
IndexBuffer::IndexBuffer(void *data, int count, IndexFormat format) : count(count), format(format) {
	if (format != IF_SHORT) {
		LOG("unsupported iFormat (%d)\n", format);
		return;
	}
	obj = Render::context3D->createIndexBuffer(count);
    obj->uploadFromByteArray(AS3::ui::internal::get_ram(), (int)&data[0], 0, count, (void*)&data[0]);
}

IndexBuffer::~IndexBuffer() {
	obj->dispose();
}

void IndexBuffer::bind() {
	//
}

// VertexBuffer ----------------------------------------------
VertexBuffer::VertexBuffer(void *data, int count, VertexFormat format) : count(count), format(format) {
	obj = Render::context3D->createVertexBuffer(count, VertexStride[format] / 4);
	obj->uploadFromByteArray(AS3::ui::internal::get_ram(), (int)&data[0], 0, count, (void*)&data[0]);
}

VertexBuffer::~VertexBuffer() {
	obj->dispose();
}

void VertexBuffer::bind() {
	switch (format) {
		case VF_P3 :
			Render::context3D->setVertexBufferAt(vaCoord, obj, 0, AS3::ui::flash::display3D::Context3DVertexBufferFormat::FLOAT_3);
			break;
		case VF_PT22 :
			Render::context3D->setVertexBufferAt(vaTexCoord0, obj, 2, AS3::ui::flash::display3D::Context3DVertexBufferFormat::FLOAT_2);
			Render::context3D->setVertexBufferAt(vaCoord, obj, 0, AS3::ui::flash::display3D::Context3DVertexBufferFormat::FLOAT_3);
			break;
		case VF_PT32 :
			Render::context3D->setVertexBufferAt(vaTexCoord0, obj, 3, AS3::ui::flash::display3D::Context3DVertexBufferFormat::FLOAT_2);
			Render::context3D->setVertexBufferAt(vaCoord, obj, 0, AS3::ui::flash::display3D::Context3DVertexBufferFormat::FLOAT_3);
			break;
		case VF_PT34 :
			Render::context3D->setVertexBufferAt(vaTexCoord0, obj, 3, AS3::ui::flash::display3D::Context3DVertexBufferFormat::FLOAT_4);
			Render::context3D->setVertexBufferAt(vaCoord, obj, 0, AS3::ui::flash::display3D::Context3DVertexBufferFormat::FLOAT_3);
			break;
		default :
			LOG("unsupported vFormat (%d)\n", format);
			return;
	}
}

// Render ----------------------------------------------------
void Render::init() {
	//
}

void Render::deinit() {
    //
}

void Render::resize(int width, int height) {
    setViewport(0, 0, width, height);
    Render::width = width;
    Render::height = height;
}

void Render::resetStates() {
//	glEnableVertexAttribArray(vaCoord);
//	glEnableVertexAttribArray(vaTexCoord0);

	setCulling(CULL_NONE);
	setCulling(CULL_BACK);
	setBlending(BLEND_NONE);
	setBlending(BLEND_ALPHA);
	setDepthTest(false);
	setDepthTest(true);
	setDepthWrite(false);
	setDepthWrite(true);
	for (int i = 0; i < 8; i++)
		m_active_texture[i] = NULL_OBJ;
	m_active_shader = NULL_OBJ;
	m_vbuffer = NULL;
	m_ibuffer = NULL;
	setViewport(0, 0, width, height);
}

void Render::clear(ClearMask clearMask, float r, float g, float b, float a) {
	context3D->clear(r, g, b, a, 1, 0, 0xffffffff);
}

TextureObj Render::createTexture(TexFormat texFormat, MipMap *mipMaps, int mipCount) {
	LOG("init texture: f-%d, mips-%d w:%d h:%d s:%d\n", texFormat, mipCount, mipMaps[0].width, mipMaps[0].height, mipMaps[0].size);

	AS3::ui::flash::display3D::textures::Texture tex;
	if (texFormat == TEX_ATF) {
		tex = context3D->createTexture(mipMaps[0].width, mipMaps[0].height, AS3::ui::flash::display3D::Context3DTextureFormat::COMPRESSED, false);
		tex->uploadCompressedTextureFromByteArray(AS3::ui::internal::get_ram(), (int)mipMaps[0].data, false);
	} else {
		tex = context3D->createTexture(mipMaps[0].width, mipMaps[0].height, AS3::ui::flash::display3D::Context3DTextureFormat::BGRA, false);
		for (int i = 0; i < mipCount; i++)
			tex->uploadFromByteArray(AS3::ui::internal::get_ram(), (int)mipMaps[i].data, i);
	}

	return tex;
}

ShaderObj Render::createShader(void *data) {
	LOG("init shader\n");
	AS3::ui::flash::display3D::Program3D prog = context3D->createProgram();
/*
	int vlen = ((int*)data)[0], flen = ((int*)data)[1];
LOG("v: %d, f: %d\n", vlen, flen);
LOG("init vCode\n");
	AS3::ui::flash::utils::ByteArray vCode = AS3::ui::flash::utils::ByteArray::_new();
LOG("write vCode\n");
	vCode->writeBytes(AS3::ui::internal::get_ram(), (unsigned int)&((char*)data)[8], vlen);
	
//	AS3::ui::flash::net::FileReference file = AS3::ui::flash::net::FileReference::_new();
//	file->save(vCode, "_shader.fsh");
			
LOG("init fCode\n");
	AS3::ui::flash::utils::ByteArray fCode = AS3::ui::flash::utils::ByteArray::_new();
LOG("write fCode\n");
	fCode->writeBytes(AS3::ui::internal::get_ram(), (unsigned int)&((char*)data)[8 + vlen], flen);
LOG("compile\n");
	vCode->endian = AS3::ui::flash::utils::Endian::__LITTLE_ENDIAN;
	fCode->endian = AS3::ui::flash::utils::Endian::__LITTLE_ENDIAN;
    prog->upload(vCode, fCode);
LOG("ok\n");
*/
	return prog;
}

void Render::setViewport(int left, int top, int width, int height) {
	//
}


void Render::setCulling(CullMode value) {
	if (m_culling == value) return;
	switch (value) {
		case CULL_NONE :
//			glDisable(GL_CULL_FACE);
			break;
		case CULL_BACK :
//			glCullFace(GL_BACK);
			break;
		case CULL_FRONT :
//			glCullFace(GL_FRONT);
			break;
	}
//	if (m_culling == CULL_NONE)
//		glEnable(GL_CULL_FACE);
	m_culling = value;
}

void Render::setBlending(BlendMode value) {
    if (value == m_blending) return;

	switch (value) {
		case BLEND_ALPHA :
			context3D->setBlendFactors(
				AS3::ui::flash::display3D::Context3DBlendFactor::SOURCE_ALPHA,
				AS3::ui::flash::display3D::Context3DBlendFactor::ONE_MINUS_SOURCE_ALPHA);
			break;
		case BLEND_ADD :
			context3D->setBlendFactors(
				AS3::ui::flash::display3D::Context3DBlendFactor::ONE,
				AS3::ui::flash::display3D::Context3DBlendFactor::ONE);
			break;
		case BLEND_MUL :
			context3D->setBlendFactors(
				AS3::ui::flash::display3D::Context3DBlendFactor::DESTINATION_COLOR,
				AS3::ui::flash::display3D::Context3DBlendFactor::ZERO);
			break;
		case BLEND_SCREEN :
			context3D->setBlendFactors(
				AS3::ui::flash::display3D::Context3DBlendFactor::ONE,
				AS3::ui::flash::display3D::Context3DBlendFactor::ONE_MINUS_SOURCE_COLOR);
			break;
	}
	m_blending = value;
}

void Render::setDepthWrite(bool value) {
	if (m_depthWrite != value) {
//		glDepthMask(value);
		m_depthWrite = value;
	}
}

void Render::setDepthTest(bool value) {
	if (m_depthTest == value) return;
//	value ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	m_depthTest = value;
}

void Render::setTexture(TextureObj obj, int sampler) {
	if (sampler > 0) return;
	
    if (m_active_texture[sampler] != obj) {
		m_active_texture[sampler] = obj;
		context3D->setTextureAt(sampler, obj);
    }
}

bool Render::setShader(ShaderObj obj) {
	if (m_active_shader != obj) {
		m_active_shader = obj;
		/*
		context3D->setProgram(obj);
		const vec4 cvec(0.0, 0.5, 1.0, 2.0);
		int idx = 16;
		setShaderUniform(utVec4, 1, (void*)&cvec, NULL, idx);
		*/
		return true;
	}
	return false;
}

void Render::setShaderUniform(UniformType type, int count, const void *value, const char *name, int &index) {
//	if (!activeShader)
//		return;
		
	if (index == -1) {		
		for (int i = 0; i < SP_MAX; i++)
			if (!String::cmp(name, SHADER_PARAM_INFO[i].name)) {
				index = SHADER_PARAM_INFO[i].cindex;
				break;
			}
	}
	
	if (index == -1)
		return;
		
	static const int vsize[utMAX] = { 1, 2, 3, 4, 16 };
	context3D->setProgramConstantsFromByteArray(AS3::ui::flash::display3D::Context3DProgramType::VERTEX, 0, vsize[type], AS3::ui::internal::get_ram(), (unsigned)value, (void*)value);
}

void Render::freeTexture(TextureObj obj) {
//	if (obj != NULL_OBJ) obj->dispose();
}

void Render::freeShader(ShaderObj obj) {
//	if (obj != NULL_OBJ) obj->dispose();
}

void Render::drawTriangles(IndexBuffer *iBuffer, VertexBuffer *vBuffer, int indexFirst, int triCount) {
	iBuffer->bind();
	vBuffer->bind();
	context3D->drawTriangles(iBuffer->obj, indexFirst, triCount);
}
#endif // #ifdef S3D