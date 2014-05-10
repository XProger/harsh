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
#include "render.h"

#ifdef OGL

#ifdef WIN32
	#include <windows.h>

	void* _GetProcAddress(const char *name) {
		void* ptr = (void*)wglGetProcAddress(name);
		if (!ptr) printf("glError: invalid ptr %s\n", name);
		return ptr;
	}

#if defined(_MSC_VER)
	#define GetProcOGL(x) *(void**)&x=(void*)_GetProcAddress(#x);
#else
	#define GetProcOGL(x) x=(typeof(x))_GetProcAddress(#x);
#endif

// Texture
	PFNGLACTIVETEXTUREPROC glActiveTexture;
	PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
// VBO
    PFNGLGENBUFFERSARBPROC glGenBuffers;
    PFNGLDELETEBUFFERSARBPROC glDeleteBuffers;
    PFNGLBINDBUFFERARBPROC glBindBuffer;
    PFNGLBUFFERDATAARBPROC glBufferData;
// GLSL
    PFNGLGETPROGRAMIVPROC glGetProgramiv;
    PFNGLCREATEPROGRAMPROC glCreateProgram;
    PFNGLDELETEPROGRAMPROC glDeleteProgram;
    PFNGLLINKPROGRAMPROC glLinkProgram;
    PFNGLUSEPROGRAMPROC glUseProgram;
    PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
    PFNGLGETSHADERIVPROC glGetShaderiv;
    PFNGLCREATESHADERPROC glCreateShader;
    PFNGLDELETESHADERPROC glDeleteShader;
    PFNGLSHADERSOURCEPROC glShaderSource;
    PFNGLATTACHSHADERPROC glAttachShader;
    PFNGLCOMPILESHADERPROC glCompileShader;
    PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
    PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
    PFNGLUNIFORM1IVPROC glUniform1iv;
    PFNGLUNIFORM1FVPROC glUniform1fv;
    PFNGLUNIFORM2FVPROC glUniform2fv;
    PFNGLUNIFORM3FVPROC glUniform3fv;
    PFNGLUNIFORM4FVPROC glUniform4fv;
    PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
    PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
// RTT
	/*
	PFNGLGENFRAMEBUFFERSPROC glGenFramebuffersEXT;
	PFNGLBINDFRAMEBUFFERPROC glBindFramebufferEXT;
    PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2DEXT;

    #define glGenFramebuffers			glGenFramebuffersEXT
	#define glBindFramebuffer			glBindFramebufferEXT
    #define glFramebufferTexture2D		glFramebufferTexture2DEXT
	*/
#endif

// DXT
#define GL_COMPRESSED_RGB_S3TC_DXT1			0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1		0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3		0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5		0x83F3
// PVRTC
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1		0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1		0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1		0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1		0x8C03
// ETC
#define GL_ETC1_RGB8_OES					0x8D64

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

GLuint renderTarget = 0;
GLint renderTargetOld = 0;

void shaderCheck(GLuint obj, bool isProgram) {
	GLint len;
	if (isProgram)
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &len);
	else
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &len);
	if (len == 0) return;
	const char *buf = new char[len];
	if (isProgram)
		glGetProgramInfoLog(obj, len, NULL, (GLchar*)buf);
	else
		glGetShaderInfoLog(obj, len, NULL, (GLchar*)buf);
	LOG("shader: %s\n", buf);
	delete[] buf;
}

void shaderAttach(GLenum type, GLuint &ID, const char *code) {
	GLuint obj = glCreateShader(type);
	const char *lCode[2] = {type == GL_VERTEX_SHADER ? "#define VERTEX\n" : "#define FRAGMENT\n", code};
	glShaderSource(obj, 2, lCode, NULL);
	glCompileShader(obj);
	GLint status;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	if (status != 1)
		shaderCheck(obj, false);
	glAttachShader(ID, obj);
	glDeleteShader(obj);
}

// IndexBuffer -----------------------------------------------
IndexBuffer::IndexBuffer(void *data, int count, IndexFormat format) : obj(NULL_OBJ), count(count), format(format) {
	glGenBuffers(1, &obj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)obj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * IndexStride[format], data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, (GLuint*)&obj);
}

