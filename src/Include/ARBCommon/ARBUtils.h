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
 * 2019-08-15 Added GetARBResourceDir
 * 2018-10-30 Moved some utils from ARBWin.
 */

#include "LibwxARBCommon.h"


/**
 * Helper to reset wxConfig path
 */
class ARBCOMMON_API CConfigPathHelper
{
	wxString m_path;

public:
	explicit CConfigPathHelper(wxString const& key);
	~CConfigPathHelper();

private:
	CConfigPathHelper() = delete;
	CConfigPathHelper(CConfigPathHelper const&) = delete;
	CConfigPathHelper(CConfigPathHelper&&) = delete;
	CConfigPathHelper& operator=(CConfigPathHelper const&) = delete;
	CConfigPathHelper& operator=(CConfigPathHelper&&) = delete;
};


/**
 * Get directory for resources.
 * On Windows/Unix, they're located with the exe.
 * On Mac, they're in the resource bundle
 */
ARBCOMMON_API wxString GetARBResourceDir();


/**
 * Create a backup file (and reorganize if some were deleted)
 * @param inFilename File name to backup.
 * @param nBackups Number of backups to keep.
 * @return Backup files reorganized.
 */
ARBCOMMON_API bool CreateBackupFile(wxString const& inFilename, int nBackups);


/**
 * Using wx to get the times on network files is really slow.
 * I suspect it's the win32 CreateFile/GetFileTime apis.
 * @param filename File name to access.
 * @param dtAccess Last access time, may be NULL.
 * @param dtMod Last modification time, may be NULL.
 * @param dtCreate Creation time, may be NULL.
 * @return Success
 */
ARBCOMMON_API bool GetFileTimes(
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
// clang-format off
#pragma warning(push)
#pragma warning (disable : 26447) // The function is declared 'noexcept' but calls function 'wx_str()' which may throw exceptions(f.6).
#pragma warning (disable : 28159) // Consider using 'GetTickCount64' instead of 'GetTickCount'. Reason: GetTickCount overflows roughly every 49 days.  Code that does not take that into account can loop indefinitely.  GetTickCount64 operates on 64 bit values and does not have that problem
// clang-format on
class ARBCOMMON_API CStackTracer
{
public:
	explicit CStackTracer(wxString const& msg);
	~CStackTracer();
	void Tickle(wxString const& msg);

private:
	wxString fMsg;
	DWORD fTics;
	DWORD fTickle;
	static int fIndent;

	CStackTracer() = delete;
	CStackTracer(CStackTracer const&) = delete;
	CStackTracer(CStackTracer&&) = delete;
	CStackTracer& operator=(CStackTracer const&) = delete;
	CStackTracer& operator=(CStackTracer&&) = delete;
};
#pragma warning(pop)

#define STACK_TRACE(name, msg)  CStackTracer name(msg)
#define STACK_TICKLE(name, msg) name.Tickle(msg)

#else
#define STACK_TRACE(name, msg)
#define STACK_TICKLE(name, msg)
#endif
