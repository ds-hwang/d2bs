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

#define _USE_32BIT_TIME_T

#include "JSFile.h"
#include "File.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include "CDebug.h"

#include "debugnew/debug_new.h"

struct FileData {
	int mode;
	char* path;
	bool autoflush, locked;
	FILE* fptr;
};

JSAPI_FUNC(file_ctor)
{
	// the ctor should never be called by regular code
	THROW_ERROR(cx, obj, "Could not create the File namespace");
}

JSBool file_equality(JSContext *cx, JSObject *obj, jsval v, JSBool *bp)
{
	CDebug cDbg("file equality");

	*bp = JS_FALSE;
	if(JSVAL_IS_OBJECT(v) && !JSVAL_IS_VOID(v) && !JSVAL_IS_NULL(v)) {
		FileData* ptr = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
		FileData* ptr2 = (FileData*)JS_GetInstancePrivate(cx, JSVAL_TO_OBJECT(v), &file_class_ex.base, NULL);
		if(ptr && ptr2 && !_strcmpi(ptr->path, ptr2->path) && ptr->mode == ptr2->mode &&
			ftell(ptr->fptr) == ftell(ptr2->fptr))
			*bp = JS_TRUE;
	}
	return JS_TRUE;
}

JSAPI_PROP(file_getProperty)
{
	CDebug cDbg("file getProperty");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	struct _stat filestat;
	if(fdata->fptr)
		_fstat(_fileno(fdata->fptr), &filestat);
	if(fdata) {
		switch(JSVAL_TO_INT(id)) {
			case FILE_READABLE:
				*vp = BOOLEAN_TO_JSVAL((fdata->fptr && !feof(fdata->fptr) && !ferror(fdata->fptr)));
				break;
			case FILE_WRITEABLE:
				*vp = BOOLEAN_TO_JSVAL((fdata->fptr && !ferror(fdata->fptr) && (fdata->mode % 3) > FILE_READ));
				break;
			case FILE_SEEKABLE:
				*vp = BOOLEAN_TO_JSVAL((fdata->fptr && !ferror(fdata->fptr)));
				break;
			case FILE_MODE:
				*vp = INT_TO_JSVAL((fdata->mode % 3));
				break;
			case FILE_BINARYMODE:
				*vp = BOOLEAN_TO_JSVAL((fdata->mode > 2));
				break;
			case FILE_LENGTH:
				if(fdata->fptr) *vp = INT_TO_JSVAL(_filelength(_fileno(fdata->fptr)));
				else *vp = JSVAL_ZERO;
				break;
			case FILE_PATH:
				*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, fdata->path+strlen(Vars.szScriptPath)+1));
				break;
			case FILE_POSITION:
				if(fdata->fptr) *vp = INT_TO_JSVAL(ftell(fdata->fptr));
				else *vp = JSVAL_ZERO;
				break;
			case FILE_EOF:
				if(fdata->fptr) *vp = BOOLEAN_TO_JSVAL(feof(fdata->fptr));
				else *vp = JSVAL_TRUE;
				break;
			case FILE_AUTOFLUSH:
				*vp = BOOLEAN_TO_JSVAL(fdata->autoflush);
				break;
			case FILE_ACCESSED:
				if(fdata->fptr) JS_NewNumberValue(cx, (jsdouble)filestat.st_atime, vp);
				else *vp = JSVAL_ZERO;
				break;
			case FILE_MODIFIED:
				if(fdata->fptr) JS_NewNumberValue(cx, (jsdouble)filestat.st_mtime, vp);
				else *vp = JSVAL_ZERO;
				break;
			case FILE_CREATED:
				if(fdata->fptr) JS_NewNumberValue(cx, (jsdouble)filestat.st_ctime, vp);
				else *vp = JSVAL_ZERO;
				break;
		}
	} else THROW_ERROR(cx, obj, "Couldn't get file object");
	return JS_TRUE;
}

JSAPI_PROP(file_setProperty)
{
	CDebug cDbg("file setProperty");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata) {
		switch(JSVAL_TO_INT(id)) {
			case FILE_AUTOFLUSH:
				if(JSVAL_IS_BOOLEAN(*vp))
					fdata->autoflush = !!JSVAL_TO_BOOLEAN(*vp);
				break;
		}
	} else THROW_ERROR(cx, obj, "Couldn't get file object");
	return JS_TRUE;
}

