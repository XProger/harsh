#include "utils.h"
#include <cstdlib>
#include "tinf.h"

#define APK_PACK_NAME "res/drawable/data.jet"
#define get16bits(d) (*((const unsigned short *) (d)))

int Stream::packFilesCount;
PackFile *Stream::packFiles;
FILE *Stream::f;

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
}

void Stream::deinit() {
    delete[] packFiles;
    if (f)
        fclose(f);
}

unsigned int Stream::getHash(const char *name) {
// SuperFastHash algorithm
    int len = strlen(name);
	unsigned int hash = len, tmp;
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

Stream::Stream(unsigned int hash) : ptr(0), pos(0), size(0) {
    for (int i = 0; i < packFilesCount; i++)
        if (packFiles[i].hash == hash) {
            PackFile &p = packFiles[i];

			size = p.size;
            ptr = new char[p.size];
            char *cdata = (p.size != p.csize) ? new char[p.csize] : ptr;

            fseek(f, p.offset, SEEK_SET);
            fread(cdata, 1, p.csize, f);

            if (p.size != p.csize) { // compressed
                if (tinf_uncompress(ptr, &size, cdata) != TINF_OK || p.size != size) {
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
	free(ptr);
}
