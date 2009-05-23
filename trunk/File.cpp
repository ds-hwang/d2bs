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

#include "File.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <string>

#include "debugnew/debug_new.h"

using namespace std;

const char* readLine(FILE* fptr)
{
	if(feof(fptr))
		return NULL;
	string buffer;
	char c = 0;
	// grab all the characters in this line
	do {
		c = fgetc(fptr);
		// append the new character unless it's a carriage return
		if(c != '\r')
			buffer.append(1, c);
	} while(!feof(fptr) && c != '\r');
	// clear the newline from the stream, and assume that the
	// lines are \r\n terminated (generally the case)
	fgetc(fptr);
	return _strdup(buffer.c_str());
}

bool writeValue(FILE* fptr, JSContext* cx, jsval value, bool isBinary)
{
	char* str;
	int len = 0, result;
	int32 ival = 0;
	jsdouble dval = 0;
	//jsuint uival = 0;
	bool bval;

	switch(JS_TypeOfValue(cx, value)) {
		case JSTYPE_VOID: case JSTYPE_NULL:
			if(fwrite(&ival, sizeof(int), 1, fptr) == 1)
				return true;
			break;
		case JSTYPE_STRING:
			str = JS_GetStringBytes(JSVAL_TO_STRING(value));
			if(fwrite(str, sizeof(char), strlen(str), fptr) == strlen(str))
				return true;
			break;
		case JSTYPE_NUMBER:
			if(isBinary) {
				if(JSVAL_IS_DOUBLE(value)) {
					JS_ValueToNumber(cx, value, &dval);
					if(fwrite(&dval, sizeof(jsdouble), 1, fptr) == 1)
						return true;
				} else if(JSVAL_IS_INT(value)) {
					JS_ValueToInt32(cx, value, &ival);
					if(fwrite(&ival, sizeof(int32), 1, fptr) == 1)
						return true;
				}
			} else {
				if(JSVAL_IS_DOUBLE(value)) {
					JS_ValueToNumber(cx, value, &dval);
					// jsdouble will never be a 64-char string, but I'd rather be safe than sorry
					str = new char[64];
					sprintf_s(str, 64, "%.16f", dval);
					len = strlen(str);
					result = fwrite(str, sizeof(char), len, fptr);
					delete[] str;
					if(result == len)
						return true;
				} else if(JSVAL_IS_INT(value)) {
					JS_ValueToInt32(cx, value, &ival);
					str = new char[16];
					_itoa(ival, str, 10);
					len = strlen(str);
					result = fwrite(str, sizeof(char), len, fptr);
					delete[] str;
					if(result == len)
						return true;
				}
			}
			break;
		case JSTYPE_BOOLEAN:
			if(!isBinary) {
				bval = !!JSVAL_TO_BOOLEAN(value);
				str = bval ? "true" : "false";
				if(fwrite(str, sizeof(char), strlen(str), fptr) == strlen(str))
					return true;
			} else {
				bval = !!JSVAL_TO_BOOLEAN(value);
				if(fwrite(&bval, sizeof(bool), 1, fptr) == 1)
					return true;
			}
			break;
/*		case JSTYPE_OBJECT:
			JSObject *arr = JSVAL_TO_OBJECT(value);
			if(JS_IsArrayObject(cx, arr)) {
				JS_GetArrayLength(cx, arr, &uival);
				for(jsuint i = 0; i < uival; i++) {
					jsval val;
					JS_GetElement(cx, arr, i, &val);
					if(!writeValue(fptr, cx, val, isBinary))
						return false;
				}
				return true;
			} else {
				JSString* jsstr = JS_ValueToString(cx, value);
				str = JS_GetStringBytes(jsstr);
				if(fwrite(str, sizeof(char), strlen(str), fptr) == strlen(str))
					return true;
			}
			break;*/
	}
	return false;
}

bool isValidPath(const char* name)
{
	return (!strstr(name, "..\\") && !strstr(name, "../") && (strcspn(name, "\":?*<>|") == strlen(name)));
}
