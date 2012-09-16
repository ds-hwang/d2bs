#include "JSProfile.h"
#include "Profile.h"

#include "D2BS.h"

// Profile() - get the active profile
// Profile(name) - get the named profile
//
// Create profiles:
//	Profile(ProfileType.singlePlayer, charname, diff)
//	Profile(ProfileType.battleNet, account, pass, charname, gateway)
//	Profile(ProfileType.openBattleNet, account, pass, charname, gateway)
//	Profile(ProfileType.tcpIpHost, charname, diff)
//	Profile(ProfileType.tcpIpJoin, charname, ip)
CLASS_CTOR(profile)
{
	Profile* prof;
	ProfileType pt;
	char* str1;
	char* str2;
	char* str3;
	char* str4;
	unsigned int i;

	str1 = str2 = str3 = str4 = NULL;
	pt = PROFILETYPE_INVALID;

	try
	{
		// Profile()
		if(argc == 0)
		{
			if(Vars.szProfile != NULL)
				prof = new Profile();
			else
				THROW_ERROR(cx, "No active profile!");
		}
		// Profile(name) - get the named profile
		else if(argc == 1 && JSVAL_IS_STRING(argv[0]))
		{
			JS_ConvertArguments(cx, argc, argv, "s", &str1);
			prof = new Profile(str1);
		}
		// Profile(ProfileType.singlePlayer, charname, diff)
		else if(argc == 3 && JSVAL_IS_INT(argv[0]) &&
			JSVAL_TO_INT(argv[0]) == PROFILETYPE_SINGLEPLAYER)
		{
			JS_ConvertArguments(cx, argc, argv, "isu", &pt, &str1, &i);
			prof = new Profile(PROFILETYPE_SINGLEPLAYER, str1, (char)i);
		}
		// Profile(ProfileType.battleNet, account, pass, charname, gateway)
		else if(argc == 5 && JSVAL_IS_INT(argv[0]) &&
			JSVAL_TO_INT(argv[0]) == PROFILETYPE_BATTLENET)
		{
			JS_ConvertArguments(cx, argc, argv, "issss", &pt, &str1, &str2, &str3, &str4);
			prof = new Profile(PROFILETYPE_BATTLENET, str1, str2, str3, str4);
		}
		// Profile(ProfileType.openBattleNet, account, pass, charname, gateway)
		else if(argc == 5 && JSVAL_IS_INT(argv[0]) &&
			JSVAL_TO_INT(argv[0]) == PROFILETYPE_OPEN_BATTLENET)
		{
			JS_ConvertArguments(cx, argc, argv, "issss", &pt, &str1, &str2, &str3, &str4);
			prof = new Profile(PROFILETYPE_OPEN_BATTLENET, str1, str2, str3, str4);
		}
		// Profile(ProfileType.tcpIpHost, charname, diff)
		else if(argc == 3 && JSVAL_IS_INT(argv[0]) &&
			JSVAL_TO_INT(argv[0]) == PROFILETYPE_TCPIP_HOST)
		{
			JS_ConvertArguments(cx, argc, argv, "isu", &pt, &str1, &i);
			prof = new Profile(PROFILETYPE_TCPIP_HOST, str1, (char)i);
		}
		// Profile(ProfileType.tcpIpJoin, charname, ip)
		else if(argc == 3 && JSVAL_IS_INT(argv[0]) &&
			JSVAL_TO_INT(argv[0]) == PROFILETYPE_TCPIP_JOIN)
		{
			JS_ConvertArguments(cx, argc, argv, "iss", &pt, &str1, &str2);
			prof = new Profile(PROFILETYPE_TCPIP_JOIN, str1, str2);
		}
		else
			THROW_ERROR(cx, "Invalid parameters.");
	}
	catch(char* ex)
	{
		THROW_ERROR(cx, ex);
	}

	JS_SetPrivate(cx, obj, prof);

	return JS_TRUE;
}

JSAPI_FUNC(profile_login)
{
	char* error;
	Profile* prof;

	prof = (Profile*)JS_GetPrivate(cx, obj);

	if(prof->login(&error) != 0)
		THROW_ERROR(cx, error);

	return JS_TRUE;
}

void profile_finalize(JSContext *cx, JSObject *obj)
{
	Profile* prof;

	prof = (Profile*)JS_GetPrivate(cx, obj);

	if(prof != NULL)
		delete prof;

	JS_SetPrivate(cx, obj, NULL);
}

JSAPI_PROP(profile_getProperty)
{
	Profile* prof;

	prof = (Profile*)JS_GetPrivate(cx, obj);

	switch(JSVAL_TO_INT(id))
	{
		case PROFILE_TYPE:
			*vp = INT_TO_JSVAL(prof->type);
			break;
		case PROFILE_IP:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, prof->ip));
			break;
		case PROFILE_USERNAME:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, prof->username));
			break;
		case PROFILE_GATEWAY:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, prof->gateway));
			break;
		case PROFILE_CHARACTER:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, prof->charname));
			break;
		case PROFILE_DIFFICULTY:
			*vp = INT_TO_JSVAL(prof->diff);
			break;
		case PROFILE_MAXLOGINTIME:
			*vp = INT_TO_JSVAL(prof->maxLoginTime);
			break;
		case PROFILE_MAXCHARSELTIME:
			*vp = INT_TO_JSVAL(prof->maxCharTime);
			break;
	}

	return JS_TRUE;
}

EMPTY_CTOR(profileType);

JSAPI_PROP(profileType_getProperty)
{
	*vp = id;

	return JS_TRUE;
}
