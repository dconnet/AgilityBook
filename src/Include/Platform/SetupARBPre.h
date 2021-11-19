#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Base Precompiled header file for ARB.
 * @author David Connet
 *
 * Standard usage: In framework specific include, include this, set up
 * framework, then include SetupARBPost.h
 *
 * Revision History
 * 2021-11-19 Added C4582 suppression in order to update to fmtv8.
 * 2020-04-09 Min WINVER check was wrong, should be 0x601.
 * 2019-01-27 Min C++ is now 14 (for make_unique)
 * 2018-10-06 Dropping support for pre VS2017 (and XP).
 * 2016-09-27 Increase minimum WINVER to 0x502
 * 2014-05-16 Updated to reflect non-support for VC9, moved HAS macros to Post.
 * 2013-01-27 Split/moved stdafx.h.
 * 2013-01-01 Added _VARIADIC_MAX for vc11.
 * 2012-01-29 Add macro to ease selecting all text on dialog init.
 * 2011-12-22 Add a macro to make using Bind() easier.
 * 2009-05-30 Tweaked pragma message macro.
 * 2009-03-09 Added ARB_SET_ERASE_RETURNS_ITERATOR.
 * 2005-10-30 Added static xerces library support.
 */

#if defined(_WIN32)

// _MSC_VER
// http://support.microsoft.com/kb/65472 (define values thru vc6)
//  600: C Compiler version 6.0
//  700: C/C++ compiler version 7.0
//  800: Visual C++, Windows, version 1.0
//  800: Visual C++, 32-bit, version 1.0
//  900: Visual C++, Windows, version 2.0
//  900: Visual C++, 32-bit, version 2.x
// 1000: Visual C++, 32-bit, version 4.0
// 1100: Visual C++, 32-bit, version 5.0
// 1200: Visual C++, 32-bit, version 6.0
// 1300: VC7.0
// 1310: VC7.1 http://msdn2.microsoft.com/en-us/library/b0084kay(VS.71).aspx
// 1400: VC8.0 http://msdn.microsoft.com/en-us/library/b0084kay%28v=vs.80%29.aspx
// 1500: VC9.0 http://msdn.microsoft.com/en-us/library/b0084kay%28v=vs.90%29.aspx
//       VC9.0 _MSC_FULL_VER == 150030729 (Visual Studio 2008, SP1)
// 1600: VC10.0 http://msdn.microsoft.com/en-us/library/b0084kay%28v=vs.100%29.aspx
// 1700: VC11.0 http://msdn.microsoft.com/en-us/library/b0084kay%28v=vs.110%29.aspx
// 1800: VC12.0 http://msdn.microsoft.com/en-us/library/b0084kay%28v=vs.120%29.aspx
// 1900: VC14.0 Visual Studio 2015
//
// https://blogs.msdn.microsoft.com/vcblog/2016/10/05/visual-c-compiler-version/
// > Starting with VS "15" Preview 5, the Visual C++ Team is monotonically updating the value of the built-in
// preprocessor macro _MSC_VER at every Visual C++ toolset update.
//
// 1910: VC14.1 Visual Studio 2017
// 1911: VC14.11 Visual Studio 2017 version 15.3
// 1912: VC14.12 Visual Studio 2017 version 15.5
// 1913: VC14.13 Visual Studio 2017 version 15.6
// 1914: VC14.14 Visual Studio 2017 version 15.7
// 1915: VC14.15 Visual Studio 2017 version 15.8
// 1920: VC14.2 Visual Studio 2019 version 16.0
// 1921: VC14.2 Visual Studio 2019 version 16.1
// 1922: VC14.2 Visual Studio 2019 version 16.2
// 1923: VC14.2 Visual Studio 2019 version 16.3
// 1924: VC14.2 Visual Studio 2019 version 16.4
//
// _M_IX86: Defined for x86 (value specifies processor)
// _M_X64: Defined for x64 processors
// _M_IA64: Defined for Itanium processor family

#if _MSC_VER < 1910
#error pre-VC141 is not supported.
#endif // End VC-elifs

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers
#endif

// Minimum system
// Win10                      WINVER >= 0x0A00 _WIN32_WINNT_WIN10
// Win8.1                     WINVER >= 0x0603 _WIN32_WINNT_WINBLUE
// Win8                       WINVER >= 0x0602 _WIN32_WINNT_WIN8
// Win7                       WINVER >= 0x0601 _WIN32_WINNT_WIN7
// Vista, WinServer2008       WINVER >= 0x0600 _WIN32_WINNT_VISTA/_WS08/_LONGHORN
// WinServer2003sp1, WinXPsp2 WINVER >= 0x0502 _WIN32_WINNT_WS03
// WinServer2003, WinXP       WINVER >= 0x0501 _WIN32_WINNT_WINXP

