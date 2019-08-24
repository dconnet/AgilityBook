#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Precompiled header file when using wxWidgets.
 * @author David Connet
 *
 * Include file for standard system include files, or project specific
 * include files that are used frequently, but are changed infrequently
 *
 * Revision History
// 2019-08-24 Up minimum wx to 3.1.
 * 2015-11-25 Created ARB_64BIT
 * 2013-01-27 Split/moved stdafx.h.
 * 2013-01-01 Added _VARIADIC_MAX for vc11.
 * 2012-01-29 Add macro to ease selecting all text on dialog init.
 * 2011-12-22 Add a macro to make using Bind() easier.
 * 2009-05-30 Tweaked pragma message macro.
 * 2009-03-09 Added ARB_SET_ERASE_RETURNS_ITERATOR.
 * 2005-10-30 Added static xerces library support.
 */

#include "Platform/SetupARBPre.h"

#ifndef USE_WXWIDGETS
#error Then do not include this file!
#endif

#include "Platform/arbWarningPush.h"
#if defined(_MSC_VER) && _MSC_VER > 1800
#pragma warning(disable : 4458) // declaration of 'identifier' hides class member
#endif

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

// Include most wx headers needed here. Include any that cause warnings.
#ifndef WX_MINIMAL_HEADERS
#include <wx/bmpcbox.h>
#include <wx/choicebk.h>
#include <wx/clipbrd.h>
#include <wx/colour.h>
#include <wx/dataview.h>
#include <wx/docview.h>
#include <wx/font.h>
#include <wx/fontdlg.h>
#include <wx/html/helpctrl.h>
#include <wx/html/htmlwin.h>
#include <wx/html/htmprint.h>
#include <wx/hyperlink.h>
#include <wx/imaglist.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>
#include <wx/stdstream.h>
#include <wx/string.h>
#include <wx/textfile.h>
#include <wx/toolbook.h>
#include <wx/treebook.h>
#include <wx/treectrl.h>
#include <wx/valgen.h>
#include <wx/valtext.h>
#endif

// Note: wxDLG_UNIT is defined in wx/defs.h
#define wxDLG_UNIT_X(parent, value) parent->ConvertDialogToPixels(wxSize(value,0)).x
#define wxDLG_UNIT_Y(parent, value) parent->ConvertDialogToPixels(wxSize(0,value)).y

// ARBv3 was developed against v3.0.0+ - anything earlier is not supported.
// It makes use of features not available in earlier versions.
#include <wx/version.h>
#include "Platform/arbWarningPop.h"

#if !wxCHECK_VERSION(3, 1, 0)
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

// WX does not define a common 64bit flag

#if defined(__WXMSW__)
 #if defined(_WIN64) && !defined(ARB_64BIT)
  #define ARB_64BIT
 #endif
#elif defined(__WXMAC__)
 #if defined(__LP64__) && !defined(ARB_64BIT)
  #define ARB_64BIT
 #endif
#elif defined(__WXGTK__) || defined(__WXX11__)
 // Just assume GTK/X11 means linux (ubuntu is what I've tested)
 #if defined(__LP64__) && !defined(ARB_64BIT)
  #define ARB_64BIT
 #endif
#else
#error Unknown platform
#endif


/**
 * Macros to enable easy first control focus.
 */
#define DECLARE_ON_INIT() \
		wxWindow* m_Focus = nullptr; \
		void OnInit(wxInitDialogEvent& evt);
#define IMPLEMENT_ON_INIT(cls, ctrl) \
		{ \
			Bind(wxEVT_INIT_DIALOG, &cls::OnInit, this); \
			m_Focus = ctrl; \
		}
#define DEFINE_ON_INIT(cls) \
	void cls::OnInit(wxInitDialogEvent& evt) \
	{ \
		/* wxWindowBase::OnInitDialog */ \
		TransferDataToWindow(); \
		UpdateWindowUI(wxUPDATE_UI_RECURSE); \
		/* end wxWindowBase */ \
		if (m_Focus) \
		{ \
			m_Focus->SetFocus(); \
			wxTextCtrl* pText = wxDynamicCast(m_Focus, wxTextCtrl); \
			if (pText) \
				pText->SelectAll(); \
		} \
	}

#include "Platform/SetupARBPost.h"
