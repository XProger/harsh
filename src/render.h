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
#ifndef RENDER_H
#define RENDER_H

#include "utils.h"

#ifdef WIN32
	#define OGL
	#include <GL/gl.h>
	#include <GL/glext.h>   // http://www.opengl.org/registry/api/GL/glext.h (!!!)
#endif

#ifdef ANDROID
	#define OGL

	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>

	#define GL_ALPHA8 GL_ALPHA
	#define GL_RGBA8 GL_RGBA
	#define GL_LUMINANCE8 GL_LUMINANCE
	#define GL_LUMINANCE8_ALPHA8 GL_LUMINANCE_ALPHA
#endif

#ifdef __APPLE__
	#define OGL

	#include "TargetConditionals.h"

	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>

		#define GL_ALPHA8 GL_ALPHA
		#define GL_RGBA8 GL_RGBA8_OES
		#define GL_LUMINANCE8 GL_LUMINANCE
		#define GL_LUMINANCE8_ALPHA8 GL_LUMINANCE_ALPHA
	#else
		#include <OpenGL/gl.h>
		#include <OpenGL/glext.h>
		#include <AGL/agl.h>
	#endif

#endif

#ifdef FLASH
	#define S3D
	#include <AS3/AS3.h>
	#include <Flash++.h>
#endif

#ifdef OGL
	#define NULL_OBJ 0
	typedef GLuint TextureObj;
	typedef GLuint ShaderObj;
	typedef GLuint IndexBufferObj;
	typedef GLuint VertexBufferObj;
	typedef unsigned short Index;
#endif

#ifdef S3D
	#define NULL_OBJ AS3::ui::internal::_null
	typedef AS3::ui::flash::display3D::textures::Texture	TextureObj;
	typedef AS3::ui::flash::display3D::Program3D			ShaderObj;
	typedef AS3::ui::flash::display3D::IndexBuffer3D		IndexBufferObj;
	typedef AS3::ui::flash::display3D::VertexBuffer3D		VertexBufferObj;
	typedef int Index;
#endif

enum TexFormat { TEX_RGBA8 = 0, TEX_A_8, TEX_AI_88,
				 TEX_PVRTC2, TEX_PVRTC2A, TEX_PVRTC4, TEX_PVRTC4A,
				 TEX_DXT1, TEX_DXT1A, TEX_DXT3, TEX_DXT5,
				 TEX_ETC1, TEX_ATF, TEX_MAX };

struct MipMap {
	void *data;
	int width, height, size;
};

enum ClearMask {
	CLEAR_ALL,
	CLEAR_COLOR,
	CLEAR_DEPTH
};

enum BlendMode {
	BLEND_NONE,
	BLEND_ALPHA,
	BLEND_ADD,
	BLEND_MUL,
	BLEND_SCREEN
};

enum CullMode {
	CULL_NONE,
	CULL_BACK,
	CULL_FRONT
};

enum VertexAttrib {
	vaCoord,
	vaBinormal,
	vaNormal,
	vaTexCoord0,
	vaTexCoord1
};

enum UniformType {
	utVec1, utVec2, utVec3, utVec4, utMat4, utMAX
};

enum AttribType  {
	atVec1b, atVec2b, atVec3b, atVec4b,
	atVec1s, atVec2s, atVec3s, atVec4s,
	atVec1f, atVec2f, atVec3f, atVec4f
};

enum IndexFormat {
	IF_BYTE,
	IF_SHORT,
	IF_INT,
	IF_MAX
};

const int IndexStride[IF_MAX] = {
	sizeof(unsigned char),
	sizeof(unsigned short),
	sizeof(unsigned int)
};

enum VertexFormat {
	VF_P3,
	VF_PT22,
	VF_PT32,
	VF_PT34,
	VF_PT34s,
	VF_PTN_324,
	VF_PN_34,
	VF_MAX
};

typedef vec3 Vertex_P3;

struct Vertex_PT22 {
	vec2	pos;
	vec2	tc;
};

struct Vertex_PT32 {
	vec3	pos;
	vec2	tc;
};

