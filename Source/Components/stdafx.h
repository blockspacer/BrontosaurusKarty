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

#include "GameObject.h"
#include "ComponentMessage.h"
#include "ComponentAnswer.h"
#include "ComponentManager.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr
#endif // !SAFE_DELETE

// TODO: reference additional headers your program requires here
