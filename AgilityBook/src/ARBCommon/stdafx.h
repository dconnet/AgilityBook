#ifdef _MSC_VER
#pragma once
#endif

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

#ifdef USE_WXWIDGETS
#include "Platform/SetupWX.h"
#else
#include "Platform/SetupNoWX.h"
#endif
