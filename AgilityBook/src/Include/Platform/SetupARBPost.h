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
 * @brief Precompiled header file for system headers.
 * @author David Connet
 *
 * This must be included after any wx headers (including any system headers
 * before include wx headers causes issues in msvc)
 *
 * Revision History
 * 2015-04-04 Add support for C99 printf formats. (Breaking change)
 * 2014-05-16 Moved HAS macros here.
 * 2013-01-27 Split/moved stdafx.h.
 */

// VC9/VC10 defines _HAS_TR1 if tr1 is present (via yvals.h).
#include <memory>
#if defined(__WXMAC__)
#include <AvailabilityMacros.h>
#endif

// If not present, pick up boost's. Now we can use std::tr1::shared_ptr
#if _MSC_VER < 1700 && (!defined(_HAS_TR1) || !_HAS_TR1)
#if defined(__WXMAC__) && defined(MAC_OS_X_VERSION_10_9) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_9
#define __USE_BOOST	0
#else //mac
#define __USE_BOOST	1
#endif
#else //vc
#define __USE_BOOST	0
#endif

#if __USE_BOOST
// Boost can also be included by tweaking the include path and
// including <memory>:
//  boost-root/boost/tr1/tr1
//  boost-root
//  other standard library replacements (like STLport)
//  your regular standard library
// (see <boost>/doc/html/boost_tr1/usage.html)
// Including memory.hpp is the preferred method over include path order.
#include <boost/version.hpp>
#if BOOST_VERSION < 103800
#error Minimum supported version of Boost: 1.38.0
#endif
#include <boost/tr1/memory.hpp>
#include <boost/tr1/tuple.hpp>
#include <boost/make_shared.hpp>
namespace std
{
	using boost::make_shared;
	using boost::shared_ptr;
	using boost::weak_ptr;
	using boost::dynamic_pointer_cast;
	using tr1::tuple;
	using tr1::get;
};

#else
#include <tuple>

#endif

#include <assert.h>

// For _() usage when used indirectly.
// poedit is configured to also look for these.
#if defined(UNICODE) || defined(_UNICODE)
#define arbT(x)	L##x
#else
#define arbT(x)	x
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400
#define ARRAY_SIZE(buf)	_countof(buf)
#else
#define ARRAY_SIZE(buf) (sizeof(buf) / sizeof(buf[0]))
#endif


// Compiler configuration

// ARB_HAS_32_AND_64_BIT_TIMET
// This define means the compiler has 2 time_ts
// time_t: 32bits
// __time64_t: 64bits
#if _MSC_VER >= 1300 && _MSC_VER < 1400
// VC6: time_t is 32bits
// VC7: time_t is 32bits, and __time64_t is 64
// VC8+: time_t is 64bit
#define ARB_HAS_32_AND_64_BIT_TIMET
#endif

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

// ARB_HAS_SECURE_SPRINTF
//  sprintf_s(char*, size_t, const char*, ...)
#if _MSC_VER >= 1400
#define ARB_HAS_SECURE_SPRINTF
#endif

// ARB_HAS_C99_PRINTF_SPECS
//  %s is narrow, %ls is wide - regardless of UNICODE (C99 breaking change)
//  (previous VC behavior had %s as the "natural" char)
#if _MSC_VER >= 1900
// If legacy mode is specified, honor it.
#ifndef _CRT_STDIO_LEGACY_WIDE_SPECIFIERS 
#define ARB_HAS_C99_PRINTF_SPECS
#endif
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

// ARB_HAS_LAMBDA
//  Supports lambda functions
#if (defined(_MSC_VER) && _MSC_VER >= 1600) || (defined(MAC_OS_X_VERSION_10_9) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_9)
#define ARB_HAS_LAMBDA
#endif

// ARB_HAS_NULLPTR
//  nullptr support (see SetupARBPost.h for setting wx-based platforms)
#if (defined(_MSC_VER) && _MSC_VER >= 1600) || (defined(MAC_OS_X_VERSION_10_9) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_9)
#define ARB_HAS_NULLPTR
#else
#define nullptr NULL
#endif

// Copied from WinUser.h for Win8.1 DPI per-monitor support
#ifdef WIN32
#ifndef WM_DPICHANGED
#define WM_DPICHANGED		0x02E0
#endif
#endif
