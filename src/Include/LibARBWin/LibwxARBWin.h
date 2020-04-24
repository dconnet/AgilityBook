#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @author David Connet
 *
 * Revision History
 * 2019-02-13 Created
 */

#if defined(_DLL) && defined(_WIN32)
#if defined(ARBWIN_EXPORT)
#define ARBWIN_API __declspec(dllexport)
#else
#define ARBWIN_API __declspec(dllimport)
#endif

#else
#define ARBWIN_API

#endif
