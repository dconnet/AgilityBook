#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Precompiled header file for system headers.
 * @author David Connet
 *
 * This must be included after any wx headers (including any system headers
 * before include wx headers causes issues in msvc)
 *
 * Revision History
 * 2018-10-30 Removed unnecessary macros as we require C++11 mode now.
 * 2015-04-04 Add support for C99 printf formats. (Breaking change)
 * 2014-05-16 Moved HAS macros here.
 * 2013-01-27 Split/moved stdafx.h.
 */

// VC9/VC10 defines _HAS_TR1 if tr1 is present (via yvals.h).
#include <memory>
#if defined(__WXMAC__)
#include <AvailabilityMacros.h>
#endif
#include <cstdint>

#include <tuple>

#include <assert.h>

// For _() usage when used indirectly.
// poedit is configured to also look for these.
#if defined(UNICODE) || defined(_UNICODE)
#define arbT(x)	L##x
#else
#define arbT(x)	x
#endif

#if !defined(_MSC_VER)
#define _countof(buf) (sizeof(buf) / sizeof(buf[0]))
#endif


// Compiler configuration

// ARB_HAS_SECURE_LOCALTIME
//  _localtime64_s(struct tm*, time_t*)
#if _MSC_VER >= 1400
#define ARB_HAS_SECURE_LOCALTIME
#endif

// ARB_HAS_SECURE_MBS_WCS
//  _wcstombs_s(size_t*, char*, size_t, const wchar_t*, size_t)
#if _MSC_VER >= 1400
#define ARB_HAS_SECURE_MBS_WCS
#endif

// ARB_SET_ERASE_RETURNS_ITERATOR
//  std::set<x>::iterator i = var.begin()...; i = var.erase(i); ...
//  If not defined, will use "var.erase(i++);" construct
#if defined(_MSC_VER) || (defined(MAC_OS_X_VERSION_10_9) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_9)
#define ARB_SET_ERASE_RETURNS_ITERATOR
#endif

// ARB_HAS_GETSYSTEMINFO
//  Has GetSystemInfo in order to check for x64.
#if !defined(_MSC_VER) || _MSC_VER >= 1300
#define ARB_HAS_GETSYSTEMINFO
#endif

// ARB_HAS_ISTREAM_WCHAR
//  Supports ifstream(wchar_t*) (MS does, xcode 4.5 doesn't)
#ifdef _MSC_VER
#define ARB_HAS_ISTREAM_WCHAR
#endif

// ARB_HAS_OSTREAM_WCHAR
//  Supports ofstream(wchar_t*) (MS does, xcode 4.5 doesn't)
#ifdef _MSC_VER
#define ARB_HAS_OSTREAM_WCHAR
#endif

// Copied from WinUser.h for Win8.1 DPI per-monitor support
#ifdef WIN32
#ifndef WM_DPICHANGED
#define WM_DPICHANGED		0x02E0
#endif
#endif
