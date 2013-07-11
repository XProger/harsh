#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>
#include "lzo/lzoconf.h"
#include "lzo/lzo1x.h"

#define USE_LZO1X 1

using namespace std;

void error(const char *text) {
	printf("error: %s\n", text);
}

void printProgress(int x, int n, int w, int size) {
    float ratio = x/(float)n;
    int   c     = ratio * w;
    printf(" %3d%% ", (int)(ratio*100));
    for (int i = 0; i < c; i++) printf("%c", (char)219);
	for (int i = c; i < w; i++) printf("%c", (char)196);
	printf(" %.2f mb", size / (float)(1024 * 1024) );
	printf("\r");
}

LZO_EXTERN(int)
lzo1x_999_compress_internal ( const lzo_bytep in , lzo_uint  in_len,
                                    lzo_bytep out, lzo_uintp out_len,
                                    lzo_voidp wrkmem,
                              const lzo_bytep dict, lzo_uint dict_len,
                                    lzo_callback_p cb,
                                    int try_lazy,
                                    lzo_uint good_length,
                                    lzo_uint max_lazy,
                                    lzo_uint nice_length,
                                    lzo_uint max_chain,
                                    lzo_uint32 flags );

void compress(const char *_data, int _size, char **_cdata, int *_csize, char *wMem) {
// compress
	lzo_uint size = _size;
	lzo_bytep data = (lzo_bytep)_data;
	lzo_uint csize = size + size / 16 + 64 + 3;
	lzo_bytep cdata = (lzo_bytep)new char[csize];

	lzo_uint best_len = size;
	int best_lazy = 0;
	lzo_uint out_len = 0;
    for (int i = 0; i < 6; i++) {
        lzo1x_999_compress_internal(data, size, cdata, &out_len, wMem, NULL, 0, 0, i, 65536L, 65536L, 65536L, 65536L, 0x1);
        if (out_len < best_len) {
            best_len = out_len;
            best_lazy = i;
        }
    }

	if (out_len < size) {
		lzo1x_999_compress_internal(data, size, cdata, &out_len, wMem, NULL, 0, 0, best_lazy, 65536L, 65536L, 65536L, 65536L, 0x1);
//		printf("LZO1X-999:   result: %8lu -> %8lu\n", (unsigned long) size, (unsigned long) out_len);
		lzo_uint orig_len = size;
		lzo1x_optimize(cdata, out_len, data, &orig_len, NULL);
	}

	*_csize = out_len;
	*_cdata = (char*)cdata;
}


string stripPath(const string &path) {
	size_t i = 0;
	string res;
	while (i < path.size()) {
		if (path.at(i) == '-') {
			while (i < path.size()) {
				if (path.at(i) == '/')
					break;
				i++;
			}
		} else
			res = res + path.at(i);
		i++;
	}
	return res;
}

string stripSlash(string &path) {
	string res = path;
	replace(res.begin(), res.end(), '\\', '/');
	return res;
}

#define get16bits(d) (*((const unsigned short *) (d)))
typedef unsigned int Hash;

Hash SFH(const char *data, int len) {
	Hash hash = len, tmp;
	int rem;

    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 4;
        hash  += hash >> 11;
    }

    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= data[2] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += *data;
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

Hash getHash(const string &text) {
	return SFH(text.c_str(), text.size());
}

struct FileItem {
	Hash hash;
	string name;
	int offset, size, csize;
	FileItem(string name, int size) : name(stripSlash(name)), size(size) {
		hash = getHash(stripPath(name));
	}
};

struct FileList : public vector<FileItem*> {
	FileItem* pop_hash(Hash hash) {
		for (size_t i = 0; i < size(); i++)
			if (at(i)->hash == hash) {
				FileItem *item = at(i);
				erase(begin() + i);
				return item;
			}
		error("hash not found!");
		return NULL;
	}
};

