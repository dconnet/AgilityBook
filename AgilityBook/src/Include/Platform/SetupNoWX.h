#ifdef _MSC_VER
#pragma once
#endif

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Precompiled header file when not using wxWidgets.
 * @author David Connet
 *
 * Revision History
 * 2013-01-27 Created
 */

#include "Platform/SetupARBPre.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <time.h>

#include "Platform/SetupARBPost.h"
