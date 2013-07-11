#include "render.h"

#ifdef OGL

#ifdef __APPLE__
	#include "TargetConditionals.h"

	#ifdef TARGET_OS_IPHONE
    	#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>

		#define GL_ALPHA8 GL_ALPHA
		#define GL_RGBA8 GL_RGBA8_OES
    	#define GL_LUMINANCE8 GL_LUMINANCE
    	#define GL_LUMINANCE8_ALPHA8 GL_LUMINANCE_ALPHA
	#endif

#endif

#ifdef ANDROID
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>

	#define GL_ALPHA8 GL_ALPHA
	#define GL_RGBA8 GL_RGBA
	#define GL_LUMINANCE8 GL_LUMINANCE
	#define GL_LUMINANCE8_ALPHA8 GL_LUMINANCE_ALPHA
#endif

#ifdef WIN32
	#include <cstdio>
	#include <windows.h>
	#include <GL/gl.h>
	#include <GL/glext.h>   // http://www.opengl.org/registry/api/GL/glext.h (!!!)

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

BlendMode Render::m_blending;
CullMode Render::m_culling;
bool Render::m_depthWrite, Render::m_depthTest, Render::m_alphaTest;
int Render::m_active_texture;
int Render::width, Render::height;
void *Render::activeTexture[8];
void *Render::activeShader;
RenderParams Render::params;
int Render::statSetTex, Render::statTriCount;
void *Render::m_vbuffer, *Render::m_ibuffer;

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
IndexBuffer::IndexBuffer(void *data, int count) : count(count) {
	glGenBuffers(1, (GLuint*)&obj);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)obj);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(short), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1, (GLuint*)&obj);
}

void IndexBuffer::bind() {
	if (Render::m_ibuffer != obj) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)obj);
		Render::m_ibuffer = obj;
	}
}

// VertexBuffer ----------------------------------------------
VertexBuffer::VertexBuffer(void *data, int count, VertexFormat format) : count(count), format(format) {
	glGenBuffers(1, (GLuint*)&obj);
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)obj);
	glBufferData(GL_ARRAY_BUFFER, count * VertexStride[format], data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, (GLuint*)&obj);
}

void VertexBuffer::bind() {
	if (Render::m_vbuffer != obj) {
		glBindBuffer(GL_ARRAY_BUFFER, (GLuint)obj);
		Render::m_vbuffer = obj;
	}
	Vertex_P2T2 *vP2T2 = NULL;
	Vertex_P3NT *vP3NT = NULL;
	Vertex_P3NT_RAW *vP3NTr = NULL;
	Vertex_P3BNTT *vP3BNTT = NULL;

	switch (format) {
		case VF_P2T2 :
			glVertexAttribPointer(vaTexCoord0, 2, GL_FLOAT, false, VertexStride[format], &vP2T2->tc);
			glVertexAttribPointer(vaCoord, 2, GL_FLOAT, false, VertexStride[format], &vP2T2->pos);
			break;
		case VF_P3NT :
			glVertexAttribPointer(vaNormal, 4, GL_UNSIGNED_BYTE, true, VertexStride[format], &vP3NT->normal);
			glVertexAttribPointer(vaTexCoord0, 2, GL_SHORT, false, VertexStride[format], &vP3NT->tc);
			glVertexAttribPointer(vaCoord, 3, GL_FLOAT, false, VertexStride[format], &vP3NT->pos);
			break;
		case VF_P3NT_RAW :
			glVertexAttribPointer(vaNormal, 3, GL_FLOAT, false, VertexStride[format], &vP3NTr->normal);
			glVertexAttribPointer(vaTexCoord0, 2, GL_FLOAT, false, VertexStride[format], &vP3NTr->tc);
			glVertexAttribPointer(vaCoord, 3, GL_FLOAT, false, VertexStride[format], &vP3NTr->pos);
			break;
		case VF_P3BNTT :
			//glVertexAttribPointer(vaNormal, 3, GL_FLOAT, false, VertexStride[format], &vP3NTr->normal);
			glVertexAttribPointer(vaTexCoord0, 4, GL_SHORT, false, VertexStride[format], &vP3BNTT->tc);
			glVertexAttribPointer(vaCoord, 3, GL_FLOAT, false, VertexStride[format], &vP3BNTT->pos);
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
		activeTexture[i] = NULL;
	activeShader = NULL;
	m_vbuffer = m_ibuffer = NULL;
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

void* Render::createTexture(TexFormat texFormat, MipMap *mipMaps, int mipCount) {
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

	GLenum oldID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&oldID);

	GLuint *ID = new GLuint(0);

	glGenTextures(1, ID);
	glBindTexture(GL_TEXTURE_2D, *ID);
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

	glBindTexture(GL_TEXTURE_2D, oldID);

	return ID;
}

void* Render::createShader(void *data) {
	GLuint *ID = new GLuint(0);
	*ID = glCreateProgram();
	shaderAttach(GL_VERTEX_SHADER, *ID, (char*)data);
	shaderAttach(GL_FRAGMENT_SHADER, *ID, (char*)data);

// bind vertex attributes
	glBindAttribLocation(*ID, vaCoord, "aCoord");
	glBindAttribLocation(*ID, vaNormal, "aNormal");
	glBindAttribLocation(*ID, vaTexCoord0, "aTexCoord0");

	glLinkProgram(*ID);
	GLint status;
	glGetProgramiv(*ID, GL_LINK_STATUS, &status);
	if (status != 1)
		shaderCheck(*ID, true);

// init samplers
	Render::setShader(ID);
	const char *samplers[] = {"sDiffuse", "sMask", "sLight"};
	for (int i = 0; i < 3; i++)
		glUniform1iv(glGetUniformLocation(*ID, samplers[i]), 1, &i);

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

void Render::setTexture(void *obj, int sampler) {
    if (activeTexture[sampler] != obj) {
		statSetTex++;
		activeTexture[sampler] = obj;
		if (m_active_texture != sampler) {
			glActiveTexture(GL_TEXTURE0 + sampler);
			m_active_texture = sampler;
		}
        glBindTexture(GL_TEXTURE_2D, obj ? *(GLuint*)obj : 0);
    }
}

bool Render::setShader(void *obj) {
	if (activeShader != obj) {
		activeShader = obj;
		glUseProgram(obj ? *(GLuint*)obj : 0);
		return true;
	}
	return false;
}

void Render::setShaderUniform(UniformType type, int count, const void *value, const char *name, int &index) {
	if (!activeShader || (index == -1 && (index = glGetUniformLocation(*(GLuint*)activeShader, name)) == -1))
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

void Render::freeTexture(void **obj) {
	if (!obj || !*obj) return;
	glDeleteTextures(1, (GLuint*)(*obj));
	delete (GLuint*)*obj;
	*obj = NULL;
}

void Render::freeShader(void **obj) {
	if (!obj || !*obj) return;
	glDeleteProgram(*(GLuint*)(*obj));
	delete (GLuint*)*obj;
	*obj = NULL;
}

void Render::drawTriangles(IndexBuffer *iBuffer, VertexBuffer *vBuffer, int indexFirst, int triCount) {
	iBuffer->bind();
	vBuffer->bind();
	glDrawElements(GL_TRIANGLES, triCount * 3, GL_UNSIGNED_SHORT, (GLvoid*)(indexFirst * sizeof(short)));
	statTriCount += triCount;
}
#endif // #ifdef OGL
