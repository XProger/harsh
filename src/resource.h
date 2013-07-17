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
#ifndef RESOURCE_H
#define RESOURCE_H

#include "utils.h"
#include "render.h"

//{ Resource (base)
#define RES_CACHE_TIME 3000
#ifdef WIN32
	#define RES_LIST
#endif

typedef unsigned int Hash;

struct Resource : public ListItem {
//protected:
	static List *list;

    Hash	hash;
	int		ref, time;
	bool	m_valid, deviceReset;

	virtual void load(Stream *stream) {};
public:
    Resource(Hash hash, bool deviceReset);
    virtual ~Resource();
	virtual void free();

	static Resource* getRes(Hash hash);
    static void init();
    static void deinit();
    static void invalidate();
	static void loading();
    static void check(bool forceFree = false);

	template <typename T>
	static T* loadRes(const char *name, Hash hash) {
		if (name)
            hash = Stream::getHash(name);
		T *res = (T*)Resource::getRes(hash);
		if (!res) res = ::new T(hash);
		return res;
	}

    virtual bool reset();
    virtual bool valid();
};
//}

//{ Texture
struct TextureRes : public Resource {
protected:
	virtual void load(Stream *stream);
public:
    TextureObj	obj;
    int			width, height;

	TextureRes(Hash hash) : Resource(hash, true), width(0), height(0) {}
	virtual ~TextureRes() { Render::freeTexture(obj); }

	static TextureRes* load(const char* name, Hash hash) { return loadRes<TextureRes>(name, hash); }
	static TextureRes* create(int width, int height);
};

struct Texture {
//protected:
	TextureRes *res;
public:
	Texture(const char *name, Hash hash = 0) { res = TextureRes::load(name, hash); }
//	Texture(int width, int height, bool restore = false) { res = TextureRes::create(width, height); }
	virtual ~Texture() { if (res) res->free(); }
	int getWidth()	{ return res && res->valid() ? res->width : 0; }
	int getHeight()	{ return res && res->valid() ? res->height : 0; }
	bool bind(int sampler = 0);
};
//}

//{ Shader
struct ShaderRes : public Resource {
protected:
	virtual void load(Stream *stream);
public:
    ShaderObj obj;
    int index[SP_MAX];

	ShaderRes(Hash hash) : Resource(hash, true) {}
	virtual ~ShaderRes() { Render::freeShader(obj); }

	static ShaderRes* load(const char* name, Hash hash) { return loadRes<ShaderRes>(name, hash); };
};

struct Shader {
protected:
	ShaderRes *res;
	static bool states[SP_MAX];
public:
	Shader(const char *name, Hash hash = 0) { res = ShaderRes::load(name, hash); }
	virtual ~Shader() { if (res) res->free(); }
	bool bind();
	static void setMatrixViewProj(const mat4 &value)    { setParam(spMatrixViewProj, &value); }
	static void setMatrixModel(const mat4 &value)       { setParam(spMatrixModel, &value); }
//    static void setColor(const vec4 &value)             { setParam(spColor, &value); }
//    static void setLightPos(const vec3 &value)          { setParam(spLightPos, &value); }
	static void setParam(ShaderParam param, const void *value);
	void setParam(ShaderParam param, const void *value, int count);  // immediately
};
//}

//{ Material
/*
struct MaterialRes : public Resource {
protected:
	virtual void load(Stream *stream);
public:
	Shader	*shader;
	Texture	*diffuse, *lightMap, *ambientMap;
	vec4	param[2];
	RenderMode mode;
	BlendMode blend;
	bool depthWrite;
	MaterialRes(Hash hash) : Resource(hash, false), shader(0), diffuse(0) {}
	virtual ~MaterialRes() { delete shader; delete diffuse; }
	static MaterialRes* load(const char* name, Hash hash) { return loadRes<MaterialRes>(name, hash); }
};

struct Material {
protected:
	MaterialRes *res;
public:
	Material(const char *name, Hash hash = 0) { res = MaterialRes::load(name, hash); };
	Material(const char *shaderName, const char *diffuseName);
	virtual ~Material() { if (res) res->free(); }
	void setLightMap(Texture *lightMap, const vec4 &param) {
		if (!res) return;
		res->lightMap = lightMap;
		res->param[0] = param;
	};
	void setAmbientMap(Texture *ambientMap, const vec4 &param) {
		if (!res) return;
		res->ambientMap = ambientMap;
		res->param[1] = param;
	};
	bool bind();
};
*/

struct Material {
	Shader		*shader;
	Texture		*diffuse, *mask, *lightMap;
	vec4		param;
	BlendMode	blending;
	bool		depthWrite;
	CullMode	culling;

	Material(Stream *stream);
	virtual ~Material();
	bool bind();
};
//}

//{ Mesh
struct MeshRes : public Resource {
protected:
	virtual void load(Stream *stream);
public:
	VertexBuffer	*vBuffer;
	IndexBuffer		*iBuffer;
	int				iCount, vCount;
	char			*vData, *iData;
	IndexFormat		iFormat;
	VertexFormat	vFormat;

	MeshRes(unsigned int hash) : Resource(hash, true), vBuffer(0), iBuffer(0), vData(0), iData(0) {}
	virtual ~MeshRes();

	static MeshRes* load(const char* name, unsigned int hash = 0) { return loadRes<MeshRes>(name, hash); };
	static MeshRes* create(IndexFormat iFormat, VertexFormat vFormat, void *idx, void *vert, int iCount, int vCount);
};
//}

//{ Sound
#define SND_MAX_CHANNELS 32
#define _clamp16s(x) (x < -0x8000 ? -0x8000 : (x > 0x7FFF ? 0x7FFF : x))

struct IMA_STATE {
	int amp, idx;
};

struct IMA_HEADER {
    int magic, fmt, size, flags;
};

struct SoundFrame {
    short L, R;
    SoundFrame(short L, short R) : L(L), R(R) {}
    SoundFrame(short M, float pan) {
		L = (int)(M * _min(1.0f, 1.0f - pan));
		R = (int)(M * _min(1.0f, pan + 1.0f));
    }
    inline SoundFrame& operator += (const SoundFrame &s) {
    	L = _clamp16s((int)L + s.L);
    	R = _clamp16s((int)R + s.R);
    	return *this;
	}
};

struct SoundRes : public Resource {
protected:
	virtual void load(Stream *stream);
public:
	static SoundRes* load(const char* name, unsigned int hash = 0) { return loadRes<SoundRes>(name, hash); }

    unsigned char *data;
    int size;
    bool mono, loop;

	SoundRes(Hash hash) : Resource(hash, false), data(0), size(0) {}
	virtual ~SoundRes() { delete data; };
};

struct Sound;

struct SoundChannel {
    IMA_STATE	ima[2];
    int			pos;
    float		pan;
	bool		playing;
	Sound		*snd;
	SoundChannel **ref;

    SoundChannel() : pos(0) {}
    void stop();
    void render(SoundFrame *frames, int count);
};

struct Sound {
	static SoundChannel channels[SND_MAX_CHANNELS];
	SoundRes *res;

	Sound(const char *name, Hash hash = 0) { res = SoundRes::load(name, hash); }
	virtual ~Sound();
    bool play(float pan = 0, SoundChannel **ch = NULL);

    static void init() { clear(); };
    static void deinit() { clear(); };
	static void clear();
    static void fill(SoundFrame *frames, int count);
};
//}

#endif // RESOURCE_H
