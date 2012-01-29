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
 * @li 2011-12-22 DRC Add a macro to make using Bind() easier.
 * @li 2009-05-30 DRC Tweaked pragma message macro.
 * @li 2009-03-09 DRC Added ARB_SET_ERASE_RETURNS_ITERATOR.
 * @li 2005-10-30 DRC Added static xerces library support.
 */


// These are needed in order to generate a decent pragma message
#define STRING2(x) #x
#define STRING(x) STRING2(x)
//#pragma message ( __FILE__ "(" STRING(__LINE__) ") : message" )
#define FILE_LINE __FILE__ "(" STRING(__LINE__) ") : "
#define PRAGMA_MESSAGE(x) message( FILE_LINE #x )
#define PRAGMA_TODO(x) message( FILE_LINE "TODO: " #x )
#define PRAGMA_FIXME(x) message( FILE_LINE "FIXME: " #x )


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
// 1400: VC8.0 http://msdn2.microsoft.com/en-us/library/b0084kay(VS.80).aspx
// 1500: VC9.0 http://msdn2.microsoft.com/en-us/library/b0084kay.aspx
// _M_IX86: Defined for x86 (value specifies processor)
// _M_X64: Defined for x64 processors
// _M_IA64: Defined for Itanium processor family

// VC6
#if _MSC_VER < 1300
	#error VC6 is not supported.

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
	#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the
// ones specified below. Refer to MSDN for the latest info on corresponding
// values for different platforms.

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
	#define _WIN32_IE 0x0500	// Minimum IE, 5.0
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

#else // _WIN32

// Any non-windows system checking should go here.

#endif // _WIN32


// Pre-compiled header setup
#include <wx/wxprec.h>
#ifdef __BORLANDC__
#   pragma hdrstop
#endif
// Compiler doesn't support precompiled headers,
// pull in the headers that wxprec includes.
#ifndef WX_PRECOMP
#include <wx/defs.h>
#include <wx/wxchar.h>
#include <wx/wx.h>
#endif

// ARB was developed against v2.8.10 - anything earlier is not supported.
// And specifically exclude 2.9.0-2.9.2 (.3 had api changes/fixes)
#include <wx/version.h>
#if wxMAJOR_VERSION == 2 && wxMINOR_VERSION == 9 && wxRELEASE_NUMBER < 3
#error pre-v2.9.3 not supported
#endif
#if !wxCHECK_VERSION(2, 8, 10)
#error Unsupported wxWidget version
#endif
// Some sanity checking
#ifndef wxUSE_WCHAR_T
#error ARB: wxUSE_WCHAR_T must be defined in wxWidgets (include/wx/<platform>/setup.)
#endif
#ifndef wxUSE_GUI
#error ARB: wxUSE_GUI must be defined in wxWidgets (include/wx/<platform>/setup.)
#endif
#ifndef wxUSE_DATETIME
#error ARB: wxUSE_DATETIME must be defined in wxWidgets (include/wx/<platform>/setup.)
#endif
#ifndef wxUSE_DATEPICKCTRL
#error ARB: wxUSE_DATEPICKCTRL must be defined in wxWidgets (include/wx/<platform>/setup.)
#endif
#ifndef wxUSE_NOTEBOOK
#error ARB: wxUSE_NOTEBOOK must be defined in wxWidgets (include/wx/<platform>/setup.)
#endif
#ifndef wxUSE_DOC_VIEW_ARCHITECTURE
#error ARB: wxUSE_DOC_VIEW_ARCHITECTURE must be defined in wxWidgets
#endif
// We need unix98-style positional parameters
#if !defined(wxUSE_PRINTF_POS_PARAMS) || !wxUSE_PRINTF_POS_PARAMS
#error ARB: wxUSE_PRINTF_POS_PARAMS must be defined in wxWidgets
#endif
// There's other things that could be checked, but that's good enough for now.
// If there's other things we need, the compilation will die at that point.