JSAPI_FUNC(file_open)
{
	CDebug cDbg("file open");

	if(argc < 2)
		THROW_ERROR(cx, obj, "Not enough parameters, 2 or more expected");
	if(!JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, obj, "Parameter 1 not a string");
	if(!JSVAL_IS_INT(argv[1]))
		THROW_ERROR(cx, obj, "Parameter 2 not a mode");

	char* file = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
	// check for attempts to break the sandbox and for invalid file name characters
	if(!isValidPath(file))
		THROW_ERROR(cx, obj, "Invalid file name");

	int32 mode;
	JS_ValueToInt32(cx, argv[1], &mode);
	// this could be simplified to: mode > FILE_APPEND || mode < FILE_READ
	// but then it takes a hit for readability
	if(!(mode == FILE_READ || mode == FILE_WRITE || mode == FILE_APPEND))
		THROW_ERROR(cx, obj, "Invalid file mode");

	bool binary = false, autoflush = false, lockFile = false;
	if(argc > 2 && JSVAL_IS_BOOLEAN(argv[2]))
		binary = !!JSVAL_TO_BOOLEAN(argv[2]);
	if(argc > 3 && JSVAL_IS_BOOLEAN(argv[3]))
		autoflush = !!JSVAL_TO_BOOLEAN(argv[3]);
	if(argc > 4 && JSVAL_IS_BOOLEAN(argv[4]))
		lockFile = !!JSVAL_TO_BOOLEAN(argv[4]);

	if(binary)
		mode += 3;
	static const char* modes[] = {"rt", "w+t", "a+t", "rb", "w+b", "a+b"};
	char path[_MAX_FNAME+_MAX_PATH];
	sprintf(path, "%s\\%s", Vars.szScriptPath, file);
	FILE* fptr = fopen(path, modes[mode]);
	if(!fptr)
		return ThrowJSError(cx, obj, "Couldn't open file %s: %s", file, _strerror(NULL));

	FileData* fdata = new FileData; // leaked?
	if(!fdata)
	{
		fclose(fptr);
		THROW_ERROR(cx, obj, "Couldn't allocate memory for the FileData object");
	}

	fdata->mode = mode;
	fdata->path = _strdup(path);
	fdata->autoflush = autoflush;
	fdata->locked = lockFile;
	fdata->fptr = fptr;
	if(lockFile)
		_lock_file(fptr);

	JSObject* res = BuildObject(cx, &file_class_ex.base, file_methods, file_props, fdata);
	if(!res)
	{
		fclose(fptr);
		delete fdata;
		THROW_ERROR(cx, obj, "Failed to define the file object");
	}
	*rval = OBJECT_TO_JSVAL(res);
	return JS_TRUE;
}

JSAPI_FUNC(file_close)
{
	CDebug cDbg("file close");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata)
		if(fdata->fptr) {
			if(fdata->locked)
				_unlock_file(fdata->fptr);
			if(!!fclose(fdata->fptr))
				THROW_ERROR(cx, obj, _strerror("Close failed"));
			fdata->fptr = NULL;
		} else THROW_ERROR(cx, obj, "File is not open");
	*rval = OBJECT_TO_JSVAL(obj);
	return JS_TRUE;
}

JSAPI_FUNC(file_reopen)
{
	CDebug cDbg("file reopen");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata)
		if(!fdata->fptr) {
			static const char* modes[] = {"rt", "w+t", "a+t", "rb", "w+b", "a+b"};
			fdata->fptr = fopen(fdata->path, modes[fdata->mode]);
			if(!fdata->fptr)
				THROW_ERROR(cx, obj, _strerror("Could not reopen file"));
		} else THROW_ERROR(cx, obj, "File is not closed");
	*rval = OBJECT_TO_JSVAL(obj);
	return JS_TRUE;
}

JSAPI_FUNC(file_read)
{
	CDebug cDbg("file read");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata && fdata->fptr) {
		clearerr(fdata->fptr);
		int32 count = 1;
		if(argc > 0 && JSVAL_TO_INT(argv[0]) > 0)
			JS_ValueToInt32(cx, argv[0], &count);
		if(fdata->mode > 2) {
			// binary mode
			int* result = new int[count+1];
			memset(result, NULL, count+1);
			if(fread(result, sizeof(int), count, fdata->fptr) != count && ferror(fdata->fptr))
				THROW_ERROR(cx, obj, _strerror("Read failed"));
			if(count == 1) {
				*rval = INT_TO_JSVAL(result[0]);
			} else {
				JSObject* arr = JS_NewArrayObject(cx, 0, NULL);
				for(int i = 0; i < count; i++) {
					jsval val = INT_TO_JSVAL(result[i]);
					JS_SetElement(cx, arr, i, &val);
				}
				*rval = OBJECT_TO_JSVAL(arr);
			}
		} else {
			// text mode
			fflush(fdata->fptr);
			char* result = new char[count+1];
			memset(result, NULL, count+1);
			if(fread(result, sizeof(char), count, fdata->fptr) != count && ferror(fdata->fptr))
				THROW_ERROR(cx, obj, _strerror("Read failed"));
			*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, result));
			delete[] result;
		}
	}
	return JS_TRUE;
}

JSAPI_FUNC(file_readLine)
{
	CDebug cDbg("file readLine");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata && fdata->fptr) {
		const char* line = readLine(fdata->fptr);
		if(strlen(line) == 0)
			return JS_TRUE;
		if(!line)
			THROW_ERROR(cx, obj, _strerror("Read failed"));
		*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, line));
		delete[] line;
	}
	return JS_TRUE;
}

