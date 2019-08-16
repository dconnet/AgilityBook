/*
 * Copyright (c) David Connet. All Rights Reserved.
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
 * 2019-06-28 Fix issue with change nBackups to less than existing backups.
 * 2018-10-30 Moved some utils from ARBWin.
 */

#include "stdafx.h"
#include "ARBCommon/ARBUtils.h"

#include <wx/config.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


#if USE_STACKTRACER && defined(WIN32)
int CStackTracer::fIndent = 0;
#endif


CConfigPathHelper::CConfigPathHelper(wxString const& key)
	: m_path(wxConfig::Get()->GetPath())
{
	if (!key.empty())
	{
		wxConfig::Get()->SetPath(key);
	}
}


CConfigPathHelper::~CConfigPathHelper()
{
	wxConfig::Get()->SetPath(m_path);
}


wxString GetARBResourceDir()
{
#if defined(__WXMAC__)
	// Note: For command line, this now (as of 3.1.2) returns the exe path.
	// For the UI, it's the resource bundle path
	return wxStandardPaths::Get().GetResourcesDir();
#else
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	return fileName.GetPath();
#endif
}


bool CreateBackupFile(
		wxString const& inFilename,
		int nBackups)
{
	bool bChanged = false;
	if (0 < nBackups)
	{
		// First find a hole.
		int nHole = -1;
		int i;
		for (i = 1; i <= nBackups; ++i)
		{
			wxString backup = wxString::Format(L"%s.bck%d", inFilename.c_str(), i);
			if (!wxFile::Exists(backup))
			{
				nHole = i;
				break;
			}
		}
		if (-1 == nHole)
			nHole = nBackups;
		// Then shift all the files into the hole.
		for (i = nHole; i > 1; --i)
		{
			wxString backup = wxString::Format(L"%s.bck%d", inFilename.c_str(), i);
			if (wxFile::Exists(backup))
				wxRemoveFile(backup);
			wxString filename = wxString::Format(L"%s.bck%d", inFilename.c_str(), i-1);
			wxRenameFile(filename, backup);
			bChanged = true;
		}
		// File may not exist if doing a 'save as'
		if (wxFile::Exists(inFilename))
		{
			bChanged = true;
			wxString backup = inFilename + L".bck1";
			wxCopyFile(inFilename, backup, true); // Force an overwrite (if nBackups+1 exists, we error out otherwise)
		}
	}
	return bChanged;
}


bool GetFileTimes(
		wxFileName const& filename,
		wxDateTime* dtAccess,
		wxDateTime* dtMod,
		wxDateTime* dtCreate)
{
#if defined(__WXMSW__)
	// Using wx to get the times on network files is really slow.
	// I suspect it's the win32 CreateFile/GetFileTime apis.
	struct __stat64 s;
	if (0 != _tstat64(filename.GetFullPath().wx_str(), &s))
		return false;
	if (dtAccess)
		*dtAccess = wxDateTime(s.st_atime);
	if (dtMod)
		*dtMod = wxDateTime(s.st_mtime);
	if (dtCreate)
		*dtCreate = wxDateTime(s.st_ctime);
	return true;
#else
	return filename.GetTimes(dtAccess, dtMod, dtCreate);
#endif
}