// Make sure UNICODE settings are consistent
#if defined(wxUSE_UNICODE) && wxUSE_UNICODE
	#if !defined(UNICODE)
		#error UNICODE must be defined when using wxUSE_UNICODE
	#endif
	#if !defined(_UNICODE)
		#error _UNICODE must be defined when using wxUSE_UNICODE
	#endif
#endif

// VC9 defines _HAS_TR1 if tr1 is present.
#include <memory>
// If not present, pick up boost's. Now we can use std::tr1::shared_ptr
#if !_HAS_TR1
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
#endif

#include <assert.h>


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

// ARB_SET_ERASE_RETURNS_ITERATOR
//  std::set<x>::iterator i = var.begin()...; i = var.erase(i); ...
//  If not defined, will use "var.erase(i++);" construct
#ifdef _MSC_VER
#define ARB_SET_ERASE_RETURNS_ITERATOR
#endif

// ARB_HAS_GETSYSTEMINFO
//  Has GetSystemInfo in order to check for x64.
#if !defined(_MSC_VER) || _MSC_VER >= 1300
#define ARB_HAS_GETSYSTEMINFO
#endif

// ARB_HAS_PRAGMAPUSHPOP
//  Supports push/pop/warnings
//   #pragma warning (push)
//   #pragma warning (disable : 4355)
#ifdef _MSC_VER
#define ARB_HAS_PRAGMAPUSHPOP
#endif

// Some often used includes for speeding the build
#ifndef _WIN32
#include <map>
#include <set>
#include <string>
#include <vector>

#include <wx/choicebk.h>
#include <wx/clipbrd.h>
#include <wx/colour.h>
#include <wx/docview.h>
#include <wx/font.h>
#include <wx/fontdlg.h>
#include <wx/html/htmlwin.h>
#include <wx/hyperlink.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/listbook.h>
#include <wx/notebook.h>
#include <wx/string.h>
#include <wx/toolbook.h>
#include <wx/treebook.h>
#include <wx/treectrl.h>
#include <wx/valgen.h>
#include <wx/valtext.h>
#endif

// For _() usage when used indirectly.
// poedit is configured to also look for these.
#if wxUSE_UNICODE
#define arbT(x)	L##x
#else
#define arbT(x)	x
#endif

/*
 * Bind is only available on wx2.9+.
 * Rather than ifdef each instance of Connect, hide behind an evil macro.
 */
#if wxCHECK_VERSION(2, 9, 3)
	#define BIND_OR_CONNECT(evt, cast, func) \
		Bind(evt, &func, this)
	#define UNBIND_OR_DISCONNECT(evt, cast, func) \
		Unbind(evt, &func, this)

	#define BIND_OR_CONNECT_CTRL(ctrl, evt, cast, func) \
		ctrl->Bind(evt, &func, this)
	#define UNBIND_OR_DISCONNECT_CTRL(ctrl, evt, cast, func) \
		ctrl->Unbind(evt, &func, this)

	#define BIND_OR_CONNECT_ID(ctrl, id, evt, cast, func) \
		ctrl->Bind(evt, &func, this, id)
	#define UNBIND_OR_DISCONNECT_ID(ctrl, id, evt, cast, func) \
		ctrl->Unbind(evt, &func, this, id)

#else
	#define BIND_OR_CONNECT(evt, cast, func) \
		Connect(evt, cast(func))
	#define UNBIND_OR_DISCONNECT(evt, cast, func) \
		Disconnect(evt, cast(func))

	#define BIND_OR_CONNECT_CTRL(ctrl, evt, cast, func) \
		ctrl->Connect(evt, cast(func), NULL, this)
	#define UNBIND_OR_DISCONNECT_CTRL(ctrl, evt, cast, func) \
		ctrl->Disconnect(evt, cast(func), NULL, this);

	#define BIND_OR_CONNECT_ID(ctrl, id, evt, cast, func) \
		ctrl->Connect(id, evt, cast(func), NULL, this)
	#define UNBIND_OR_DISCONNECT_ID(ctrl, id, evt, cast, func) \
		ctrl->Disconnect(id, evt, cast(func), NULL, this)
#endif
