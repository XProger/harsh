#include "resource.h"
#include "core.h"

//{ Resource
List *Resource::list;

Resource::Resource(unsigned int hash, bool deviceReset) : hash(hash), ref(1), m_valid(false), deviceReset(deviceReset) {
	list->add(this);
}

Resource::~Resource() {
	list->remove(this);
}

void Resource::free() {
	if (!--ref)
        time = Core::getTime() + RES_CACHE_TIME;
}

void Resource::init() {
    list = new List();
}

void Resource::deinit() {
	delete list;
}

Resource* Resource::getRes(unsigned int hash) {
    if (!hash)
        return NULL;

	Resource* r = (Resource*)list->first;
	while (r)
		if (r->hash == hash) {
			r->ref++;
			return r;
		} else
            r = (Resource*)r->next;
	return NULL;
}

void Resource::invalidate() {
	Resource* r = (Resource*)list->first;
	while (r) {
	    if (r->deviceReset)
            r->m_valid = false;
		r = (Resource*)r->next;
	}
}

void Resource::loading() {
	Resource* r = (Resource*)list->first;
	while (r) {
		r->valid();
		r = (Resource*)r->next;
	}	
}

void Resource::check(bool forceFree) {
	Resource *p, *r = (Resource(*))list->first;
	int time = Core::getTime();
	while (r) {
	    p = (Resource*)r->next;
        if (!r->ref && (forceFree || (r->time < time) ))
            delete r;
		r = p;
	}
}

bool Resource::reset() {
    if (hash) {
        Stream *stream = new Stream(hash);
        if (stream->size > 0)
            load(stream);
        delete stream;
    } else 
        load(NULL);	
	return m_valid;
}

bool Resource::valid() {
	return m_valid || reset();
}
//}

//{ Texture
void TextureRes::load(Stream *stream) {
	Render::freeTexture(&obj);

    if (!stream) {
        MipMap m = { NULL, width, height, width * height * 4 };
        obj = Render::createTexture(TEX_RGBA8, &m, 1);
        m_valid = obj != NULL;
        return;
    }

    struct PVR_TEXTURE_HEADER {
        unsigned int dwHeaderSize;
        unsigned int dwHeight;
        unsigned int dwWidth;
        unsigned int dwMipMapCount;
        unsigned int dwpfFlags;
        unsigned int dwDataSize;
        unsigned int dwBitCount;
        unsigned int dwMask[4];
        unsigned int dwPVR;
        unsigned int dwNumSurfs;
    } *header = (PVR_TEXTURE_HEADER*)stream->getData(sizeof(PVR_TEXTURE_HEADER));

	int mipCount = header->dwMipMapCount + 1;

	struct FormatInfo {
		int id, minX, minY;
		bool alpha;
		TexFormat texFormat;
	} info[] = {
		{0x12,	1,	1,	true,	TEX_RGBA8},
		{0x07,	1,	1,	false,	TEX_A_8},
		{0x17,	1,	1,	true,	TEX_AI_88},
		{0x18,	16,	8,	false,	TEX_PVRTC2},
		{0x18,	16,	8,	true,	TEX_PVRTC2A},
		{0x19,	8,	8,	false,	TEX_PVRTC4},
		{0x19,	8,	8,	true,	TEX_PVRTC4A},
		{0x20,	4,	4,	false,	TEX_DXT1},
		{0x20,	4,	4,	true,	TEX_DXT1A},
		{0x22,	4,	4,	true,	TEX_DXT3},
		{0x24,	4,	4,	true,	TEX_DXT5},
		{0x36,	4,	4,	false,	TEX_ETC1},
	};

	int  id = header->dwpfFlags & 0xff;
	bool alpha = header->dwMask[3] > 0;
	FormatInfo *fmt = NULL;
	for (int i = 0; i < (int)(sizeof(info) / sizeof(FormatInfo)); i++)
		if (info[i].id == id && info[i].alpha == alpha) {
			fmt = &info[i];
			break;
		}

	if (!fmt) {
		LOG("invalid texture format: %d\n", id);
		return;
	}

	int sizeX = header->dwWidth, sizeY = header->dwHeight;

    MipMap *mipMaps = new MipMap[mipCount];

	for (int i = 0; i < mipCount; i++) {
	    MipMap &m = mipMaps[i];
		m.size   = (_max(sizeX, fmt->minX) * _max(sizeY, fmt->minY) * header->dwBitCount + 7) / 8;
		m.data   = stream->getData(m.size);
        m.width  = sizeX;
        m.height = sizeY;
		sizeX >>= 1;
		sizeY >>= 1;
	}

    width  = header->dwWidth;
    height = header->dwHeight;
	obj = Render::createTexture(fmt->texFormat, mipMaps, mipCount);

	delete[] mipMaps;
    m_valid = obj != NULL;
}

