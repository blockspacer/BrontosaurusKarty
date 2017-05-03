#pragma once

#include "../CommonUtilities/CommonSTL.h"
#include "../CommonUtilities/CommonCU.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(comptr) if (comptr != nullptr) { comptr->Release(); comptr = nullptr; } (void)0
#endif
