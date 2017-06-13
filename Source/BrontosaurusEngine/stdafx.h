// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "../CommonUtilities/CommonSTL.h"
#include "../CommonUtilities/CommonCU.h"

#include "../CommonUtilities/MemoryLeekLogger.h"
#include "../CommonUtilities/TimerManager.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(ptr) delete[] ptr; ptr = nullptr
#endif // !SAFE_DELETE_ARRAY

#define self (*this)

#define SAFE_RELEASE(comptr) if (comptr != nullptr) { comptr->Release(); comptr = nullptr; }
#define SAFE_ADD_REF(com_ptr) if (com_ptr) { com_ptr->AddRef(); }

#ifndef SUPRESS_UNUSED_WARNING
#define SUPRESS_UNUSED_WARNING(variable) variable
#endif // !SUPRESS_UNUSED_WARNING

#include "Engine.h"
#include "DXFramework.h"