void IndexBuffer::bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)obj);
}

// VertexBuffer ----------------------------------------------
VertexBuffer::VertexBuffer(void *data, int count, VertexFormat format) : obj(NULL_OBJ), count(count), format(format) {
	glGenBuffers(1, &obj);
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)obj);
	glBufferData(GL_ARRAY_BUFFER, count * VertexStride[format], data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &obj);
}

void VertexBuffer::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, obj);

	Vertex_PT22		*vPT22 = NULL;
	Vertex_PT32		*vPT32 = NULL;
	Vertex_PT34		*vPT34 = NULL;
	Vertex_PT34s	*vPT34s = NULL;
	Vertex_PTN_324	*vPTN_324 = NULL;
	Vertex_PN_34	*vPN_34 = NULL;


	switch (format) {
		case VF_PT22 :
			glVertexAttribPointer(vaTexCoord0, 2, GL_FLOAT, false, VertexStride[format], &vPT22->tc);
			glVertexAttribPointer(vaCoord, 2, GL_FLOAT, false, VertexStride[format], &vPT22->pos);
			break;
		case VF_PT32 :
			glVertexAttribPointer(vaTexCoord0, 2, GL_FLOAT, false, VertexStride[format], &vPT32->tc);
			glVertexAttribPointer(vaCoord, 3, GL_FLOAT, false, VertexStride[format], &vPT32->pos);
			break;
		case VF_PT34 :
			glVertexAttribPointer(vaTexCoord0, 4, GL_FLOAT, false, VertexStride[format], &vPT34->tc);
			glVertexAttribPointer(vaCoord, 3, GL_FLOAT, false, VertexStride[format], &vPT34->pos);
			break;
		case VF_PT34s :
			glVertexAttribPointer(vaTexCoord0, 4, GL_SHORT, false, VertexStride[format], &vPT34s->tc);
			glVertexAttribPointer(vaCoord, 3, GL_FLOAT, false, VertexStride[format], &vPT34s->pos);
			break;
		case VF_PTN_324 :
			glVertexAttribPointer(vaNormal, 4, GL_UNSIGNED_BYTE, true, VertexStride[format], &vPTN_324->n);
			glVertexAttribPointer(vaTexCoord0, 2, GL_SHORT, false, VertexStride[format], &vPTN_324->tc);
			glVertexAttribPointer(vaCoord, 3, GL_FLOAT, false, VertexStride[format], &vPTN_324->pos);
			break;
		case VF_PN_34 :
			glVertexAttribPointer(vaNormal, 4, GL_UNSIGNED_BYTE, true, VertexStride[format], &vPN_34->n);
			glVertexAttribPointer(vaCoord, 3, GL_FLOAT, false, VertexStride[format], &vPN_34->pos);
			break;
		default :
			return;
	}
}

