#include <cstdlib>
#include <conio.h>
#include <cstdio>

#include "D2BS.h"
#include "Build.h"
#include "Version.h"
#include "VCRev.h"

void version_updateVersionString()
{
	int length;

	// Release build
	if(build_release)
	{
		length = _cprintf(version_releaseFormat, build_version);
		Vars.pszD2bsVersionString = (char*)malloc(length+1);
		_snprintf_s(Vars.pszD2bsVersionString, length+1, length, version_releaseFormat, build_version);
	}
	// Unofficial build
	else if(vcRev_revNumber == -1)
	{
		length = strlen(version_unofficialBuild);
		Vars.pszD2bsVersionString = (char*)malloc(length+1);
		strcpy_s(Vars.pszD2bsVersionString, length+1, version_unofficialBuild);
	}
	// Revly build
	else
	{
		length = _cprintf(version_revlyFormat, vcRev_revNumber);
		Vars.pszD2bsVersionString = (char*)malloc(length+1);
		_snprintf_s(Vars.pszD2bsVersionString, length+1, length, version_revlyFormat, vcRev_revNumber);
	}
}