struct Vertex_PT34 {
	vec3	pos;
	vec4	tc;
};

struct Vertex_PT34s {
	vec3 pos;
	short tc[4];
};

struct Vertex_PTN_324 {
	vec3 pos;
	vec2 tc;
	unsigned char n[4];
};

struct Vertex_PN_34 {
	vec3 pos;
	unsigned char n[4];
};

const int VertexStride[VF_MAX] = {
	sizeof(Vertex_P3),
	sizeof(Vertex_PT22),
	sizeof(Vertex_PT32),
	sizeof(Vertex_PT34),
	sizeof(Vertex_PT34s),
	sizeof(Vertex_PTN_324),
	sizeof(Vertex_PN_34)
};

struct IndexBuffer {
	IndexBufferObj obj;	
	int count;
	IndexFormat format;
	IndexBuffer(void *data, int count, IndexFormat format);
	~IndexBuffer();
	void bind();
};

struct VertexBuffer {
	VertexBufferObj obj;
	int count;
	VertexFormat format;
	VertexBuffer(void *data, int count, VertexFormat format);
	~VertexBuffer();
	void bind();
};

struct RenderParams {
	mat4 mViewProj, mModel, mViewInv;
	struct {
		vec3 pos;
	} light;
	vec4 color;
	vec4 edge;
};

struct Render {
protected:
    static BlendMode m_blending;
	static CullMode m_culling;
	static bool m_depthWrite, m_depthTest, m_alphaTest;
	static int m_active_sampler;
	static VertexBuffer *m_vbuffer;
	static IndexBuffer *m_ibuffer;
    static TextureObj m_active_texture[8];
    static ShaderObj m_active_shader;
public:
	static int statSetTex, statTriCount;
	#ifdef S3D
		static AS3::ui::flash::display::Stage stage;
		static AS3::ui::flash::display::Stage3D stage3D;
		static AS3::ui::flash::display3D::Context3D context3D;
	#endif

	static RenderParams params;
	static int width, height;

	static void init();
	static void deinit();
	static void resize(int width, int height);
	static void resetStates();
	static void clear(ClearMask clearMask, float r, float g, float b, float a);

	static TextureObj createTexture(TexFormat texFormat, MipMap *mipMaps, int mipCount);
	static ShaderObj  createShader(void *data);

	static void freeTexture(TextureObj obj);
	static void freeShader(ShaderObj obj);

	static void setViewport(int left, int top, int width, int height);
	static void setCulling(CullMode value);
	static void setBlending(BlendMode value);
	static void setDepthTest(bool value);
	static void setDepthWrite(bool value);
	static void setTexture(TextureObj obj, int sampler);
	static bool setShader(ShaderObj obj);
	static void setShaderUniform(UniformType type, int count, const void *value, const char *name, int &index);

	static void drawTriangles(IndexBuffer *iBuffer, VertexBuffer *vBuffer, int indexFirst, int triCount);
};

struct ShaderParamInfo {
	UniformType type;
	const char *name;
	int size, count, cindex;
	void *ptr;
};

enum ShaderParam {
	spMatrixViewProj,
	spMatrixModel,
	spMatrixViewInv,
	spEdge,
//	spColor,
	spLightPos,
//	spLMap,
	SP_MAX
};

const ShaderParamInfo SHADER_PARAM_INFO[SP_MAX] = {
	{utMat4, "uViewProjMatrix", sizeof(mat4), 1, 0, &Render::params.mViewProj},
	{utMat4, "uModelMatrix", 	sizeof(mat4), 1, 4, &Render::params.mModel},
	{utMat4, "uViewInvMatrix", 	sizeof(mat4), 1, 8, &Render::params.mViewInv},
	{utVec4, "uEdge", 			sizeof(vec4), 1, 12, &Render::params.edge},
//	{utVec4, "uColor", 			sizeof(vec4), 1, 0, &Render::params.color},
	{utVec3, "uLightPos", 		sizeof(vec3), 1, 0, &Render::params.light.pos},
//	{utVec4, "uLMap",			sizeof(vec4), 1, 8, NULL},
};

#endif // RENDER_H
