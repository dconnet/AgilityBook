#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
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
 * @li 2005-10-30 DRC Added static xerces library support.
 */

#ifdef _WINDOWS

// _MSCVER
// 1200: VC6.0
// 1300: VC7.0
// 1310: VC7.1
// 1400: VC8.0
// 1500: VC9.0
// _M_IX86: Defined for x86 (value specifies processor)
// _M_X64: Defined for x64 processors
// _M_IA64: Defined for Itanium processor family

// VC6
#if _MSC_VER < 1300
	#if defined(UNICODE) || defined(_UNICODE)
		#error In order to compile with UNICODE in VC6, the code needs a lot of work!
	#endif
	// Turn off some warnings in vc6.
	#pragma warning ( disable : 4786 )	// identifier was truncated to '255' characters in the debug information
	//#pragma warning( disable : 4503 )

// VC7
#elif _MSC_VER >= 1300 && _MSC_VER < 1400
	#pragma warning ( disable : 4100 )	// 'identifier' : unreferenced formal parameter
	#pragma warning ( disable : 4702 )	// unreachable code (generated during link from STL code)

// VC8
#elif _MSC_VER >= 1400 && _MSC_VER < 1500
	#pragma warning ( disable : 4100 )	// 'identifier' : unreferenced formal parameter

// VC9
#elif _MSC_VER >= 1500
	#pragma warning ( disable : 4100 )	// 'identifier' : unreferenced formal parameter

#endif // End VC-elifs

#ifndef VC_EXTRALEAN
	#define VC_EXTRALEAN		/// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.

// Minimum system
// Vista              _WIN32_WINNT >= 0x0600  WINVER >= 0x0600
// Server 2003 family _WIN32_WINNT >= 0x0502  WINVER >= 0x0502
// WinXP              _WIN32_WINNT >= 0x0501  WINVER >= 0x0501
// Win2K              _WIN32_WINNT >= 0x0500  WINVER >= 0x0500
// WinNT4.0           _WIN32_WINNT >= 0x0400  WINVER >= 0x0400
// WinMe            _WIN32_WINDOWS >= 0x0500  WINVER >= 0x0500
// Win98            _WIN32_WINDOWS >= 0x0410  WINVER >= 0x0410
// Win95            _WIN32_WINDOWS >= 0x0400  WINVER >= 0x0400
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
	#define _WIN32_IE 0x0500	/// Minimum IE, 5.0
#endif

#ifndef WINVER
	#if defined(_M_IA64)
		#define WINVER	0x0502
	#elif defined(_M_X64)
		#define WINVER	0x0501
	#else //x86
		#if _MSC_VER >= 1500
			#define WINVER	0x0500
		#else
			#define WINVER	0x0400
		#endif
	#endif
#endif

#ifndef _WIN32_WINNT
	#define _WIN32_WINNT	WINVER
#endif

#if WINVER != _WIN32_WINNT
	#error WINVER is different than _WIN32_WINNT
#endif

// Note VC6 hard-codes this define to ME. So we'll follow suit...
// (unset if VC9+)
#ifndef _WIN32_WINDOWS
	#if _MSC_VER < 1500 && !defined(UNICODE)
		#define _WIN32_WINDOWS	0x0500
	#else
		#undef _WIN32_WINDOWS
	#endif
#endif

// Error checking
// Minimum system: x86 - Win2000, x64 - XP, Itanium - Server 2003
#if _MSC_VER >= 1500 && WINVER < 0x0500
	#error VC9 minimum version is 0x0500
#elif defined(_M_IA64) && WINVER < 0x0502
	#error Itanium minimum version is 0x0502
#elif defined(_M_X64) && WINVER < 0x0501
	#error Itanium minimum version is 0x0501
#endif

#if defined(_WIN32_WINDOWS) && defined(UNICODE)
	#error We do not support UNICODE builds for Win98
#endif

// This is new as of vc7
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	/// some CString constructors will be explicit
#define _AFX_ALL_WARNINGS	/// turns off MFC's hiding of some common and often safely ignored warning messages

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxdlgs.h>

// Some minor tweaks to allow VC6 support while using new features.
#if _MSC_VER < 1300
#define CStringA CString
typedef DWORD DWORD_PTR;
typedef long LONG_PTR;
#define _tstol _ttol
#endif

#else // _WINDOWS

// Include other platform common files here. This way we can continue to
// include 'stdafx.h' as the first header in all .cpp files so win32
// pre-compiled headers work properly.

// These are some examples of some things that MUST be defined in order to
// compile ARB. I am making use of Window's tchar.h mappings.
// Obviously, much more work is required for the 'Win' directory!
#ifdef UNICODE
typedef wchar_t	TCHAR;
#define _T(x)	L##x
#define _tstol	atol
#define _tcstod	strtod

#else
typedef char	TCHAR;
#define _T(x)	x
#define _tstol	atol
#define _tcstod	strtod
#endif // UNICODE

#endif // _WINDOWS
