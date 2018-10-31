#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * Utility functions
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Global functions
 * @author David Connet
 *
 * Revision History
 * 2018-10-30 Moved some utils from ARBWin.
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


/**
 * Create a backup file (and reorganize if some were deleted)
 * @param inFilename File name to backup.
 * @param nBackups Number of backups to keep.
 * @return Backup files reorganized.
 */
extern bool CreateBackupFile(
		wxString const& inFilename,
		int nBackups);


/**
 * Using wx to get the times on network files is really slow.
 * I suspect it's the win32 CreateFile/GetFileTime apis.
 * @param filename File name to access.
 * @param dtAccess Last access time, may be NULL.
 * @param dtMod Last modification time, may be NULL.
 * @param dtCreate Creation time, may be NULL.
 * @return Success
 */
extern bool GetFileTimes(
		wxFileName const& filename,
		wxDateTime* dtAccess,
		wxDateTime* dtMod,
		wxDateTime* dtCreate);

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
