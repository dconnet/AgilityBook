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
 * 2022-02-01 Added backup directory to CreateBackupFile.
 * 2021-08-24 Make StackTracer work on non-Win32 platforms.
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
 * @param inDirectory Directory where backups are kept. (default: inFilename)
 * @return Backup files reorganized.
 */
ARBCOMMON_API bool CreateBackupFile(wxString const& inFilename, int nBackups, wxString const& inDirectory = wxString());


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

#if USE_STACKTRACER

class ARBCOMMON_API CStackTracer
{
public:
	explicit CStackTracer(wxString const& msg);
	~CStackTracer();
	void Tickle(wxString const& msg);

private:
	wxString m_msg;
	wxStopWatch m_stopwatch;
	long m_tickle;
	static int m_indent;

	CStackTracer() = delete;
	CStackTracer(CStackTracer const&) = delete;
	CStackTracer(CStackTracer&&) = delete;
	CStackTracer& operator=(CStackTracer const&) = delete;
	CStackTracer& operator=(CStackTracer&&) = delete;
};

#define STACK_TRACE(name, msg)  CStackTracer name(msg)
#define STACK_TICKLE(name, msg) name.Tickle(msg)

#else
#define STACK_TRACE(name, msg)
#define STACK_TICKLE(name, msg)
#endif