// Unless overridden, target Win7 by default.
#ifndef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WIN7
#define _WIN32_WINNT  0x601
#endif
#include <sdkddkver.h>

// Error checking (no support for pre-Win7)
#if WINVER < 0x0601
#error Minimum supported OS is Windows 7.
#endif

// Disable some warnings from /Wall
// clang-format off
// 4365: signed/unsigned mismatch
// 4623: default constructor was implicitly defined as deleted
// 4625: copy constructor was implicitly defined as deleted
// 4626: assignment operator was implicitly defined as deleted
// 5026: move constructor was implicitly defined as deleted
// 5027: move assignment operator was implicitly defined as deleted
#pragma warning(disable : 4365 4623 4625 4626 5026 5027) // Needed to use fmt

#pragma warning(disable : 4266) // no override available for virtual member function; function is hidden
#pragma warning(disable : 4355) // 'this': used in base member initializer list
#pragma warning(disable : 4371) // layout of class may have changed from a previous version of the compiler due to better packing of member '<name>'
#pragma warning(disable : 4464) // relative include path contains '..'
#pragma warning(disable : 4514) // unreferenced inline function has been removed
#pragma warning(disable : 4582) // '<name>': constructor is not implicitly called
#pragma warning(disable : 4668) // '<name>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#pragma warning(disable : 4710) // function not inlined
#pragma warning(disable : 4711) // function '<name>' selected for automatic inline expansion
#pragma warning(disable : 4774) // 'string' : format string expected in argument number is not a string literal
#pragma warning(disable : 4820) // '<n>' bytes padding added after data member '<name>'
#pragma warning(disable : 5031) // likely mismatch, popping warning state pushed in different file
#pragma warning(disable : 5032) // detected #pragma warning(push) with no corresponding #pragma warning(pop)
#pragma warning(disable : 5039) // pointer or reference to potentially throwing function passed to 'extern "C"' function under -EHc.
#pragma warning(disable : 5045) // Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
#pragma warning(disable : 5204) // class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly

// x86 release builds trip on Catch
// Just defining this before the Catch headers isn't good enough
#if defined(_M_IX86) && defined(NDEBUG)
#pragma warning(disable : 4738) // storing 32-bit float result in memory, possible loss of performance
#endif

// clang-format on

#else // _WIN32

// Any non-windows system checking should go here.

// xcode7 issues

#if defined(__clang__) && __clang_major__ >= 7
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#pragma clang diagnostic ignored "-Wpotentially-evaluated-expression"
#endif

#endif // _WIN32

// These are needed in order to generate a decent pragma message
#define STRING2(x) #x
#define STRING(x)  STRING2(x)
//#pragma message ( __FILE__ "(" STRING(__LINE__) ") : message" )
#define FILE_LINE         __FILE__ "(" STRING(__LINE__) ") : "
#define PRAGMA_MESSAGE(x) message(FILE_LINE #x)
#define PRAGMA_TODO(x)    message(FILE_LINE "TODO: " #x)
#define PRAGMA_FIXME(x)   message(FILE_LINE "FIXME: " #x)


// MSVC Note: you must pass "/Zc:__cplusplus" in VS2017 so the macro gets set
// to the proper value. Without that (as of 15.8.8), it is set to 199711L.
// As of 15.8.8, MS defines __cplusplus (when /Z option is used):
// Defaults to C++14 201402L
// /std:c++14        201402L
// /std:c++17        201703L
// /std:c++latest    201704L
// Without /Z option (or with it disabled):
//                   199711L

#if !defined(__cplusplus) || __cplusplus < 201402
#pragma message(FILE_LINE "ERROR: __cplusplus is defined as " STRING(__cplusplus))
#error Compiler must support C++14
#endif


#define DECLARE_NO_ASSIGN_IMPLEMENTED(cls) \
private: \
	cls& operator=(cls const&) = delete; \
	cls& operator=(cls&&) = delete;
#define DECLARE_NO_COPY_IMPLEMENTED(cls) \
private: \
	cls(cls const&) = delete; \
	cls(cls&&) = delete; \
	cls& operator=(cls const&) = delete; \
	cls& operator=(cls&&) = delete;


// Compiler configuration (moved to SetupARBPost.h)
