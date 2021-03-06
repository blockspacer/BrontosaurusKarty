// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers


#include "../CommonUtilities/CommonSTL.h"
#include "../CommonUtilities/CommonCU.h"

#include "../BrontosaurusEngine/Scene.h"
#include "../Game/PlayState.h"

#include "KevinLoader/KevinLoader.h"
#include "ComponentManager.h"

#include "LoadManager.h"

#include "../CommonUtilities/MemoryLeekLogger.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr
#endif // !SAFE_DELETE

//#include "../CommonUtilities/MemoryLeekLogger.h"
