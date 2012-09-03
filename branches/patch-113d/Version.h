#ifndef VERSION_H
#define VERSION_H

const char* const version_releaseFormat = "D2BS %s";
const char* const version_unofficialBuild = "D2BS Unofficial Build";
const char* const version_revlyFormat = "D2BS revly %d";

// Called on load, updates build_versionString
void version_updateVersionString();

#endif