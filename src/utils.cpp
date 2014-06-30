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
#include "utils.h"

#define APK_PACK_NAME "res/drawable/data.jet"
#define get16bits(d) (*((const unsigned short *) (d)))

int Stream::packFilesCount;
PackFile *Stream::packFiles;
int Stream::packOffset;
FILE *Stream::f;

int Stream::packSet;

void Stream::init(const char *packName) {
    packFiles = NULL;
    packFilesCount = 0;

    f = fopen(packName, "rb");
    if (!f) {
        LOG("Stream: error open pack %s\n", packName);
        return;
    }

#ifdef ANDROID
    // read APK format (zip)
	int offset = 0, size;
	unsigned int magic;
	unsigned short count;

// search offset to central directory
	fseek(f, -22, SEEK_END);
	do {
		fread(&magic, 1, 4, f);
		if (magic == 0x06054b50) {
			fseek(f, 6, SEEK_CUR);
			fread(&count, 1, 2, f);
			fread(&size, 1, 4, f);
			fread(&offset, 1, 4, f);
			break;
		}
		fseek(f, -5, SEEK_CUR);
	} while (ftell(f) > 0);

// read central directory
	fseek(f, offset, SEEK_SET);
	char *data = new char[size];
	char *ptr = data;
	fread(data, 1, size, f);
// get pack file offset
	int lenName, lenExtra;
	offset = 0;
	char buf[1024];

	for (int i = 0; i < count; i++) {
		lenName  = *(short*)&ptr[28];
		lenExtra = *(short*)&ptr[30] + *(short*)&ptr[32];
		buf[lenName] = 0;
		memcpy(buf, &ptr[46], lenName);
		if (strcmp(buf, APK_PACK_NAME) == 0) {
			if (*(short*)&ptr[10] != 0) {
				LOG("! FATAL ERROR ! compressed data pack in APK!");
				break;
			}
			offset = *(int*)&ptr[42] + 30 + lenName + lenExtra;
			break;
		}
		ptr = &ptr[46 + lenName + lenExtra];
	}
	delete[] data;
	fseek(f, offset, SEEK_SET);	
#endif
// read file table
	int fpos = ftell(f);
	fread(&packFilesCount, 1, sizeof(packFilesCount), f);
	packFiles = new PackFile[packFilesCount];
	fread(packFiles, 1, sizeof(PackFile) * packFilesCount, f);
	for (int i = 0; i < packFilesCount; i++)
		packFiles[i].offset += fpos;
	packOffset = ftell(f);
}

void Stream::deinit() {
    delete[] packFiles;
    if (f)
        fclose(f);
}

