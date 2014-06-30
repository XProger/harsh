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
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <math.h>
#include <algorithm>
#ifdef ANDROID
	#include <android/log.h>
	#define LOG_TAG "game"
	#define LOG(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#endif

#ifdef WIN32
	#include <windows.h>
	#define LOG(...) printf(__VA_ARGS__)
#endif

#ifdef __APPLE__
    #define LOG(...) printf(__VA_ARGS__)
#endif

#ifdef FLASH
	#define LOG(...) printf(__VA_ARGS__)
#endif

#define randf() ((float)rand()/RAND_MAX)

#define _EPS		1e-6f
#define _PI			3.14159265358979323846f
#define _DEG2RAD	(_PI / 180.0f)
#define _RAD2DEG	(180.0f / _PI)
#define _SQRT2		sqrtf(2.0f)
#define _INV_SQRT2	(1.0f/_SQRT2)

static float _min(float a, float b) { return a < b ? a : b; }
static float _max(float a, float b) { return a > b ? a : b; }
static float _sign(float x) { return x < 0.0f ? -1.0f : (x > 0.0f ? 1.0f : 0.0f); }
static float _clamp(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }

static int _min(int a, int b) { return a < b ? a : b; }
static int _max(int a, int b) { return a > b ? a : b; }
static int _sign(int x) { return x < 0.0f ? -1 : (x > 0 ? 1 : 0); }
static int _clamp(int x, int a, int b) { return x < a ? a : (x > b ? b : x); }

typedef unsigned int Hash;

struct PackFile {
	Hash hash;
	int offset, size, csize;
};

struct Stream {
private:
	void setPtrSize(void *ptr, int size);
	void setSize(int _size);
public:
	char *ptr;
	int pos, size;

	static int packFilesCount;
	static PackFile *packFiles;
	static FILE *f;
	static int packOffset;
	static void init(const char *packName);
	static void deinit();
	static int getFileOffset(unsigned int hash, int *size);
	static Hash getHash(const char *name);

	static int packSet;

	Stream(Hash hash);
	Stream(void *_ptr, int _size);
	~Stream();
	bool    eof() { return pos >= size; }
	int     seek(int offset) { pos += offset; return pos; }
	char*	getData(int count)	{ char *r = &ptr[pos]; pos += count; return r; }
	void	getCopy(void* data, int count)	{ memcpy(data, getData(count), count); }
	char	getChar()	{ char r = ptr[pos]; pos += 1; return r; };
	short	getShort()	{ short r = ((short*)&ptr[pos])[0]; pos += 2; return r; };
	int		getInt()	{ int r = ((int*)&ptr[pos])[0]; pos += 4; return r; };
	float	getFloat()	{ float r = ((float*)&ptr[pos])[0]; pos += 4; return r; };
	char*	getAnsi()	{ int len = getShort(); char *res = new char[len + 1]; memcpy(res, getData(len), len); res[len] = 0; return res; }
};

struct vec2 {
	float x, y;

	explicit vec2(float value = 0.0f) : x(value), y(value) {}
	explicit vec2(float x, float y) : x(x), y(y) {}
	explicit vec2(const int value) : x((float)value), y((float)value) {}
	explicit vec2(const int x, const int y) : x((float)x), y((float)y) {}

	inline vec2& operator += (const vec2 &v) { x+=v.x; y+=v.y; return *this; }
	inline vec2& operator -= (const vec2 &v) { x-=v.x; y-=v.y; return *this; }
	inline vec2& operator *= (const vec2 &v) { x*=v.x; y*=v.y; return *this; }
    inline vec2& operator /= (const vec2 &v) { x/=v.x; y/=v.y; return *this; }
	inline vec2& operator *= (float s) { x*=s; y*=s; return *this; }
	inline vec2& operator /= (float s) { float p=1.0f/s; x*=p; y*=p; return *this; }

