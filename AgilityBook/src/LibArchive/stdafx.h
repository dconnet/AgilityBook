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
 *
 * @brief Precompiled header file.
 * @author David Connet
 *
 * Include file for standard system include files, or project specific
 * include files that are used frequently, but are changed infrequently
 *
 * Revision History
 * @li 2013-01-27 DRC Split/moved stdafx.h.
 */

#ifdef USE_WXWIDGETS
#include "Platform/SetupWX.h"
#else
#include "Platform/SetupNoWX.h"
#endif