Hash Stream::getHash(const char *name) {
// SuperFastHash algorithm
    int len = strlen(name);
	Hash hash = len, tmp;
	int rem;

    if (len <= 0 || name == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    for (;len > 0; len--) {
        hash  += get16bits (name);
        tmp    = (get16bits (name+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        name  += 4;
        hash  += hash >> 11;
    }

    switch (rem) {
        case 3: hash += get16bits (name);
                hash ^= hash << 16;
                hash ^= name[2] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (name);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += *name;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

Stream::Stream(Hash hash) : ptr(0), pos(0), size(0) {
    for (int i = 0; i < packFilesCount; i++)
        if (packFiles[i].hash == hash) {
            PackFile &p = packFiles[i];

			size = p.size;
            ptr = new char[p.size];
            char *cdata = (p.size != p.csize) ? new char[p.csize] : ptr;

			if (packOffset != p.offset) {
				fseek(f, p.offset, SEEK_SET);
				packSet++;
			}
            fread(cdata, 1, p.csize, f);
			packOffset = p.offset + p.csize;

            if (p.size != p.csize) { // compressed				
				if (lzo_decompress((lzo_bytep)cdata, p.csize, (lzo_bytep)ptr) != p.size) {
					LOG("%d - %d", p.size, size);
                //if (tinf_uncompress(ptr, &size, cdata) != TINF_OK || p.size != size) {
                    LOG("Stream: error uncompress %u\n", p.hash);
                    delete[] ptr;
                    ptr = NULL;
                    size = 0;
                }
                delete[] cdata;
            }
            return;
        }


    LOG("Stream: file not found %u\n", hash);
}

Stream::~Stream() {
	delete ptr;
}

#define UA_GET32(p)			(*(unsigned int*)(p))
#define UA_SET32(p,v)		((*(unsigned int*)(p)) = (unsigned int)(v))
#define UA_COPY32(d,s)		UA_SET32(d, UA_GET32(s))
#define PTR(a)              ((unsigned long) (a))
#define PTR_LINEAR(a)       PTR(a)
#define PTR_ALIGNED2_4(a,b) (((PTR_LINEAR(a) | PTR_LINEAR(b)) & 3) == 0)

int lzo_decompress(const lzo_bytep in, int in_len, lzo_bytep out) {
	register lzo_bytep op;
	register const lzo_bytep ip;
	register unsigned int t;
	register const lzo_bytep m_pos;

	op = out;
	ip = in;

	if (*ip > 17) {
		t = *ip++ - 17;
		if (t < 4)
			goto match_next;
		do *op++ = *ip++; while (--t > 0);
		goto first_literal_run;
	}

	while (1) {
		t = *ip++;
		if (t >= 16)
			goto match;

		if (t == 0) {
			while (*ip == 0) {
				t += 255;
				ip++;
			}
			t += 15 + *ip++;
		}

		if (PTR_ALIGNED2_4(op,ip)) {
			UA_COPY32(op,ip);
			op += 4; ip += 4;
			if (--t > 0) {
				if (t >= 4) {
					do {
						UA_COPY32(op,ip);
						op += 4; ip += 4; t -= 4;
					} while (t >= 4);
					if (t > 0) do *op++ = *ip++; while (--t > 0);
				} else
					do *op++ = *ip++; while (--t > 0);
			}
		} else {
            *op++ = *ip++; *op++ = *ip++; *op++ = *ip++;
            do *op++ = *ip++; while (--t > 0);
		}

first_literal_run:
		t = *ip++;
		if (t >= 16)
			goto match;
		m_pos = op - (1 + 0x0800);
		m_pos -= t >> 2;
		m_pos -= *ip++ << 2;
		*op++ = *m_pos++; *op++ = *m_pos++; *op++ = *m_pos;
		goto match_done;

		do {
match:
			if (t >= 64) {
				m_pos = op - 1;
				m_pos -= (t >> 2) & 7;
				m_pos -= *ip++ << 3;
				t = (t >> 5) - 1;
				goto copy_match;
			} else 
				if (t >= 32) {
					t &= 31;
					if (t == 0)	{
						while (*ip == 0) {
							t += 255;
							ip++;
						}
						t += 31 + *ip++;
					}
					m_pos = op - 1;
					m_pos -= (ip[0] >> 2) + (ip[1] << 6);
					ip += 2;
				} else 
					if (t >= 16) {
						m_pos = op;
						m_pos -= (t & 8) << 11;
						t &= 7;
						if (t == 0)	{
							while (*ip == 0) {
								t += 255;
								ip++;
							}
							t += 7 + *ip++;
						}						
						m_pos -= (ip[0] >> 2) + (ip[1] << 6);
						ip += 2;
						if (m_pos == op)
							goto eof_found;
						m_pos -= 0x4000;
					} else {
						m_pos = op - 1;
						m_pos -= t >> 2;
						m_pos -= *ip++ << 2;
						*op++ = *m_pos++; *op++ = *m_pos;
						goto match_done;
					}

			if (t >= 2 * 4 - (3 - 1) && PTR_ALIGNED2_4(op,m_pos)) {
			//if (t >= 2 * 4 - (3 - 1) && (op - m_pos) >= 4) {
				UA_COPY32(op,m_pos);
				op += 4; m_pos += 4; t -= 4 - (3 - 1);
				do {
					UA_COPY32(op,m_pos);
					op += 4; m_pos += 4; t -= 4;
				} while (t >= 4);
				if (t > 0) do *op++ = *m_pos++; while (--t > 0);
			} else {
copy_match:
				*op++ = *m_pos++; *op++ = *m_pos++;
				do *op++ = *m_pos++; while (--t > 0);
			}
match_done:
			t = ip[-2] & 3;
			if (t == 0)
				break;
match_next:
			*op++ = *ip++;
			if (t > 1) { *op++ = *ip++; if (t > 2) { *op++ = *ip++; } }
			t = *ip++;
		} while (1);
	}
eof_found:
	return op - out;
}