	inline vec2 operator + (const vec2 &v) const { return vec2(x+v.x, y+v.y); }
	inline vec2 operator - (const vec2 &v) const { return vec2(x-v.x, y-v.y); }
	inline vec2 operator * (const vec2 &v) const { return vec2(x*v.x, y*v.y); }
	inline vec2 operator / (const vec2 &v) const { return vec2(x/v.x, y/v.y); }
	inline vec2 operator - () const { return vec2(-x, -y); }
	inline vec2 operator * (float s) const { return vec2(x*s, y*s); }
	inline vec2 operator / (float s) const { float p=1.0f/s; return vec2(x*p, y*p); }

	inline float length2() const { return x*x + y*y; }
	inline float length() const { return sqrtf(length2()); }
	inline float angle() const { return atan2f(y, x); }
    inline float dot(const vec2 &v) const { return x*v.x + y*v.y; }
    inline float cross(const vec2 &v) const { return x*v.y-y*v.x; }	// vec3 cross product [z]
	inline void normalize() { *this /= length(); }
	inline vec2 normal() const { return *this / length(); }
    inline vec2 reflect(const vec2 &n) const { return *this - n * (2 * dot(n)); }

	vec2 rotate(float angle) { float s = sinf(angle), c = cosf(angle); return vec2(x*c - y*s, x*s + y*c); }
	vec2 lerp(const vec2 &v, float t) { return *this + (v - *this) * t; }
};

struct vec3 {
	union {
		struct { float x, y, z; };
		float v[3];
	};

	vec3(float value = 0.0f) : x(value), y(value), z(value) {}
	vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	vec3(const vec2 &xy, float z) : x(xy.x), y(xy.y), z(z) {}

	vec2& xy() { return *((vec2*)&x); }