JSAPI_FUNC(file_readAllLines)
{
	CDebug cDbg("file readAllLines");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata && fdata->fptr) {
		JSObject* arr = JS_NewArrayObject(cx, 0, NULL);
		int i = 0;
		while(!feof(fdata->fptr)) {
			const char* line = readLine(fdata->fptr);
			if(!line)
				THROW_ERROR(cx, obj, _strerror("Read failed"));
			jsval val = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, line));
			JS_SetElement(cx, arr, i++, &val);
			delete[] line;
		}
		*rval = OBJECT_TO_JSVAL(arr);
	}
	return JS_TRUE;
}

JSAPI_FUNC(file_readAll)
{
	CDebug cDbg("file readAll");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata && fdata->fptr) {
		fseek(fdata->fptr, 0, SEEK_END);
		int size = ftell(fdata->fptr);
		fseek(fdata->fptr, 0, SEEK_SET);
		char* contents = new char[size];
		if(fread(contents, sizeof(char), size, fdata->fptr) != size && ferror(fdata->fptr))
			THROW_ERROR(cx, obj, _strerror("Read failed"));
		*rval = STRING_TO_JSVAL(JS_NewStringCopyN(cx, contents, size));
		delete[] contents;
	}
	return JS_TRUE;

}

JSAPI_FUNC(file_write)
{
	CDebug cDbg("file write");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata && fdata->fptr) {
		for(uintN i = 0; i < argc; i++)
			writeValue(fdata->fptr, cx, argv[i], !!(fdata->mode > 2));

		if(fdata->autoflush)
			fflush(fdata->fptr);
	}
	*rval = OBJECT_TO_JSVAL(obj);
	return JS_TRUE;
}

JSAPI_FUNC(file_seek)
{
	CDebug cDbg("file seek");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata && fdata->fptr) {
		if(argc > 0) {
			int32 bytes;
			bool isLines = false, fromStart = false;
			JS_ValueToInt32(cx, argv[0], &bytes);
			if(argc > 1 && JSVAL_IS_BOOLEAN(argv[1]))
				isLines = !!JSVAL_TO_BOOLEAN(argv[1]);
			if(argc > 2 && JSVAL_IS_BOOLEAN(argv[2]))
				fromStart = !!JSVAL_TO_BOOLEAN(argv[1]);
			if(!isLines) {
				if(fdata->locked && _fseek_nolock(fdata->fptr, ftell(fdata->fptr)+bytes, SEEK_CUR)) {
					THROW_ERROR(cx, obj, _strerror("Seek failed"));
				} else if(fseek(fdata->fptr, ftell(fdata->fptr)+bytes, SEEK_CUR)) {
					THROW_ERROR(cx, obj, _strerror("Seek failed"));
				}
			} else {
				if(fromStart)
					rewind(fdata->fptr);
				// semi-ugly hack to seek to the specified line
				// if I were unlazy I wouldn't be allocating/deallocating all this memory, but for now it's ok
				while(bytes--)
					delete[] readLine(fdata->fptr);
			}
		} else
			THROW_ERROR(cx, obj, "Not enough parameters");
	}
	*rval = OBJECT_TO_JSVAL(obj);
	return JS_TRUE;
}

JSAPI_FUNC(file_flush)
{
	CDebug cDbg("file flush");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata && fdata->fptr) {
		fflush(fdata->fptr);
	}
	*rval = OBJECT_TO_JSVAL(obj);
	return JS_TRUE;
}

JSAPI_FUNC(file_reset)
{
	CDebug cDbg("file reset");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata && fdata->fptr) {
		if(fdata->locked && _fseek_nolock(fdata->fptr, 0L, SEEK_SET)) {
			THROW_ERROR(cx, obj, _strerror("Seek failed"));
		} else if(fseek(fdata->fptr, 0L, SEEK_SET)) {
			THROW_ERROR(cx, obj, _strerror("Seek failed"));
		}
	}
	*rval = OBJECT_TO_JSVAL(obj);
	return JS_TRUE;
}

JSAPI_FUNC(file_end)
{
	CDebug cDbg("file end");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata && fdata->fptr) {
		if(fdata->locked && _fseek_nolock(fdata->fptr, 0L, SEEK_END)) {
			THROW_ERROR(cx, obj, _strerror("Seek failed"));
		} else if(fseek(fdata->fptr, 0L, SEEK_END)) {
			THROW_ERROR(cx, obj, _strerror("Seek failed"));
		}
	}
	*rval = OBJECT_TO_JSVAL(obj);
	return JS_TRUE;
}

void file_finalize(JSContext *cx, JSObject *obj)
{
	CDebug cDbg("file finalize");

	FileData* fdata = (FileData*)JS_GetInstancePrivate(cx, obj, &file_class_ex.base, NULL);
	if(fdata) {
		free(fdata->path);
		if(fdata->locked && fdata->fptr)
			_unlock_file(fdata->fptr);
		if(fdata->fptr)
			fclose(fdata->fptr);
	}
}