FileList* getFileList(const string &OS, const string &basePath, const string &path = "", FileList *list = NULL) {
	if (!list) list = new FileList();

	string str = basePath + path + '*';
	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(str.c_str(), &fd);
	if (!h) return list;
	do {
		str = fd.cFileName;
		if (str.at(0) == '.')
			continue;
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (str.at(0) != '-' || str.find(OS) != string::npos)
				getFileList(OS, basePath, path + str + '/', list);
		} else
			list->push_back(new FileItem(path + str, fd.nFileSizeLow));
	} while (FindNextFile(h, &fd));
	return list;
}

void remap(const string &fileName, FileList **list) {
	FILE *f = fopen("pack.list", "rb");
	if (!f) {
		error("remap list not found\n");
		return;
	}
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	int count = size / sizeof(Hash);
	fseek(f, 0, SEEK_SET);
	Hash *info = new Hash[count];
	fread(info, size, 1, f);
	fclose(f);

	FileList *newList = new FileList();
	for (int i = 0; i < count; i++) {
		FileItem *item = (*list)->pop_hash(info[i]);
		if (item)
			newList->push_back(item);
	}

	for (size_t i = 0; i < (*list)->size(); i++) {
		printf("warning: unused \"%s\"\n", (*list)->at(i)->name.c_str());
		newList->push_back((*list)->at(i));
	}

	delete *list;
	delete info;
	*list = newList;
}

void pack(const string &OS, const string &basePath, const string &fileName, bool useCompress) {
	FileList *list = getFileList(OS, basePath);
	remap("pack.list", &list);

	FILE *f = fopen(fileName.c_str(), "wb");
	if (!f) {
		error("can't create output file");
		return;
	}

	int count = list->size();
	int hsize = count * 16;
	fseek(f, 4 + hsize, SEEK_SET);

	lzo_init();
    char *wMem = new char[LZO1X_999_MEM_COMPRESS];

	int psize = 0;
	int pcur  = 0;
	int pcomp = 0;
	for (int i = 0; i < count; i++)
		psize += list->at(i)->size;

	printf("compressing...\n");
	printProgress(pcur, psize, 62, pcomp);
	for (int i = 0; i < count; i++) {
		FileItem *item = list->at(i);
		item->offset = ftell(f);

		FILE *input = fopen((basePath + item->name).c_str(), "rb");
		if (!input) {
			printf(item->name.c_str());
			error("can't open file");
			continue;
		}
		fseek(input, 0, SEEK_END);
		item->size = ftell(input);
		fseek(input, 0, SEEK_SET);
		char *data = new char[item->size];
		fread(data, item->size, 1, input);
		fclose(input);

		char *cdata;
		if (!useCompress) {
			cdata = NULL;
			item->csize = item->size;
		} else
			compress(data, item->size, &cdata, &item->csize, wMem);

		if (item->csize >= item->size) {
			item->csize = item->size;
			fwrite(data, item->size, 1, f);
		} else
			fwrite(cdata, item->csize, 1, f);
		delete data;
		delete cdata;

		pcur += item->size;
		pcomp += item->csize;
		printProgress(pcur, psize, 62, pcomp);
	}
	delete wMem;

	fseek(f, 0, SEEK_SET);
	fwrite(&count, 4, 1, f);
	for (int i = 0; i < count; i++) {
		FileItem *item = list->at(i);
		fwrite(&item->hash, 4, 1, f);
		fwrite(&item->offset, 4, 1, f);
		fwrite(&item->size, 4, 1, f);
		fwrite(&item->csize, 4, 1, f);
	}
	fclose(f);
	delete list;
	printf("\n");
}

int main(int argc, char *argv[]) {

	//pack("win", "tmp/", "../sys_win/bin/data.jet", true);

	if (argc == 5)
		pack(argv[1], argv[2], argv[3], argv[4][0] == '1');
	else
		printf("format: cpack win/android/ios resDir packDir 0/1");

    return 0;
}
