#pragma once

#if defined(EXPORTING)
#define EXPORT __declspec(dllexport)
#elif defined(EXPORTED)
#define EXPORT __declspec(dllimport)
#endif
