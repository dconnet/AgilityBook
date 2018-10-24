#ifdef _MSC_VER
#pragma once
#endif

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * Utility functions
 *
 * License: See License.txt
 */


/**
 * Helper to reset wxConfig path
 */
class CConfigPathHelper
{
	wxString m_path;

public:
	CConfigPathHelper(wxString const& key);
	~CConfigPathHelper();
};

/////////////////////////////////////////////////////////////////////////////
// Debugging performance aid

#ifndef USE_STACKTRACER
#ifdef WIN32
#ifdef _DEBUG
#define USE_STACKTRACER 1
#else
#define USE_STACKTRACER 0
#endif
#endif
#endif

#if USE_STACKTRACER && defined(WIN32)
class CStackTracer
{
public:
	CStackTracer(wxString const& msg) : fMsg(msg)
	{
		fTics = fTickle = GetTickCount();
		++fIndent;
		OutputDebugString(wxString::Format(L"%*s%s: Enter\n", fIndent, L" ", fMsg).wx_str());		
	}
	void Tickle(wxString const& msg)
	{
		DWORD dw = GetTickCount();
		OutputDebugString(wxString::Format(L"%*s%s: Tickle [%d]\n", fIndent, L" ", msg, dw - fTickle).wx_str());
		fTickle = dw;
	}
	~CStackTracer()
	{
		OutputDebugString(wxString::Format(L"%*s%s: Leave [%d]\n", fIndent, L" ", fMsg, GetTickCount() - fTics).wx_str());
		--fIndent;
	}
private:
	wxString fMsg;
	DWORD fTics;
	DWORD fTickle;
	static int fIndent;
};

#define STACK_TRACE(name, msg)	CStackTracer name(msg)
#define STACK_TICKLE(name, msg) name.Tickle(msg)

#else
#define STACK_TRACE(name, msg)
#define STACK_TICKLE(name, msg)
#endif
