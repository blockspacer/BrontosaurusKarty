// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#ifdef _WIN32
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr

#include "CommonSTL.h"
#include "CommonCU.h"

