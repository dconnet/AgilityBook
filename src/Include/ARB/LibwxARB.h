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
#if defined(ARB_EXPORT)
#define ARB_API __declspec(dllexport)
#else
#define ARB_API __declspec(dllimport)
#endif

#else
#define ARB_API

#endif
