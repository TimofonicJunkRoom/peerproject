//
// stdafx.h : include file for standard system include files,
// or project specific files that are used frequently, but changed infrequently.

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "TargetVer.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#include "Resource.h"

#include <atlbase.h>
#include <atlcom.h>
#include <atlsafe.h>
#include <ExDisp.h>

using namespace ATL;