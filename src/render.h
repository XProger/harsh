#ifndef RENDER_H
#define RENDER_H

#include "utils.h"

#ifdef ANDROID
	#define OGL
#endif

#ifdef WIN32
	#define OGL
#endif

#ifdef __APPLE__
	#define OGL
#endif

#ifdef FLASH
	#define S3D
	#include <AS3/AS3.h>
	#include <Flash++.h>
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

typedef unsigned short Index;

enum VertexFormat {
	VF_P2T2,
	VF_P3NT,
	VF_P3NT_RAW,
	VF_P3BNTT,
	VF_MAX
};

struct Vertex_P2T2 {
	vec2		pos;
	vec2		tc;
};

struct Vertex_P3NT {
	vec3	pos;
	char	normal[4];
	short	tc[2];
};

struct Vertex_P3NT_RAW {
	vec3	pos;
	vec3	normal;
	vec2	tc;
	Vertex_P3NT_RAW(float x, float y, float z, float nx, float ny, float nz, float s, float t) : pos(x, y, z), normal(nx, ny, nz), tc(s, t) {};
};

struct Vertex_P3BNTT {
	vec3 pos;
//	char binormal[4];
//	char normal[4];
	short tc[4];
};

const int VertexStride[VF_MAX] = {
	sizeof(Vertex_P2T2),
	sizeof(Vertex_P3NT),
	sizeof(Vertex_P3NT_RAW),
	sizeof(Vertex_P3BNTT)
};

struct IndexBuffer {
	void *obj;
	int count;
	IndexBuffer(void *data, int count);
	~IndexBuffer();
	void bind();
};

struct VertexBuffer {
	void *obj;
	int count;
	VertexFormat format;
	VertexBuffer(void *data, int count, VertexFormat format);
	~VertexBuffer();
	void bind();
};

struct RenderParams {
	mat4 mViewProj, mModel;
	struct {
		vec3 pos;
	} light;
	vec4 color;
};

struct Render {
protected:
    static BlendMode m_blending;
	static CullMode m_culling;
	static bool m_depthWrite, m_depthTest, m_alphaTest;
	static int m_active_texture;
public:
	static int statSetTex, statTriCount;
#ifdef S3D
	static AS3::ui::flash::display::Stage stage;
	static AS3::ui::flash::display::Stage3D stage3D;
	static AS3::ui::flash::display3D::Context3D context3D;
#endif
	static void *m_vbuffer, *m_ibuffer;

	static RenderParams params;
    static void *activeTexture[8];
    static void *activeShader;
    static int width, height;

    static void init();
    static void deinit();
    static void resize(int width, int height);
    static void resetStates();
    static void clear(ClearMask clearMask, float r, float g, float b, float a);

    static void* createTexture(TexFormat texFormat, MipMap *mipMaps, int mipCount);
    static void* createShader(void *data);

    static void freeTexture(void **obj);
    static void freeShader(void **obj);

    static void setViewport(int left, int top, int width, int height);
	static void setCulling(CullMode value);
	static void setBlending(BlendMode value);
	static void setDepthTest(bool value);
	static void setDepthWrite(bool value);
    static void setTexture(void *obj, int sampler);
    static bool setShader(void *obj);
    static void setShaderUniform(UniformType type, int count, const void *value, const char *name, int &index);

    static void drawTriangles(IndexBuffer *iBuffer, VertexBuffer *vBuffer, int indexFirst, int triCount);
};

#endif // RENDER_H
