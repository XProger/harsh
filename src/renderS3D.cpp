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
IndexBuffer::IndexBuffer(void *data, int count, IndexFormat format) : obj(NULL_OBJ), count(count), format(format) {
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
VertexBuffer::VertexBuffer(void *data, int count, VertexFormat format) : obj(NULL_OBJ), count(count), format(format) {
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
	resetStates();
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
		setTexture(NULL_OBJ, i);
	m_active_shader = NULL_OBJ;
	m_vbuffer = NULL;
	m_ibuffer = NULL;
	setViewport(0, 0, width, height);
}

void Render::clear(ClearMask clearMask, float r, float g, float b, float a) {
	context3D->clear(r, g, b, a, 1, 0, 0xffffffff);
}

TextureObj Render::createTexture(TexFormat texFormat, MipMap *mipMaps, int mipCount) {
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
	AS3::ui::flash::display3D::Program3D prog = context3D->createProgram();

	AS3::ui::flash::utils::ByteArray vCode = AS3::ui::flash::utils::ByteArray::_new(),
									 fCode = AS3::ui::flash::utils::ByteArray::_new();

	int vlen = ((int*)data)[0], flen = ((int*)data)[1];
	vCode->writeBytes(AS3::ui::internal::get_ram(), (unsigned int)&((char*)data)[8], vlen);	
	fCode->writeBytes(AS3::ui::internal::get_ram(), (unsigned int)&((char*)data)[8 + vlen], flen);
	vCode->endian = AS3::ui::flash::utils::Endian::__LITTLE_ENDIAN;
	fCode->endian = AS3::ui::flash::utils::Endian::__LITTLE_ENDIAN;
    prog->upload(vCode, fCode);
	return prog;
}

void Render::setViewport(int left, int top, int width, int height) {
	//
}

void Render::setCulling(CullMode value) {
	if (m_culling == value) return;
	switch (value) {
		case CULL_NONE :
			context3D->setCulling(AS3::ui::flash::display3D::Context3DTriangleFace::NONE);
			break;
		case CULL_BACK :
			context3D->setCulling(AS3::ui::flash::display3D::Context3DTriangleFace::FRONT);
			break;
		case CULL_FRONT :
			context3D->setCulling(AS3::ui::flash::display3D::Context3DTriangleFace::BACK);
			break;
	}
	m_culling = value;
}

void Render::setBlending(BlendMode value) {
    if (value == m_blending) return;
	switch (value) {
		case BLEND_NONE :
			context3D->setBlendFactors(
				AS3::ui::flash::display3D::Context3DBlendFactor::ONE,
				AS3::ui::flash::display3D::Context3DBlendFactor::ZERO);
			break;
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
	if (m_depthWrite == value) return;
	m_depthWrite = value;
	if (m_depthTest)
		context3D->setDepthTest(m_depthWrite, AS3::ui::flash::display3D::Context3DCompareMode::LESS_EQUAL);
	else
		context3D->setDepthTest(m_depthWrite, AS3::ui::flash::display3D::Context3DCompareMode::ALWAYS);
}

void Render::setDepthTest(bool value) {
	if (m_depthTest == value) return;
	m_depthTest = value;
	if (m_depthTest)
		context3D->setDepthTest(m_depthWrite, AS3::ui::flash::display3D::Context3DCompareMode::LESS_EQUAL);
	else
		context3D->setDepthTest(m_depthWrite, AS3::ui::flash::display3D::Context3DCompareMode::ALWAYS);
}

void Render::setTexture(TextureObj obj, int sampler) {
    if (m_active_texture[sampler] != obj) {
		m_active_texture[sampler] = obj;
		context3D->setTextureAt(sampler, obj);
    }
}

bool Render::setShader(ShaderObj obj) {
	if (m_active_shader != obj) {
		m_active_shader = obj;
		context3D->setProgram(obj);
		setTexture(NULL_OBJ, 0);
		setTexture(NULL_OBJ, 1);
		setTexture(NULL_OBJ, 2);

		vec4 v = vec4(0.0, 0.5, 1.0, 0.9);
		context3D->setProgramConstantsFromByteArray(AS3::ui::flash::display3D::Context3DProgramType::FRAGMENT, 0, 1, AS3::ui::internal::get_ram(), (unsigned)&v, (void*)&v);

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

	const void *data = value;
	mat4 m;
	if (type == utMat4) {
		m = ((mat4*)value)->transpose();
		data = &m;
	}
/*
	vec4 v;
	if (count % sizeof(vec4)) {
		value = &v;
	}
*/		
	static const int vsize[utMAX] = { 1, 1, 1, 1, 4 };
	context3D->setProgramConstantsFromByteArray(AS3::ui::flash::display3D::Context3DProgramType::VERTEX, index, vsize[type], AS3::ui::internal::get_ram(), (unsigned)data, (void*)data);
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