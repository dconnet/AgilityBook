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
 * @brief Precompiled header file when using wxWidgets.
 * @author David Connet
 *
 * Include file for standard system include files, or project specific
 * include files that are used frequently, but are changed infrequently
 *
 * Revision History
 * @li 2013-01-27 DRC Split/moved stdafx.h.
 * @li 2013-01-01 DRC Added _VARIADIC_MAX for vc11.
 * @li 2012-01-29 DRC Add macro to ease selecting all text on dialog init.
 * @li 2011-12-22 DRC Add a macro to make using Bind() easier.
 * @li 2009-05-30 DRC Tweaked pragma message macro.
 * @li 2009-03-09 DRC Added ARB_SET_ERASE_RETURNS_ITERATOR.
 * @li 2005-10-30 DRC Added static xerces library support.
 */


#include "Platform/SetupARBPre.h"

#ifdef NO_WXWIDGETS
#error Then do not include this file!
#endif

#include "Platform/arbWarningPush.h"
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
#include <wx/zipstrm.h>
#endif

// ARBv3 was developed against v2.9.4+ - anything earlier is not supported.
// It makes use of features not available in earlier versions.
#include <wx/version.h>
#include "Platform/arbWarningPop.h"

#if !wxCHECK_VERSION(2, 9, 4)
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


/*
 * Bind is only available on wx2.9+.
 * Rather than ifdef each instance of Connect, hide behind an evil macro.
 */
#if wxCHECK_VERSION(2, 9, 4)
	#define BIND_OR_CONNECT(evt, cast, func) \
		Bind(evt, &func, this)
	#define UNBIND_OR_DISCONNECT(evt, cast, func) \
		Unbind(evt, &func, this)

	#define BIND_OR_CONNECT_CTRL(ctrl, evt, cast, func) \
		ctrl->Bind(evt, &func, this)
	#define UNBIND_OR_DISCONNECT_CTRL(ctrl, evt, cast, func) \
		ctrl->Unbind(evt, &func, this)

	#define BIND_OR_CONNECT_ID(id, evt, cast, func) \
		Bind(evt, &func, this, id)
	#define UNBIND_OR_DISCONNECT_ID(id, evt, cast, func) \
		Unbind(evt, &func, this, id)

	#define BIND_OR_CONNECT_ID_CTRL(ctrl, id, evt, cast, func) \
		ctrl->Bind(evt, &func, this, id)
	#define UNBIND_OR_DISCONNECT_ID_CTRL(ctrl, id, evt, cast, func) \
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

	#define BIND_OR_CONNECT_ID(id, evt, cast, func) \
		Connect(id, evt, cast(func), NULL, this)
	#define UNBIND_OR_DISCONNECT_ID(id, evt, cast, func) \
		Disconnect(id, evt, cast(func), NULL, this)

	#define BIND_OR_CONNECT_ID_CTRL(ctrl, id, evt, cast, func) \
		ctrl->Connect(id, evt, cast(func), NULL, this)
	#define UNBIND_OR_DISCONNECT_ID_CTRL(ctrl, id, evt, cast, func) \
		ctrl->Disconnect(id, evt, cast(func), NULL, this)
#endif


/**
 * Macros to enable easy first control focus.
 */
#define DECLARE_ON_INIT() \
		wxWindow* m_Focus; \
		void OnInit(wxInitDialogEvent& evt);
#define IMPLEMENT_ON_INIT(cls, ctrl) \
		{ \
			BIND_OR_CONNECT(wxEVT_INIT_DIALOG, wxInitDialogEventHandler, cls::OnInit); \
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