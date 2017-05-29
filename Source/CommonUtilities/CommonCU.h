#pragma once

#include "GrowingArray.h"
#include "VectorOnStack.h"
#include "StaticArray.h"

#include "StringHelper.h"
#include "DL_Debug.h"

#include "cuvector.h"
#include "matrix.h"

#include "EMouseButtons.h"
#include "CommandLineManager.h"

#include "CUTime.h"
#include "TimerManager.h"


#define RANGE(VALUE, LOW, HIGH) ((VALUE) < (LOW)) ? (LOW) : ((VALUE) > (HIGH)) ? (HIGH) : (VALUE)
#define CLAMP(VALUE, A_MIN, A_MAX) RANGE((VALUE), (A_MIN), (A_MAX))