// Render ----------------------------------------------------
void Render::init() {
	#ifdef WIN32
	// Texture
		GetProcOGL(glActiveTexture);
		GetProcOGL(glCompressedTexImage2D);
	// VBO
		GetProcOGL(glGenBuffers);
		GetProcOGL(glDeleteBuffers);
		GetProcOGL(glBindBuffer);
		GetProcOGL(glBufferData);
	// GLSL
		GetProcOGL(glGetProgramiv);
		GetProcOGL(glCreateProgram);
		GetProcOGL(glDeleteProgram);
		GetProcOGL(glLinkProgram);
		GetProcOGL(glUseProgram);
		GetProcOGL(glGetProgramInfoLog);
		GetProcOGL(glGetShaderiv);
		GetProcOGL(glCreateShader);
		GetProcOGL(glDeleteShader);
		GetProcOGL(glShaderSource);
		GetProcOGL(glAttachShader);
		GetProcOGL(glCompileShader);
		GetProcOGL(glGetShaderInfoLog);
		GetProcOGL(glGetUniformLocation);
		GetProcOGL(glUniform1iv);
		GetProcOGL(glUniform1fv);
		GetProcOGL(glUniform2fv);
		GetProcOGL(glUniform3fv);
		GetProcOGL(glUniform4fv);
		GetProcOGL(glUniformMatrix4fv);
		GetProcOGL(glBindAttribLocation);
		GetProcOGL(glEnableVertexAttribArray);
		GetProcOGL(glVertexAttribPointer);
	/*
		GetProcOGL(glGenFramebuffersEXT);
		GetProcOGL(glBindFramebufferEXT);
		GetProcOGL(glFramebufferTexture2DEXT);
	*/
	#endif
	resetStates();

	m_vbuffer = NULL;
	m_ibuffer = NULL;

	LOG("GL_VERSION : %s\n", glGetString(GL_VERSION));
	LOG("GL_VENDOR  : %s\n", glGetString(GL_VENDOR));
//	LOG("GL_EXTENSIONS : %s\n", glGetString(GL_EXTENSIONS));
//	glGenFramebuffers(1, &renderTarget);
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
	glEnableVertexAttribArray(vaCoord);
	glEnableVertexAttribArray(vaNormal);
	glEnableVertexAttribArray(vaTexCoord0);

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
	glClearColor(r, g, b, a);
	switch (clearMask) {
		case CLEAR_ALL :
			setDepthWrite(true);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			break;
		case CLEAR_COLOR :
			glClear(GL_COLOR_BUFFER_BIT);
			break;
		case CLEAR_DEPTH :
			setDepthWrite(true);
			glClear(GL_DEPTH_BUFFER_BIT);
			break;
	}
}

TextureObj Render::createTexture(TexFormat texFormat, MipMap *mipMaps, int mipCount) {
	struct FormatInfo {
        int iformat, eformat, type;
	} info[] = {
		{GL_RGBA,							GL_RGBA,			GL_UNSIGNED_BYTE},
		{GL_ALPHA,							GL_ALPHA,			GL_UNSIGNED_BYTE},
		{GL_LUMINANCE8_ALPHA8,				GL_LUMINANCE_ALPHA,	GL_UNSIGNED_BYTE},
		{GL_COMPRESSED_RGB_PVRTC_2BPPV1,	GL_FALSE,			GL_FALSE},
		{GL_COMPRESSED_RGBA_PVRTC_2BPPV1,	GL_FALSE,			GL_FALSE},
		{GL_COMPRESSED_RGB_PVRTC_4BPPV1,	GL_FALSE,			GL_FALSE},
		{GL_COMPRESSED_RGBA_PVRTC_4BPPV1,	GL_FALSE,			GL_FALSE},
		{GL_COMPRESSED_RGB_S3TC_DXT1,		GL_FALSE,			GL_FALSE},
		{GL_COMPRESSED_RGBA_S3TC_DXT1,		GL_FALSE,			GL_FALSE},
		{GL_COMPRESSED_RGBA_S3TC_DXT3,		GL_FALSE,			GL_FALSE},
		{GL_COMPRESSED_RGBA_S3TC_DXT5,		GL_FALSE,			GL_FALSE},
		{GL_ETC1_RGB8_OES,					GL_FALSE,			GL_FALSE},
	};

	GLuint ID;

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipCount > 1 ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	int aniso;
	glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	
    FormatInfo &fmt = info[texFormat];

	for (int i = 0; i < mipCount; i++) {
	    MipMap &m = mipMaps[i];
		if (texFormat >= TEX_PVRTC2)
			glCompressedTexImage2D(GL_TEXTURE_2D, i, fmt.iformat, m.width, m.height, 0, m.size, m.data);
		else
			glTexImage2D(GL_TEXTURE_2D, i, fmt.iformat, m.width, m.height, 0, fmt.eformat, fmt.type, m.data);
	}

	return ID;
}