TextureRes* TextureRes::create(int width, int height) {
    TextureRes *tex = new TextureRes(0);
    tex->width = width;
    tex->height = height;
    return tex;
}

bool Texture::bind(int sampler) {
    if (res && res->valid()) {
        Render::setTexture(res->obj, sampler);
        return true;
    } else {
		LOG("fuck texture\n");
        return false;
	}
}
//}

//{ Shader
bool Shader::states[SP_MAX];

void ShaderRes::load(Stream *stream) {
	Render::freeShader(&obj);
	obj = Render::createShader(stream->getData(stream->size));
	m_valid = obj != NULL;
}

bool Shader::bind() {
    if (res && res->valid()) {

    	if (Render::setShader(res->obj))
			memset(&states, true, sizeof(states));

		for (int i = 0; i < SP_MAX; i++)
			if (states[i]) {
                const ShaderParamInfo &p = SHADER_PARAM_INFO[i];
				if (p.ptr)
					Render::setShaderUniform(p.type, p.count, p.ptr, p.name, index[i]);
                states[i] = false;
			}

        return true;
    } else {
		LOG("fuck shader\n");
        return false;
	}
}

void Shader::setParam(ShaderParam param, const void *value) {
	const ShaderParamInfo &p = SHADER_PARAM_INFO[param];
    states[param] = true;
    memcpy(p.ptr, value, p.size);
}

void Shader::setParam(ShaderParam param, const void *value, int count) {
	const ShaderParamInfo &p = SHADER_PARAM_INFO[param];
    Render::setShaderUniform(p.type, count, value, p.name, index[param]);
}
//}

//{ Material
Material::Material(Stream *stream) {
	blending	= (BlendMode)stream->getInt();
	depthWrite	= stream->getInt() != 0;
	culling		= (CullMode)stream->getInt();
	alphaTest	= stream->getInt() != 0;
	shader		= new Shader(0, stream->getInt());
	Hash h;
	diffuse		= (h = stream->getInt()) ? new Texture(NULL, h) : NULL;
	mask		= (h = stream->getInt()) ? new Texture(NULL, h) : NULL;
	lightMap	= (h = stream->getInt()) ? new Texture(NULL, h) : NULL;
	ambientMap	= (h = stream->getInt()) ? new Texture(NULL, h) : NULL;
	stream->getCopy(&param, sizeof(param));
}

Material::~Material() {
	delete shader;
	delete diffuse;
	delete mask;
	delete lightMap;
	delete ambientMap;
}

bool Material::bind() {
	if ( /* mode != Render::mode || */ !shader->bind())
		return false;
	if (diffuse && !diffuse->bind(0))
		return false;
	if (mask && !mask->bind(1))
		return false;
	if (lightMap && !lightMap->bind(2))
		return false;
	if (ambientMap && !ambientMap->bind(3))
		return false;
	shader->setParam(spLMap, &param, 2);	
	Render::setBlending(blending);
	Render::setCulling(culling);
	Render::setDepthWrite(depthWrite);
	return true;
}

