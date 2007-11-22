// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#ifndef WINVER
#if _MSC_VER >= 1500
#define WINVER 0x0500
#else
#define WINVER 0x0400
#endif
#endif

#ifndef _WIN32_WINNT
#if _MSC_VER >= 1500
#define _WIN32_WINNT 0x0500
#else
#define _WIN32_WINNT 0x0400
#endif
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0500
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0500
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _AFX_ALL_WARNINGS

#include <afxwin.h>			// MFC core and standard components
#include <afxext.h>			// MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
