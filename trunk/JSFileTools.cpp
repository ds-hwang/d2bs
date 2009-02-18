/*
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "JSFileTools.h"
#include "File.h"
#include "windows.h"
#include <cstdio>
#include <io.h>
#include <errno.h>
#include "CDebug.h"

#include "debugnew/debug_new.h"

using namespace std;

JSAPI_FUNC(filetools_ctor)
{
	// the ctor should never be called by regular code
	THROW_ERROR(cx, obj, "Could not create the FileTools namespace");
}

JSAPI_FUNC(filetools_remove)
{
	CDebug cDbg("filetools remove");

	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, obj, "You must supply a file name");
	char* file = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
	if(!isValidPath(file))
		THROW_ERROR(cx, obj, "Invalid file name");
	char path[_MAX_PATH+_MAX_FNAME];
	sprintf(path, "%s\\%s", Vars.szScriptPath, file);

	remove(path);

	return JS_TRUE;
}

JSAPI_FUNC(filetools_rename)
{
	CDebug cDbg("filetools rename");

	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, obj, "You must supply an original file name");
	char* orig = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
	if(!isValidPath(orig))
		THROW_ERROR(cx, obj, "Invalid file name");
	char porig[_MAX_PATH+_MAX_FNAME];
	sprintf(porig, "%s\\%s", Vars.szScriptPath, orig);

	if(argc < 2 || !JSVAL_IS_STRING(argv[1]))
		THROW_ERROR(cx, obj, "You must supply a new file name");
	char* newName = JS_GetStringBytes(JSVAL_TO_STRING(argv[1]));
	if(!isValidPath(newName))
		THROW_ERROR(cx, obj, "Invalid file name");
	char pnewName[_MAX_PATH+_MAX_FNAME];
	sprintf(pnewName, "%s\\%s", Vars.szScriptPath, newName);

	rename(porig, pnewName);

	return JS_TRUE;
}

JSAPI_FUNC(filetools_copy)
{
	CDebug cDbg("filetools copy");

	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, obj, "You must supply an original file name");
	char* orig = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
	if(!isValidPath(orig))
		THROW_ERROR(cx, obj, "Invalid file name");
	char porig[_MAX_PATH+_MAX_FNAME];
	sprintf(porig, "%s\\%s", Vars.szScriptPath, orig);

	if(argc < 2 || !JSVAL_IS_STRING(argv[1]))
		THROW_ERROR(cx, obj, "You must supply a new file name");
	char* newName = JS_GetStringBytes(JSVAL_TO_STRING(argv[1]));
	if(!isValidPath(newName))
		THROW_ERROR(cx, obj, "Invalid file name");
	char pnewName[_MAX_PATH+_MAX_FNAME];
	sprintf(pnewName, "%s\\%s", Vars.szScriptPath, newName);

	bool overwrite = false;
	if(argc > 2 && JSVAL_IS_BOOLEAN(argv[2]))
		overwrite = !!JSVAL_TO_BOOLEAN(argv[2]);

	if(overwrite && _access(pnewName, 0) == 0)
		return JS_TRUE;

	FILE* fptr1 = fopen(porig, "r");
	FILE* fptr2 = fopen(pnewName, "w");

	//Sanity check to make sure the file opened for reading!
	if(!fptr1)
	{
		THROW_ERROR(cx, obj, _strerror("Read failed"));
		return JS_TRUE;
	}
	// Same for file opened for writing
	if(!fptr2)
	{
		THROW_ERROR(cx, obj, _strerror("Write failed"));
		return JS_TRUE;
	}

	//int size = _filelength(_fileno(fptr1));// trying someting else.. results wonky with this method..
	// obtain file size:
	//fseek(fptr1 , 0 , SEEK_END);
	//long size = ftell(fptr1); // 44479
	//rewind(fptr1); 
	// the above gives the same results as the filelength().. which is also wrong..
	int size = 0;
	while(!feof(fptr1))	// this on the other hand.. gives an accurate file after copying.. no damn clue why the number does not match!
	{
		fgetc(fptr1);
		size++;
	} 
	rewind(fptr1); // slap the file back to the begining to prepare for the read operation.
	char* contents = new char[size];
	memset(contents, 0, size);
	if(fread(contents, sizeof(char), size, fptr1) != size && ferror(fptr1))
		THROW_ERROR(cx, obj, _strerror("Read failed"));
	if(fwrite(contents, sizeof(char), size, fptr2) != size && ferror(fptr2))
		THROW_ERROR(cx, obj, _strerror("Write failed"));
	fflush(fptr2);
	fclose(fptr2);
	fclose(fptr1);
	delete[] contents;

	return JS_TRUE;
}

JSAPI_FUNC(filetools_exists)
{
	CDebug cDbg("filetools exists");

	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, obj, "Invalid file name");
	char* file = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
	if(!isValidPath(file))
		THROW_ERROR(cx, obj, "Invalid file name");
	char path[_MAX_PATH+_MAX_FNAME];
	sprintf(path, "%s\\%s", Vars.szScriptPath, file);

	*rval = BOOLEAN_TO_JSVAL(!(_access(path, 0) != 0 && errno == ENOENT));

	return JS_TRUE;
}

JSAPI_FUNC(filetools_readText)
{
	CDebug cDbg("filetools readText");

	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, obj, "You must supply an original file name");
	char* orig = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
	if(!isValidPath(orig))
		THROW_ERROR(cx, obj, "Invalid file name");
	char porig[_MAX_PATH+_MAX_FNAME];
	sprintf(porig, "%s\\%s", Vars.szScriptPath, orig);

	if((_access(porig, 0) != 0 && errno == ENOENT))
		THROW_ERROR(cx, obj, "File not found");

	FILE* fptr = fopen(porig, "r");
	fseek(fptr, 0, SEEK_END);
	int size = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	char* contents = new char[size];
	memset(contents, 0, size);
	if(fread(contents, 1, size, fptr) != size && ferror(fptr))
		THROW_ERROR(cx, obj, _strerror("Read failed"));
	fclose(fptr);

	*rval = STRING_TO_JSVAL(JS_NewStringCopyN(cx, contents, size));
	delete[] contents;
	return JS_TRUE;
}

JSAPI_FUNC(filetools_writeText)
{
	CDebug cDbg("filetools writeText");

	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, obj, "You must supply an original file name");
	char* orig = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
	if(!isValidPath(orig))
		THROW_ERROR(cx, obj, "Invalid file name");
	char porig[_MAX_PATH+_MAX_FNAME];
	sprintf(porig, "%s\\%s", Vars.szScriptPath, orig);

	bool result = true;
	FILE* fptr = fopen(porig, "w");
	for(uintN i = 1; i < argc; i++)
		if(!writeValue(fptr, cx, argv[i]))
			result = false;
	fflush(fptr);
	fclose(fptr);
	*rval = BOOLEAN_TO_JSVAL(result);

	return JS_TRUE;
}

JSAPI_FUNC(filetools_appendText)
{
	CDebug cDbg("filetools appendText");

	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, obj, "You must supply an original file name");
	char* orig = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
	if(!isValidPath(orig))
		THROW_ERROR(cx, obj, "Invalid file name");
	char porig[_MAX_PATH+_MAX_FNAME];
	sprintf(porig, "%s\\%s", Vars.szScriptPath, orig);

	bool result = true;
	FILE* fptr = fopen(porig, "a+");
	for(uintN i = 1; i < argc; i++)
		if(!writeValue(fptr, cx, argv[i]))
			result = false;
	fflush(fptr);
	fclose(fptr);
	*rval = BOOLEAN_TO_JSVAL(result);

	return JS_TRUE;
}
