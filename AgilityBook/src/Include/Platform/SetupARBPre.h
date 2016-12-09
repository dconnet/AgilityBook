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
 * @brief Base Precompiled header file for ARB.
 * @author David Connet
 *
 * Standard usage: In framework specific include, include this, set up
 * framework, then include SetupARBPost.h
 *
 * Revision History
 * 2016-09-27 Increase minimun WINVER to 0x502
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
// 1600: VC10.0 http://msdn.microsoft.com/en-us/library/b0084kay%28v=vs.100%29.aspx
// 1700: VC11.0 http://msdn.microsoft.com/en-us/library/b0084kay%28v=vs.110%29.aspx
// 1800: VC12.0 http://msdn.microsoft.com/en-us/library/b0084kay%28v=vs.120%29.aspx
// 1900: VC14.0
//
// https://blogs.msdn.microsoft.com/vcblog/2016/10/05/visual-c-compiler-version/
// > Starting with VS “15” Preview 5, the Visual C++ Team is monotonically updating the value of the built-in preprocessor macro _MSC_VER at every Visual C++ toolset update. 
//
// 1910: VC15.0
// _M_IX86: Defined for x86 (value specifies processor)
// _M_X64: Defined for x64 processors
// _M_IA64: Defined for Itanium processor family

#if _MSC_VER < 1500
	#error pre-VC9 is not supported.
#endif // End VC-elifs

#ifndef VC_EXTRALEAN
	#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Minimum system
// Vista                      _WIN32_WINNT >= 0x0600  WINVER >= 0x0600
// WinServer2003sp1, WinXPsp2 _WIN32_WINNT >= 0x0502  WINVER >= 0x0502
// WinServer2003, WinXP       _WIN32_WINNT >= 0x0501  WINVER >= 0x0501
// Win2K                      _WIN32_WINNT >= 0x0500  WINVER >= 0x0500
// WinNT4.0                   _WIN32_WINNT >= 0x0400  WINVER >= 0x0400
// WinMe                    _WIN32_WINDOWS >= 0x0500  WINVER >= 0x0500
// Win98                    _WIN32_WINDOWS >= 0x0410  WINVER >= 0x0410
// Win95                    _WIN32_WINDOWS >= 0x0400  WINVER >= 0x0400
// IE7.0             _WIN32_IE >= 0x0700
// IE6.0 SP2         _WIN32_IE >= 0x0603
// IE6.0 SP1         _WIN32_IE >= 0x0601
// IE6.0             _WIN32_IE >= 0x0600
// IE5.5             _WIN32_IE >= 0x0550
// IE5.01            _WIN32_IE >= 0x0501
// IE5.0, 5.0a, 5.0b _WIN32_IE >= 0x0500
// IE4.01            _WIN32_IE >= 0x0401
// IE4.0             _WIN32_IE >= 0x0400
// IE3.0, 3.01, 3.02 _WIN32_IE >= 0x0300

#ifndef _WIN32_IE
	#define _WIN32_IE 0x0600	// Minimum IE, 6.0
#endif

#if _WIN32_IE < _WIN32_IE_IE55
	#error pre-IE5.5 is not supported.
#endif

// Unless overridden, target XP by default.
#ifndef WINVER
	#if _MSC_VER >= 1700
		#define WINVER	0x0600
	#elif defined(_M_IA64)
		#define WINVER	0x0502
	#elif defined(_M_X64)
		#define WINVER	0x0502
	#else //x86
		#define WINVER	0x0502
	#endif
#endif

#ifndef _WIN32_WINNT
	#define _WIN32_WINNT	WINVER
#endif

#if WINVER != _WIN32_WINNT
	#error WINVER is different than _WIN32_WINNT
#endif

// Win9x not supported
#ifdef _WIN32_WINDOWS
	#undef _WIN32_WINDOWS
#endif

// Error checking
// VC10: Minimum system: x86 - XP SP2, x64 - XP, Itanium - Server 2003 SP1
// VC11: Minimum system: Vista (Can target XP using platform toolset 'v110_xp')
#if _MSC_VER >= 1800
	#if WINVER < 0x0600
		#error VC12 minimum version is 0x0600
	#endif
	// Note: VC12 now has true variadic macros.
#elif _MSC_VER >= 1700
	#if WINVER < 0x0600
		#error VC11 minimum version is 0x0600
	#endif
	// VC11 changed how make_shared works. Default is 5. I need 6.
	#define _VARIADIC_MAX 6
#elif _MSC_VER >= 1600
	#if defined(_M_IA64) && WINVER < 0x0502
		#error Itanium minimum version is 0x0502
	#elif defined(_M_X64) && WINVER < 0x0502
		#error x64 minimum version is 0x0502
	#elif WINVER < 0x0502
		#error VC10 minimum version is 0x0502
	#endif
#else
	#error Go away. Only VC10+ supported.
#endif

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
#define STRING(x) STRING2(x)
//#pragma message ( __FILE__ "(" STRING(__LINE__) ") : message" )
#define FILE_LINE __FILE__ "(" STRING(__LINE__) ") : "
#define PRAGMA_MESSAGE(x) message( FILE_LINE #x )
#define PRAGMA_TODO(x) message( FILE_LINE "TODO: " #x )
#define PRAGMA_FIXME(x) message( FILE_LINE "FIXME: " #x )

#define DECLARE_NO_COPY_IMPLEMENTED(cls) \
	private: \
		cls(cls const&); \
		cls& operator=(cls const&);


// Compiler configuration (moved to SetupARBPost.h)
