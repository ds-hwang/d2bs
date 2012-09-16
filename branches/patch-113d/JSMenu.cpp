#include "JSMenu.h"
//#include "Control.h"
#include "JSControl.h"
#include "Constants.h"
#include "Helpers.h"
#include "D2BS.h"

#include "Profile.h"

JSAPI_FUNC(my_login)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	char *profile = NULL;
	
	bool copiedProfile = false;
	char* error;

	Profile* prof;

	if(!JSVAL_IS_STRING(argv[0]))
	{
		if(Vars.szProfile != NULL)
		{
			int size = strlen(Vars.szProfile)+1;
			profile = new char[size];
			strcpy_s(profile, size, Vars.szProfile);
			copiedProfile = true;
		}
		else
			THROW_ERROR(cx, "Invalid profile specified!");
	} else {
		profile = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
		strcpy_s(Vars.szProfile, 256, profile);
	}

	if(!profile) THROW_ERROR(cx, "Could not get profile!");
	if(!Profile::ProfileExists(profile)) THROW_ERROR(cx, "Profile does not exist!");

	prof = new Profile(profile);
	if(copiedProfile)
		delete[] profile;

	if(prof->login(&error) != 0)
		THROW_ERROR(cx, error);

	return JS_TRUE;
}

JSAPI_FUNC(my_selectChar)
{
	if(argc != 1 || !JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, "Invalid parameters specified to selectCharacter");

	char* profile = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if(!Profile::ProfileExists(profile))
		THROW_ERROR(cx, "Invalid profile specified");
	char charname[24], file[_MAX_FNAME+MAX_PATH];
	sprintf_s(file, sizeof(file), "%sd2bs.ini", Vars.szPath);
	GetPrivateProfileString(profile, "character", "ERROR", charname, sizeof(charname), file);

	*rval = JSVAL_TO_BOOLEAN(OOG_SelectCharacter(charname));
	return JS_TRUE;
}

JSAPI_FUNC(my_createGame)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	char *name = NULL, *pass = NULL;
	int32 diff = 3;
	if(!JS_ConvertArguments(cx, argc, argv, "s/si", &name, &pass, &diff))
	{
		JS_ReportError(cx, "Invalid arguments specified to createGame");
		return JS_FALSE;
	}
	if(!pass)
		pass = "";

	if(strlen(name) > 15 || strlen(pass) > 15)
		THROW_ERROR(cx, "Invalid game name or password length");

	if(!OOG_CreateGame(name, pass, diff))
		THROW_ERROR(cx, "createGame failed");

	return JS_TRUE;
}

JSAPI_FUNC(my_joinGame)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	char *name = NULL, *pass = NULL;
	if(!JS_ConvertArguments(cx, argc, argv, "s/s", &name, &pass))
	{
		JS_ReportError(cx, "Invalid arguments specified to createGame");
		return JS_FALSE;
	}
	if(!pass)
		pass = "";

	if(strlen(name) > 15 || strlen(pass) > 15)
		THROW_ERROR(cx, "Invalid game name or password length");

	if(!OOG_JoinGame(name, pass))
		THROW_ERROR(cx, "joinGame failed");

	return JS_TRUE;
}

JSAPI_FUNC(my_addProfile)
{
	// validate the args...
	char *profile, *mode, *gateway, *username, *password, *charname;
	profile = mode = gateway = username = password = charname = NULL;
	int spdifficulty = 3;
	if(argc < 6 || argc > 7)
		THROW_ERROR(cx, "Invalid arguments passed to addProfile");

	char** args[] = {&profile, &mode, &gateway, &username, &password, &charname};
	for(uintN i = 0; i < 6; i++)
	{
		if(!JSVAL_IS_STRING(argv[i]))
		{
			THROW_ERROR(cx, "Invalid argument passed to addProfile");
		}
		else
			*args[i] = JS_GetStringBytes(JSVAL_TO_STRING(argv[i]));
	}

	for(int i = 0; i < 6; i++)
	{
		if(!(*args[i]))
			THROW_ERROR(cx, "Failed to convert string");
	}

	if(argc == 7)
		spdifficulty = JSVAL_TO_INT(argv[6]);

	if(spdifficulty > 3 || spdifficulty < 0)
		THROW_ERROR(cx, "Invalid argument passed to addProfile");

	char file[_MAX_FNAME+_MAX_PATH];

	sprintf_s(file, sizeof(file), "%sd2bs.ini", Vars.szPath);
	if(!Profile::ProfileExists(*args[0]))
	{
		char settings[600] = "";
		sprintf_s(settings, sizeof(settings),
					"mode=%s\tgateway=%s\tusername=%s\tpassword=%s\tcharacter=%s\tspdifficulty=%d\t",
					mode, gateway, username, password, charname, spdifficulty);

		StringReplace(settings, '\t', '\0', 600);
		WritePrivateProfileSection(*args[0], settings, file);
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_getOOGLocation)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	*rval = INT_TO_JSVAL(OOG_GetLocation());

	return JS_TRUE;
}

JSAPI_FUNC(my_createCharacter)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	char* name = NULL;
	int32 type = -1;
	JSBool hc = JS_FALSE, ladder = JS_FALSE;
	if(!JS_ConvertArguments(cx, argc, argv, "si/bb", &name, &type, &hc, &ladder))
		return JS_FALSE;

	*rval = BOOLEAN_TO_JSVAL(!!OOG_CreateCharacter(name, type, !!hc, !!ladder));
	return JS_TRUE;
}