/*
void MaterialRes::load(Stream *stream) {
	if (!stream) {
		m_valid = false;
		LOG("fuck!\n");
		return;
	}

	mode		= (RenderMode)stream->getInt();
	blend		= (BlendMode)stream->getInt();
	depthWrite	= stream->getInt() != 0;
	shader		= new Shader(0, stream->getInt());
	diffuse		= new Texture(NULL, stream->getInt());	
	lightMap	= new Texture(NULL, stream->getInt());
	ambientMap	= new Texture(NULL, stream->getInt());
	stream->getCopy(&param, sizeof(param));

	m_valid = true;
}

Material::Material(const char *shaderName, const char *diffuseName) {
	res = new MaterialRes(0);
	res->shader = new Shader(shaderName);
	res->diffuse = new Texture(diffuseName);
}

bool Material::bind() {
	if (!res || !res->valid() || res->mode != Render::mode || !res->shader->bind())
		return false;
	if (res->diffuse && !res->diffuse->bind(0))
		return false;
	if (res->lightMap && !res->lightMap->bind(2))
		return false;
	if (res->ambientMap && !res->ambientMap->bind(3))
		return false;
	res->shader->setParam(spLMap, &res->param, 2);	
	Render::setBlend(res->blend);
	Render::setDepthWrite(res->depthWrite);
	return true;
}
*/
//}

//{ Mesh Resource
void MeshRes::load(Stream *stream) {	
	if (vData && iData) {
		delete vBuffer;
		delete iBuffer;
		vBuffer = new VertexBuffer(vData, vCount, VF_P3BNTT);
		iBuffer = new IndexBuffer(iData, iCount);
	} else {
		/*
		stream->pos += 28; // BBox (24), Mode, Attrib, align, align

	// joints
		jCount  = stream->getInt();
		if (jCount) {
	//		jointHash = new JointHash[jCount];
	//		stream->getCopy(jointHash, jCount * sizeof(JointHash));
			stream->pos += jCount * 4;
		}
		*/
	// vertex buffer
		vCount  = stream->getInt();
		iCount  = stream->getInt();

		int vStride = sizeof(Vertex_P3BNTT); //stream->getInt();
		vBuffer = new VertexBuffer(stream->getData(vCount * vStride), vCount, VF_P3BNTT);

//		LOG("%d \n", stream->pos);

	// index buffer
//		iCount  = stream->getInt();
//		stream->getInt();
		iBuffer = new IndexBuffer(stream->getData(iCount * 2), iCount);

//		LOG("jCount:  %d\n", jCount);
//		LOG("vCount:  %d\n", vCount);
//		LOG("vStride: %d\n", vStride);
//		LOG("iCount:  %d\n", iCount);
	}
//	LOG("mesh i:%d\tv:%d\n", iCount, vCount);
	m_valid = true;
}

MeshRes* MeshRes::create(VertexFormat format, void *vert, int vCount, void *idx, int iCount) {
	MeshRes *res = new MeshRes(0);
	res->jCount = 0;
	res->vCount = vCount;
	res->iCount = iCount;
	res->vData = new char[vCount * VertexStride[format]];
	memcpy(res->vData, vert, vCount * VertexStride[format]);
	res->iData = new char(iCount * 2);
	memcpy(res->iData, idx, iCount * 2);
	return res;
}

MeshRes::~MeshRes() {
//	delete[] jointHash;
	delete iData;
	delete vData;
	delete iBuffer;
	delete vBuffer;
}
//}

//{ Sound
SoundChannel Sound::channels[SND_MAX_CHANNELS];

// Use IMA-ADPCM decoder: http://wiki.multimedia.cx/index.php?title=IMA_ADPCM
int ima_index_table[] = {
	  -1, -1, -1, -1, 2, 4, 6, 8
};