	inline vec3& operator += (const vec3 &v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
	inline vec3& operator -= (const vec3 &v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
	inline vec3& operator *= (const vec3 &v) { x*=v.x; y*=v.y; z*=v.z; return *this; }
    inline vec3& operator /= (const vec3 &v) { x/=v.x; y/=v.y; z/=v.z; return *this; }
	inline vec3& operator *= (float s) { x*=s; y*=s; z*=s; return *this; }
	inline vec3& operator /= (float s) { float p=1.0f/s; x*=p; y*=p; z*=p; return *this; }

	inline vec3 operator + (const vec3 &v) const { return vec3(x+v.x, y+v.y, z+v.z); }
	inline vec3 operator - (const vec3 &v) const { return vec3(x-v.x, y-v.y, z-v.z); }
	inline vec3 operator * (const vec3 &v) const { return vec3(x*v.x, y*v.y, z*v.z); }
	inline vec3 operator / (const vec3 &v) const { return vec3(x/v.x, y/v.y, z/v.z); }
	inline vec3 operator - () const { return vec3(-x, -y, -z); }
	inline vec3 operator * (float s) const { return vec3(x*s, y*s, z*s); }
	inline vec3 operator / (float s) const { float p=1.0f/s; return vec3(x*p, y*p, z*p); }

	inline float dot(const vec3 &v) const { return x*v.x + y*v.y + z*v.z; }
	inline vec3 cross(const vec3 &v) const { return vec3(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline float length2() { return x*x + y*y + z*z; }
	inline float length() { return sqrtf(length2()); }
	inline void normalize() { *this = normal(); }
	inline vec3 normal() { float len = length(); len = len < _EPS ? 0 : 1 / len; return (*this * len); }

    vec3 vmin(const vec3 &v) { return vec3(_min(x, v.x), _min(y, v.y), _min(z, v.z)); }
    vec3 vmax(const vec3 &v) { return vec3(_max(x, v.x), _max(y, v.y), _max(z, v.z)); }
 	vec3 lerp(const vec3 &v, float t) { return *this + (v - *this) * t; }
	vec3 reflect(const vec3 &n) { return *this - n * (dot(n) * 2.0f); }

	vec3 refract(const vec3 &n, float f) {
		float d = dot(n);
		float s = (1.0f - f*f) * (1.0f - d*d);
		return (s < _EPS) ? reflect(n) : (*this * f - n * (sqrtf(s) + d*f));
	}

	vec3 rotate(float angle, const vec3 &axis) {
		float s = sinf(angle), c = cosf(angle);
		vec3 v0 = axis * dot(axis),
			 v1 = *this - v0,
			 v2 = axis.cross(v1);
		return vec3(v0.x + v1.x * c + v2.x * s,
					v0.y + v1.y * c + v2.y * s,
					v0.z + v1.z * c + v2.z * s);
	}
};

struct vec4 {
	union {
		struct { float x, y, z, w; };				
		float v[4];
	};

	vec4(float value = 0.0f) : x(value), y(value), z(value), w(value) {}
	vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	vec4(const vec3 &xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}

	vec3& xyz() { return *((vec3*)&x); }

	inline float dot(const vec3 &v) const { return x*v.x + y*v.y + z*v.z + w; }
};

struct quat {
	float x, y, z, w;

	explicit quat(float x=0, float y=0, float z=0, float w=1) : x(x), y(y), z(z), w(w) {}
	quat(float angle, const vec3 &axis) {
		float a = angle * 0.5f, s = sinf(a);
		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;
		w = cosf(a);
	}

	inline quat& operator += (quat &q) { x+=q.x; y+=q.y; z+=q.z; w+=q.w; return *this; }
	inline quat& operator -= (quat &q) { x-=q.x; y-=q.y; z-=q.z; w-=q.w; return *this; }
	inline quat& operator *= (float s) { x*=s; y*=s; z*=s; w*=s; return *this; }

	inline quat operator + (const quat &q) const { return quat(x+q.x, y+q.y, z+q.z, w+q.w); }
	inline quat operator - (const quat &q) const { return quat(x-q.x, y-q.y, z-q.z, w-q.w); }
	inline quat operator * (float s) const { return quat(x*s, y*s, z*s, w*s); }

	quat operator * (const quat &q) const {
		return quat(w * q.x + x * q.w + y * q.z - z * q.y,
					w * q.y + y * q.w + z * q.x - x * q.z,
					w * q.z + z * q.w + x * q.y - y * q.x,
					w * q.w - x * q.x - y * q.y - z * q.z);
	}

	vec3 operator * (const vec3 &v) const {
		quat q = *this * quat(v.x, v.y, v.z, 0) * inverse();
		return vec3(q.x, q.y, q.z);
	}

	inline quat inverse() const {
		return quat(-x, -y, -z, w);
	}

	inline float dot(const quat &q) const { return x*q.x + y*q.y + z*q.z + w*q.w; }

	inline quat normal() const {
		float len = 1.0f / sqrtf(x*x + y*y + z*z + w*w);
		return quat(x*len, y*len, z*len, w*len);
	}

	quat lerp(const quat &q, float t) const {
		return dot(q) < 0 ? (*this - (q + *this) * t) : (*this + (q - *this) * t);
	}
};

struct quat2 {
	quat real, dual;

	explicit quat2() : real(), dual() {}

	quat2(const quat &real, const quat &dual): real(real), dual(dual) {}

	quat2(const quat &rot, const vec3 &pos) {
		real = rot;
		dual = quat(pos.x, pos.y, pos.z, 0) * (rot * 0.5);
	}

	quat2 operator * (const quat2 &q) const {
		return quat2(real * q.real, real * q.dual + dual * q.real);
	}

	quat2 lerp(const quat2 &q, float t) {
		return real.dot(q.real) < 0 ? quat2(real - (q.real + real) * t, dual - (q.dual + dual) * t) :
									  quat2(real + (q.real - real) * t, dual + (q.dual - dual) * t);
	}

	vec3 getPos() {
		return vec3((dual.x * real.w - real.x * dual.w + real.y * dual.z - real.z * dual.y) * 2.0f,
					(dual.y * real.w - real.y * dual.w + real.z * dual.x - real.x * dual.z) * 2.0f,
					(dual.z * real.w - real.z * dual.w + real.x * dual.y - real.y * dual.x) * 2.0f);
	}
};

struct mat4 {
	union {
		struct {
			float e00, e10, e20, e30;
			float e01, e11, e21, e31;
			float e02, e12, e22, e32;
			float e03, e13, e23, e33;
		};
		float e[16];
	};

	mat4() {};

	mat4(float value) { identity(value); };

	mat4(float angle, const vec3 &axis) {
		float s = sinf(angle), c = cosf(angle), ic = 1 - c,
			xy = axis.x * axis.y,	yz = axis.y * axis.z,	zx = axis.z * axis.x,
			xs = axis.x * s,		ys = axis.y * s,		zs = axis.z * s,
			ixy = ic * xy,			iyz = ic * yz,			izx = ic * zx;
		e00 = ic * axis.x * axis.x + c;	e01 = ixy - zs;					e02 = izx + ys;
		e10 = ixy + zs;					e11 = ic * axis.y * axis.y + c;	e12 = iyz - xs;
		e20 = izx - ys;					e21 = iyz + xs;					e22 = ic * axis.z * axis.z + c;
		e03 = e13 = e23 = e30 = e31 = e32 = 0;
		e33 = 1;
	}

	mat4 operator * (const mat4 &m) {
		mat4 r;
		r.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20 + e03 * m.e30;
		r.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20 + e13 * m.e30;
		r.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20 + e23 * m.e30;
		r.e30 = e30 * m.e00 + e31 * m.e10 + e32 * m.e20 + e33 * m.e30;
		r.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21 + e03 * m.e31;
		r.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21 + e13 * m.e31;
		r.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21 + e23 * m.e31;
		r.e31 = e30 * m.e01 + e31 * m.e11 + e32 * m.e21 + e33 * m.e31;
		r.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22 + e03 * m.e32;
		r.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22 + e13 * m.e32;
		r.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22 + e23 * m.e32;
		r.e32 = e30 * m.e02 + e31 * m.e12 + e32 * m.e22 + e33 * m.e32;
		r.e03 = e00 * m.e03 + e01 * m.e13 + e02 * m.e23 + e03 * m.e33;
		r.e13 = e10 * m.e03 + e11 * m.e13 + e12 * m.e23 + e13 * m.e33;
		r.e23 = e20 * m.e03 + e21 * m.e13 + e22 * m.e23 + e23 * m.e33;
		r.e33 = e30 * m.e03 + e31 * m.e13 + e32 * m.e23 + e33 * m.e33;
		return r;
	}

	vec3 operator * (const vec3 &v) {
		return vec3(
			e00 * v.x + e01 * v.y + e02 * v.z + e03,
			e10 * v.x + e11 * v.y + e12 * v.z + e13,
			e20 * v.x + e21 * v.y + e22 * v.z + e23);
	}

	vec4 operator * (const vec4 &v) {
		return vec4(
			e00 * v.x + e01 * v.y + e02 * v.z + e03 * v.w,
			e10 * v.x + e11 * v.y + e12 * v.z + e13 * v.w,
			e20 * v.x + e21 * v.y + e22 * v.z + e23 * v.w,
			e30 * v.x + e31 * v.y + e32 * v.z + e33 * v.w);
	}

	void identity(float s = 1) {
		e00 = s;	e10 = 0;	e20 = 0;	e30 = 0;
		e01 = 0;	e11 = s;	e21 = 0;	e31 = 0;
		e02 = 0;	e12 = 0;	e22 = s;	e32 = 0;
		e03 = 0;	e13 = 0;	e23 = 0;	e33 = s;
	}

	void setPos(const vec3 &v) {
		e03 = v.x;
		e13 = v.y;
		e23 = v.z;
	}

	vec3 getPos() {
		return vec3(e03, e13, e23);
	}

	void setRot(const quat &q) {
		float	sx = q.x * q.x,
				sy = q.y * q.y,
				sz = q.z * q.z,
				sw = q.w * q.w,
				inv = 1.0f / (sx + sy + sz + sw);

		e00 = ( sx - sy - sz + sw) * inv;
		e11 = (-sx + sy - sz + sw) * inv;
		e22 = (-sx - sy + sz + sw) * inv;
		inv *= 2.0f;

		float t1 = q.x * q.y;
		float t2 = q.z * q.w;
		e10 = (t1 + t2) * inv;
		e01 = (t1 - t2) * inv;

		t1 = q.x * q.z;
		t2 = q.y * q.w;
		e20 = (t1 - t2) * inv;
		e02 = (t1 + t2) * inv;

		t1 = q.y * q.z;
		t2 = q.x * q.w;
		e21 = (t1 + t2) * inv;
		e12 = (t1 - t2) * inv;
	}

	quat getRot() {
		float t, s;
		t = 1.0f + e00 + e11 + e22;
		if (t > _EPS) {
			s = 0.5f / sqrtf(t);
			return quat((e21 - e12) * s, (e02 - e20) * s, (e10 - e01) * s, 0.25f / s);
		} else
			if (e00 > e11 && e00 > e22) {
				s = 0.5f / sqrtf(1.0f + e00 - e11 - e22);
				return quat(0.25f / s, (e01 + e10) * s, (e02 + e20) * s, (e21 - e12) * s);
			} else
				if (e11 > e22) {
					s = 0.5f / sqrtf(1.0f - e00 + e11 - e22);
					return quat((e01 + e10) * s, 0.25f / s, (e12 + e21) * s, (e02 - e20) * s);
				} else {
					s = 0.5f / sqrtf(1.0f - e00 - e11 + e22);
					return quat((e02 + e20) * s, (e12 + e21) * s, 0.25f / s, (e10 - e01) * s);
				}
	}

	void translate(const vec3 &v) {
		mat4 m(1);
		m.setPos(v);
		*this = *this * m;
	}

	void rotate(float angle, const vec3& axis) {
		mat4 m(angle, axis);
		*this = *this * m;
	}

	void scale(const vec3 &v) {
		mat4 m(1);
		m.e00 = v.x;
		m.e11 = v.y;
		m.e22 = v.z;
		*this = *this * m;
	}

	float det() const {
		return	e00 * (e11 * (e22 * e33 - e32 * e23) - e21 * (e12 * e33 - e32 * e13) + e31 * (e12 * e23 - e22 * e13)) -
				e10 * (e01 * (e22 * e33 - e32 * e23) - e21 * (e02 * e33 - e32 * e03) + e31 * (e02 * e23 - e22 * e03)) +
				e20 * (e01 * (e12 * e33 - e32 * e13) - e11 * (e02 * e33 - e32 * e03) + e31 * (e02 * e13 - e12 * e03)) -
				e30 * (e01 * (e12 * e23 - e22 * e13) - e11 * (e02 * e23 - e22 * e03) + e21 * (e02 * e13 - e12 * e03));
	}

	mat4 inverse() const {
		float idet = 1.0f / det();
		mat4 r;
		r.e00 =  (e11 * (e22 * e33 - e32 * e23) - e21 * (e12 * e33 - e32 * e13) + e31 * (e12 * e23 - e22 * e13)) * idet;
		r.e01 = -(e01 * (e22 * e33 - e32 * e23) - e21 * (e02 * e33 - e32 * e03) + e31 * (e02 * e23 - e22 * e03)) * idet;
		r.e02 =  (e01 * (e12 * e33 - e32 * e13) - e11 * (e02 * e33 - e32 * e03) + e31 * (e02 * e13 - e12 * e03)) * idet;
		r.e03 = -(e01 * (e12 * e23 - e22 * e13) - e11 * (e02 * e23 - e22 * e03) + e21 * (e02 * e13 - e12 * e03)) * idet;
		r.e10 = -(e10 * (e22 * e33 - e32 * e23) - e20 * (e12 * e33 - e32 * e13) + e30 * (e12 * e23 - e22 * e13)) * idet;
		r.e11 =  (e00 * (e22 * e33 - e32 * e23) - e20 * (e02 * e33 - e32 * e03) + e30 * (e02 * e23 - e22 * e03)) * idet;
		r.e12 = -(e00 * (e12 * e33 - e32 * e13) - e10 * (e02 * e33 - e32 * e03) + e30 * (e02 * e13 - e12 * e03)) * idet;
		r.e13 =  (e00 * (e12 * e23 - e22 * e13) - e10 * (e02 * e23 - e22 * e03) + e20 * (e02 * e13 - e12 * e03)) * idet;
		r.e20 =  (e10 * (e21 * e33 - e31 * e23) - e20 * (e11 * e33 - e31 * e13) + e30 * (e11 * e23 - e21 * e13)) * idet;
		r.e21 = -(e00 * (e21 * e33 - e31 * e23) - e20 * (e01 * e33 - e31 * e03) + e30 * (e01 * e23 - e21 * e03)) * idet;
		r.e22 =  (e00 * (e11 * e33 - e31 * e13) - e10 * (e01 * e33 - e31 * e03) + e30 * (e01 * e13 - e11 * e03)) * idet;
		r.e23 = -(e00 * (e11 * e23 - e21 * e13) - e10 * (e01 * e23 - e21 * e03) + e20 * (e01 * e13 - e11 * e03)) * idet;
		r.e30 = -(e10 * (e21 * e32 - e31 * e22) - e20 * (e11 * e32 - e31 * e12) + e30 * (e11 * e22 - e21 * e12)) * idet;
		r.e31 =  (e00 * (e21 * e32 - e31 * e22) - e20 * (e01 * e32 - e31 * e02) + e30 * (e01 * e22 - e21 * e02)) * idet;
		r.e32 = -(e00 * (e11 * e32 - e31 * e12) - e10 * (e01 * e32 - e31 * e02) + e30 * (e01 * e12 - e11 * e02)) * idet;
		r.e33 =  (e00 * (e11 * e22 - e21 * e12) - e10 * (e01 * e22 - e21 * e02) + e20 * (e01 * e12 - e11 * e02)) * idet;
		return r;
	}

	mat4 transpose() const {
		mat4 r;
		r.e00 = e00; r.e10 = e01; r.e20 = e02; r.e30 = e03;
		r.e01 = e10; r.e11 = e11; r.e21 = e12; r.e31 = e13;
		r.e02 = e20; r.e12 = e21; r.e22 = e22; r.e32 = e23;
		r.e03 = e30; r.e13 = e31; r.e23 = e32; r.e33 = e33;
		return r;
	}

	void ortho(float l, float r, float b, float t, float znear, float zfar) {
		e00 = 2.0f / (r - l);
		e11 = 2.0f / (t - b);
		e22 = 2.0f / (znear - zfar);
		e03 = (l + r) / (l - r);
		e13 = (b + t) / (b - t);
		e23 = (zfar + znear) / (znear - zfar);
		e10 = e20 = e30 = e01 = e21 = e31 = e02 = e12 = e32 = 0;
		e33 = 1;
	}

	void frustum(float l, float r, float b, float t, float znear, float zfar) {
		e00 = 2.0f * znear / (r - l);
		e11 = 2.0f * znear / (t - b);
		e02 = (r + l) / (r - l);
		e12 = (t + b) / (t - b);
		e22 = (znear + zfar) / (znear - zfar);
		e32 = -1;
		e23 = 2.0f * zfar * znear / (znear - zfar);
		e10 = e20 = e30 = e01 = e21 = e31 = e03 = e13 = e33 = 0;
	}

	void perspective(float FOV, float aspect, float znear, float zfar, bool vertFOV) {
		float x, y, k = znear * tanf(FOV * 0.5f * _DEG2RAD);
		if (vertFOV) {
			y = k;
			x = y * aspect;
		} else {
			x = k;
			y = x / aspect;
		}
		frustum(-x, x, -y, y, znear, zfar);
	}

	void perspective(float FOV, float aspect, float znear, float zfar) {
		perspective(FOV, aspect, znear, zfar, true);
	}

	void lookAt(const vec3 &pos, const vec3 &target, const vec3 &up) {
		/*
		vec3 r, u, d;
		d = (pos - target).normal();
		r = up.cross(d).normal();
		u = d.cross(r);
		e00 = r.x;	e01 = r.y; e02 = r.z; e03 = -pos.dot(r);
		e10 = u.x;	e11 = u.y; e12 = u.z; e13 = -pos.dot(u);
		e20 = d.x;	e21 = d.y; e22 = d.z; e23 = -pos.dot(d);
		e30 = e31 = e32 = 0;
		e33 = 1;
		*/
	}
};

struct rectf {
    float l, t, r, b;   // left, top, right, bottom
    rectf() : l(0), t(0), r(0), b(0) {}
    rectf(float l, float t, float r, float b) : l(l), t(t), r(r), b(b) {}

	inline rectf& operator += (float v) { l-=v; t-=v; r+=v; b+=v; return *this; }
	inline rectf& operator -= (float v) { l+=v; t+=v; r-=v; b-=v; return *this; }

    bool intersect(const rectf &rect) {
        return !(l >= rect.r || r <= rect.l || t >= rect.b || b <= rect.t);
    }

    bool intersect(const vec2 &v) {
        return (v.x >= l && v.x < r && v.y >= t && v.y < b);
    }
};

struct Box {
    vec3 min, max;
};

struct ListItem {
	ListItem *next, *prev;
	virtual ~ListItem() {}
};

struct List {
	ListItem *first;

	List() : first(0) {}

	void clear() {
		ListItem *temp, *item = first;
		while (item) {
			temp = item;
			item = item->next;
			delete temp;
		}
		first = NULL;
	}

	ListItem* add(ListItem *item) {
		item->prev = NULL;
		item->next = first;
		if (first)
			first->prev = item;
		first = item;
		return item;
	}

	ListItem* remove(ListItem *item, bool free = false) {
		if (item->prev)
			item->prev->next = item->next;
		if (item->next)
			item->next->prev = item->prev;
		if (first == item)
			first = item->next;

        ListItem *nextItem = item->next;
        if (free)
            delete item;
        return nextItem;
	}

	ListItem* remove(ListItem *begin, ListItem *end, bool free = false) {
	    while (begin && begin != end)
            begin = remove(begin, free);
        return begin;
	}
};

struct Array {
private:
    void **items;
    int mLength;
public:
    Array() : items(NULL), mLength(0) {}
    ~Array() { delete[] items; }

    void* get(const int index) {
        return items[index];
    }

    int length() {
        return mLength;
    }

    void setLength(int newLength) {
        newLength = _max(0, newLength);
        if (newLength == mLength)
            return;

        void **newItems;
        if (newLength) {
            newItems = (void**)(new char[newLength * sizeof(void*)]);
            memcpy(newItems, items, _min(mLength, newLength) * sizeof(void*));
        } else
            newItems = NULL;

        delete[] items;
        items = newItems;
        mLength = newLength;
    }

    int indexOf(void *item) {
        for (int i = 0; i < mLength; i++)
            if (items[i] == item)
                return i;
        return -1;
    }

    void push(void *item) {
        setLength(mLength + 1);
        items[mLength - 1] = item;
    }

    void* pop() {
        if (mLength) {
            void *item = items[mLength - 1];
            setLength(mLength - 1);
            return item;
        } else
            return NULL;
    }

    void remove(int index) {
        if (index < 0 || index >= mLength)
            return;
        items[index] = items[mLength - 1];
        setLength(mLength - 1);
    }

    void sort(void *compareFunc) {
        // TODO
    }
};

struct String {
	char *data;

	String(const char *str = 0) : data(String::copy(str)) {}
	~String() { delete data; }

	bool operator == (const char *str) const { 
		return String::cmp(data, str) == 0;
	}

	String operator + (const char *str) {
		int d1 = String::length(data),
			d2 = String::length(str);
		char *res = new char[d1 + d2 + 1];
		if (d1) memcpy(res, data, d1);
		if (d2) memcpy(&res[d1], str, d2);
		res[d1 + d2] = 0;
		return String(res);
	}

	static char* copy(const char *str) {
		if (!str)
			return 0;
		int length = strlen(str);
		char *res = new char[length + 1];
		memcpy(res, str, length);
		res[length] = 0;
		return res;
	}

	static int cmp(const char *str1, const char *str2) {
		return (str1 == 0 && str2 == 0) ? 0 : (str1 == 0 ? -1 : (str2 == 0 ? 1 : strcmp(str1, str2)));
	}

	static int length(const char *str) {
		return str ? strlen(str) : 0;
	}

	static void replace(char *str, char src, char dst) {
		int len = length(str);
		for (int i = 0; i < len; i++)
			if (str[i] == src)
				str[i] = dst;
	}
};


#define lzo_bytep	unsigned char*
#define lzo_voidp	void*
int lzo_decompress(const lzo_bytep in, int in_len, lzo_bytep out);

#endif // UTILS_H
