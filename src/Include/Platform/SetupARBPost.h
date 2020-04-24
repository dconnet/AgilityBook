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
#define arbT(x) L##x
#else
#define arbT(x) x
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
#define WM_DPICHANGED 0x02E0
#endif
#endif

// When turning on VS2017's static analysis (All Rules!), there's a few warnings
// we're just not going to deal with at this time.
#ifdef _MSC_VER

// clang-format off
#pragma warning (disable : 26415) // Smart pointer parameter '<var>' is used only to access contained pointer. Use T* or T& instead (r.30).
#pragma warning (disable : 26418) // Shared pointer parameter '<var>' is not copied or moved. Use T* or T& instead (r.36).
#pragma warning (disable : 26426) // Global initializer calls a non-constexpr function '<val>' (i.22).
#pragma warning (disable : 26429) // Symbol '<var>' is never tested for nullness, it can be marked as not_null (f.23).
#pragma warning (disable : 26434) // Function 'ARBVector<std::shared_ptr<class> >::ARBVector<std::shared_ptr<class> >' hides a non - virtual function 'ARBVectorNoSave<std::shared_ptr<class> >::ARBVectorNoSave<std::shared_ptr<class> >' (c.128).
#pragma warning (disable : 26439) // This kind of function may not throw. Declare it 'noexcept' (f.6).
#pragma warning (disable : 26440) // Function '<func>' can be declared 'noexcept' (f.6).
#pragma warning (disable : 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning (disable : 26447) // The function is declared 'noexcept' but calls function '<func>()' which may throw exceptions(f.6).
#pragma warning (disable : 26451) // Arithmetic overflow: Using operator '+' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator '+' to avoid overflow (io.2).
#pragma warning (disable : 26455) // Default constructor may not throw. Declare it 'noexcept' (f.6).
#pragma warning (disable : 26456) // Operator '<class>::operator=' hides a non - virtual operator '<baseclass>::operator=' (c.128).
#pragma warning (disable : 26460) // The reference argument '<var>' for function '<var>' can be marked as const (con.3).
#pragma warning (disable : 26462) // The value pointed to by '<var>' is assigned only once, mark it as a pointer to const (con.4).
#pragma warning (disable : 26472) // Don't use a static_cast for arithmetic conversions. Use brace initialization, gsl::narrow_cast or gsl::narrow (type.1).
#pragma warning (disable : 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).
#pragma warning (disable : 26482) // Only index into arrays using constant expressions (bounds.2).
#pragma warning (disable : 26485) // Expression '<var>': No array to pointer decay (bounds.3).
#pragma warning (disable : 26486) // Don't pass a pointer that may be invalid to a function. ... (lifetime.1).
#pragma warning (disable : 26489) // Don't dereference a pointer that may be invalid: '<var>'. '<var>' may have been invalidated at line <line> (lifetime.1).
#pragma warning (disable : 26496) // The variable '<var>' is assigned only once, mark it as const (con.4).
// clang-format on

#endif