int ima_step_table[] = {
		7,     8,     9,    10,    11,    12,    13,    14,
	   16,    17,    19,    21,    23,    25,    28,    31,
	   34,    37,    41,    45,    50,    55,    60,    66,
	   73,    80,    88,    97,   107,   118,   130,   143,
	  157,   173,   190,   209,   230,   253,   279,   307,
	  337,   371,   408,   449,   494,   544,   598,   658,
	  724,   796,   876,   963,  1060,  1166,  1282,  1411,
	 1552,  1707,  1878,  2066,  2272,  2499,  2749,  3024,
	 3327,  3660,  4026,  4428,  4871,  5358,  5894,  6484,
	 7132,  7845,  8630,  9493, 10442, 11487, 12635, 13899,
	15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
	32767
};

__inline short IMA_DecodeSample(IMA_STATE *ima, int nibble) {
	int &amp = ima->amp;
	int &idx = ima->idx;
	int step = ima_step_table[idx];

	idx += ima_index_table[nibble & 7];
	if (idx <  0) idx = 0;
	if (idx > 88) idx = 88;

	int diff = step >> 3;
	if (nibble & 1) diff += step >> 2;
	if (nibble & 2) diff += step >> 1;
	if (nibble & 4) diff += step;

	if (nibble & 8) {
		amp -= diff;
		if (amp < -0x8000)
			amp = -0x8000;
	} else {
		amp += diff;
		if (amp > 0x7FFF)
			amp = 0x7FFF;
	}

	return amp;
}


// -------- Sound Channel ----------------------------------------------------------------
void SoundChannel::stop() {
	if (ref) *ref = NULL;
	memset(this, 0, sizeof(SoundChannel));
}

void SoundChannel::render(SoundFrame *frames, int count) {
	SoundFrame *end = frames + count;

	SoundRes *res = snd->res;

	while (frames < end) {

		if (pos >= res->size) { // end playing
			if (res->loop) {	  // repeated?
				pos = 0;
				memset(&ima, 0, sizeof(ima));
			} else {
				stop();
				return;
			}
		}

		if (res->mono) {
			*(frames++) += SoundFrame(IMA_DecodeSample(&ima[0], res->data[pos] & 0x0F), pan);
			*(frames++) += SoundFrame(IMA_DecodeSample(&ima[0], res->data[pos++] >> 4), pan);
		} else
			*(frames++) += SoundFrame(IMA_DecodeSample(&ima[0], res->data[pos] & 0x0F),
									  IMA_DecodeSample(&ima[1], res->data[pos++] >> 4));
	}
}

// -------- Sound (Sample / Mixer) ----------------------------------------------------------------
void SoundRes::load(Stream *stream) {
	IMA_HEADER *header = (IMA_HEADER*)stream->getData(sizeof(IMA_HEADER));
	size = header->size;
	mono = header->fmt == 0;
	loop = header->flags & 1;
	data = new unsigned char[size];
	stream->getCopy(data, size);
	m_valid = true;
}

void Sound::clear() {
	memset(&channels, 0, sizeof(channels));
}

void Sound::fill(SoundFrame *frames, int count) {
    memset(frames, 0, count * sizeof(SoundFrame));
	for (int i = 0; i < SND_MAX_CHANNELS; i++)
		if (channels[i].playing)
			channels[i].render(frames, count);
}

Sound::~Sound() {
    for (int i = 0; i < SND_MAX_CHANNELS; i++)
		if (channels[i].playing && channels[i].snd == this)
			channels[i].stop();
    if (res) res->free();
}

bool Sound::play(float pan, SoundChannel **ref) {
	if (!res || !res->valid()) return false;

	for (int i = 0; i < SND_MAX_CHANNELS; i++)
		if (!channels[i].playing) {
			if (ref) *ref = &channels[i];
			channels[i].snd = this;
			channels[i].pan = _min(1.0f, _max(-1.0f, pan));
			channels[i].playing = true;
			return true;
		}
	LOG("warning: MAX_CHANNELS\n");
	return false;
}
//}
