#pragma once

/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, AgilityBook,
 * AgilityRecordBook or "Agility Record Book" not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission. David Connet makes no representations about
 * the suitability of this software for any purpose. It is provided "as is"
 * without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * OSI Certified Open Source Software
 * http://opensource.org
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
 */

#ifdef WIN32

// Vc7 == _MSC_VER=1300
#if _MSC_VER < 1300
#if defined(UNICODE) || defined(_UNICODE)
	#error In order to compile with UNICODE in VC6, the code needs a lot of work!
#endif
// Turn off some warnings in vc6.
#pragma warning ( disable : 4786 )	// identifier was truncated to '255' characters in the debug information
//#pragma warning( disable : 4503 )
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		/// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.

#ifndef WINVER				/// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		/// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		/// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400	/// Minimum system, NT4.0
#endif

#if _MSC_VER >= 1300
#ifndef _WIN32_WINDOWS		/// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 /// Minimum system, Win98
#endif
#endif

#ifndef _WIN32_IE			/// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0401	/// Minimum IE, 4.01
#endif

// This is new as of vc7
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	/// some CString constructors will be explicit

// This is new as of vc7
/// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxdlgs.h>

#else

// Include other platform common files here. This way we can continue to
// include 'stdafx.h' as the first header in all .cpp files so win32
// pre-compiled headers work properly.

#endif // WIN32
