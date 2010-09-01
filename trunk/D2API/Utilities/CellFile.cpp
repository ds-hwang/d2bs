#include <windows.h>
#include <io.h>
#include <errno.h>
#include <map>

#include "D2BS.h"
#include "stringhash.h"
#include "D2Helpers.h"
#include "D2Ptrs.h"

DWORD ReadFile(HANDLE hFile, void *buf, DWORD len);
void *memcpy2(void *dest, const void *src, size_t count);
HANDLE OpenFileRead(char *filename);
BYTE *AllocReadFile(char *filename);
CellFile *LoadBmpCellFile(BYTE *buf1, int width, int height);
CellFile *LoadBmpCellFile(char *filename);
CellFile *InitCellFile(CellFile *cf);

std::map<unsigned __int32, CellFile*> g_CachedCellFiles;

CellFile* LoadCellFile(char* lpszPath, DWORD bMPQ)
{
	// AutoDetect the Cell File
	if(bMPQ == 3)
	{
		// Check in our directory first
		HANDLE hFile = OpenFileRead(lpszPath);

		if(hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return LoadCellFile(lpszPath, FALSE);
		}
		else
			return LoadCellFile(lpszPath, TRUE);
	}

	unsigned __int32 hash = sfh(lpszPath, (int)strlen(lpszPath));
	if(g_CachedCellFiles.count(hash) > 0)
		return g_CachedCellFiles[hash];
	if(bMPQ == TRUE)
	{
		CellFile* result = (CellFile*)D2CLIENT_LoadUIImage_ASM(lpszPath);
		g_CachedCellFiles[hash] = result;
		return result;
	}
	else if(bMPQ == FALSE)
	{
		// see if the file exists first
		if(!(_access(lpszPath, 0) != 0 && errno == ENOENT))
		{
			CellFile* result = InitCellFile((CellFile*)LoadBmpCellFile(lpszPath));
			g_CachedCellFiles[hash] = result;
			return result;
		}
	}

	return NULL;
}

DWORD ReadFile(HANDLE hFile, void *buf, DWORD len)
{
	DWORD numdone = 0;
	ReadFile(hFile, buf, len, &numdone, NULL);
	return numdone;
}

void *memcpy2(void *dest, const void *src, size_t count)
{
	return (char *)memcpy(dest, src, count)+count;
}

HANDLE OpenFileRead(char *filename)
{
	return CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

BYTE *AllocReadFile(char *filename)
{
	HANDLE hFile = OpenFileRead(filename);
	int filesize = GetFileSize(hFile, 0);
	if (filesize <= 0) return 0;
	BYTE *buf = new BYTE[filesize];
	ReadFile(hFile, buf, filesize);
	CloseHandle(hFile);
	return buf;
}

CellFile *LoadBmpCellFile(BYTE *buf1, int width, int height)
{
	BYTE *buf2 = new BYTE[(width*height*2)+height], *dest = buf2;

	for (int i = 0; i < height; i++) {
		BYTE *src = buf1+(i*((width+3)&-4)), *limit = src+width;
		while (src < limit) {
			BYTE *start = src, *limit2 = min(limit, src+0x7f), trans = !*src;
			do src++; while ((trans == (BYTE)!*src) && (src < limit2));
			if (!trans || (src < limit)) *dest++ = (BYTE)((trans?0x80:0)+(src-start));
			if (!trans) while (start < src) *dest++ = *start++;
		}
		*dest++ = 0x80;
	}

	static DWORD dc6head[] = {6, 1, 0, 0xeeeeeeee, 1, 1, 0x1c,  0, (DWORD)-1, (DWORD)-1, 0, 0, 0, (DWORD)-1, (DWORD)-1};
	dc6head[8] = width;
	dc6head[9] = height;
	dc6head[14] = dest - buf2;
	dc6head[13] = sizeof(dc6head)+(dc6head[14])+3;
	BYTE *ret = new BYTE[dc6head[13]];
	memset(memcpy2(memcpy2(ret, dc6head, sizeof(dc6head)), buf2, dc6head[14]), 0xee, 3);
	delete[] buf2;

	return (CellFile *)ret;
}

CellFile *LoadBmpCellFile(char *filename)
{
	BYTE *ret = 0;

	BYTE *buf1 = AllocReadFile(filename);
	BITMAPFILEHEADER *bmphead1 = (BITMAPFILEHEADER *)buf1;
	BITMAPINFOHEADER *bmphead2 = (BITMAPINFOHEADER *)(buf1+sizeof(BITMAPFILEHEADER));
	if (buf1 && (bmphead1->bfType == 'MB') && (bmphead2->biBitCount == 8) && (bmphead2->biCompression == BI_RGB)) {
		ret = (BYTE *)LoadBmpCellFile(buf1+bmphead1->bfOffBits, bmphead2->biWidth, bmphead2->biHeight);
	}
	delete[] buf1;

	return (CellFile *)ret;
}

CellFile *InitCellFile(CellFile *cf)
{
	if(cf)
		D2CMP_InitCellFile(cf, &cf, "?", 0, (DWORD)-1, "?");
	return cf;
}