ShaderObj Render::createShader(void *data) {
	GLuint ID = glCreateProgram();
	shaderAttach(GL_VERTEX_SHADER, ID, (char*)data);
	shaderAttach(GL_FRAGMENT_SHADER, ID, (char*)data);

// bind vertex attributes
	glBindAttribLocation(ID, vaCoord, "aCoord");
	glBindAttribLocation(ID, vaNormal, "aNormal");
	glBindAttribLocation(ID, vaTexCoord0, "aTexCoord0");
	glBindAttribLocation(ID, vaTexCoord1, "aTexCoord1");

	glLinkProgram(ID);
	GLint status;
	glGetProgramiv(ID, GL_LINK_STATUS, &status);
	if (status != 1)
		shaderCheck(ID, true);

// init samplers
	Render::setShader(ID);
	const char *samplers[] = {"sDiffuse", "sMask", "sLight"};
	for (int i = 0; i < 3; i++)
		glUniform1iv(glGetUniformLocation(ID, samplers[i]), 1, &i);

	return ID;
}

void Render::setViewport(int left, int top, int width, int height) {
	glViewport(left, top, width, height);
}

void Render::setCulling(CullMode value) {
	if (m_culling == value) return;
	switch (value) {
		case CULL_NONE :
			glDisable(GL_CULL_FACE);
			break;
		case CULL_BACK :
			glCullFace(GL_BACK);
			break;
		case CULL_FRONT :
			glCullFace(GL_FRONT);
			break;
	}
	if (m_culling == CULL_NONE)
		glEnable(GL_CULL_FACE);
	m_culling = value;
}

void Render::setBlending(BlendMode value) {
    if (value == m_blending) return;
	switch (value) {
		case BLEND_NONE :
			glDisable(GL_BLEND);
			break;
		case BLEND_ALPHA :
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case BLEND_ADD :
			glBlendFunc(GL_ONE, GL_ONE);
			break;
		case BLEND_MUL :
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			break;
		case BLEND_SCREEN :
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
			break;
	}

	if (m_blending == BLEND_NONE)
		glEnable(GL_BLEND);
	m_blending = value;
}

void Render::setDepthWrite(bool value) {
	if (m_depthWrite != value) {
		glDepthMask(value);
		m_depthWrite = value;
	}
}

void Render::setDepthTest(bool value) {
	if (m_depthTest == value) return;
	value ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	m_depthTest = value;
}

void Render::setTexture(TextureObj obj, int sampler) {
    if (m_active_texture[sampler] != obj) {
		statSetTex++;
		m_active_texture[sampler] = obj;
		if (m_active_sampler != sampler) {
			glActiveTexture(GL_TEXTURE0 + sampler);
			m_active_sampler = sampler;
		}
        glBindTexture(GL_TEXTURE_2D, obj ? obj : 0);
    }
}

bool Render::setShader(ShaderObj obj) {
	if (m_active_shader != obj) {
		m_active_shader = obj;
		glUseProgram(obj ? obj : 0);
		return true;
	}
	return false;
}

void Render::setShaderUniform(UniformType type, int count, const void *value, const char *name, int &index) {
	if (index == -1 && (index = glGetUniformLocation(m_active_shader, name)) == -1)
		return;

	switch (type) {
		case utVec1	: glUniform1fv(index, count, (GLfloat*)value); break;
		case utVec2	: glUniform2fv(index, count, (GLfloat*)value); break;
		case utVec3	: glUniform3fv(index, count, (GLfloat*)value); break;
		case utVec4	: glUniform4fv(index, count, (GLfloat*)value); break;
		case utMat4	: glUniformMatrix4fv(index, count, false, (GLfloat*)value); break;
	default :
		return;
	}
}

void Render::freeTexture(TextureObj obj) {
	if (obj) glDeleteTextures(1, &obj);
}

void Render::freeShader(ShaderObj obj) {
	if (obj) glDeleteProgram(obj);
}

void Render::drawTriangles(IndexBuffer *iBuffer, VertexBuffer *vBuffer, int indexFirst, int triCount) {
	if (m_ibuffer != iBuffer) { iBuffer->bind(); m_ibuffer = iBuffer; };
	if (m_vbuffer != vBuffer) { vBuffer->bind(); m_vbuffer = vBuffer; };
	
	const int iFormatGL[3] = {GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT};

	glDrawElements(GL_TRIANGLES, triCount * 3, iFormatGL[iBuffer->format], (GLvoid*)(indexFirst * sizeof(short)));
	statTriCount += triCount;
}
#endif // #ifdef OGL
