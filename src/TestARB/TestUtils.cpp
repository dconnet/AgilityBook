/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test util functions
 * @author David Connet
 *
 * Revision History
 * 2019-08-15 Added resource directory test.
 * 2019-06-28 Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBCommon/ARBUtils.h"

#include <wx/fileconf.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class SetFiles
{
	wxString m_filename;
	std::vector<wxString> m_filenames;

public:
	SetFiles()
	{
		// Note: prefix is only up to 3 chars.
		m_filename = wxFileName::CreateTempFileName(L"arb");
		m_filenames.push_back(m_filename);
	}

	wxString FileName() const { return m_filename; }

	~SetFiles()
	{
		Clear();
	}

	bool Clear()
	{
		for (auto name : m_filenames)
		{
			if (wxFile::Exists(name))
			{
				if (!wxRemoveFile(name))
					return false;
			}
		}
		m_filenames.clear();
		return true;
	}

	bool Reset(int nFiles)
	{
		Clear();
		// Create one more file than requested
		for (int i = 0; i <= nFiles + 1; ++i)
		{
			wxString name;
			if (0 == i)
				name = m_filename;
			else
				name = wxString::Format(L"%s.bck%d", m_filename, i);
			m_filenames.push_back(name);

			wxTextFile file(name);
			if (wxFile::Exists(name))
			{
				if (!file.Open())
					return false;
			}
			else
			{
				if (!file.Create())
					return false;
			}
			file.AddLine(wxString::Format(L"%d", i));
			file.Write();
			file.Close();
		}
		return true;
	}

	// nKill == 'kill backup n' (0 == basefile)
	bool Hole(int nFiles, int nKill)
	{
		if (nKill > nFiles)
			return false;
		Reset(nFiles);
		wxRemoveFile(m_filenames[nKill]);
		return true;
	}

	bool Verify(int backups, int nHole)
	{
		bool bVerified = true;
		if (0 < backups)
		{
			// Work backwards - we don't care if trailing files don't exist.
			bool bExists = false;
			for (int i = static_cast<int>(m_filenames.size()); i != 0; --i)
			{
				if (!wxFile::Exists(m_filenames[i - 1]))
				{
					// We deleted (nHole) +2 and renamed (via backup) +1
					if (i == nHole + 2 || i == nHole + 1)
						continue;
					if (bExists)
						return false;
					continue;
				}
				wxTextFile file(m_filenames[i-1]);
				if (!file.Open())
					return false;
#pragma PRAGMA_TODO("Validity test that files renamed properly")
			}
		}
		return bVerified;
	}
};


TEST_CASE("Utils")
{
	SetFiles files;

	SECTION("ConfigPath")
	{
		if (!g_bMicroTest)
		{
			wxFileConfig config;
			auto old = wxConfig::Set(&config);

			config.SetPath(L"Test");
			REQUIRE(config.GetPath() == L"/Test");
			{
				CConfigPathHelper path(L"Test2");
				REQUIRE(config.GetPath() == L"/Test/Test2");
				config.SetPath(L"Test3");
				REQUIRE(config.GetPath() == L"/Test/Test2/Test3");
			}
			REQUIRE(config.GetPath() == L"/Test");

			wxConfig::Set(old);
		}
	}


	SECTION("ResourceDir")
	{
		if (!g_bMicroTest)
		{
			wxString dir = GetARBResourceDir();
			REQUIRE(!dir.empty());
			wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
			// Note: For Mac, this is only true with a command line program
			REQUIRE(fileName.GetPath() == dir);
		}
	}

	SECTION("BackupFiles0")
	{
		if (!g_bMicroTest)
		{
			const int nBackups = 0;
			// '0' is special - no backups are made so API returns false (no change)
			for (int i = 0; i < 2; ++i)
			{
				if (i == 0)
					REQUIRE(files.Clear());
				else
					REQUIRE(files.Reset(nBackups));
				REQUIRE(!CreateBackupFile(files.FileName(), nBackups));
				REQUIRE(files.Verify(nBackups, -1));
			}
		}
	}


	SECTION("BackupFilesClean1")
	{
		if (!g_bMicroTest)
		{
			const int nBackups = 1;
			REQUIRE(files.Clear());
			REQUIRE(!CreateBackupFile(files.FileName(), nBackups));
			REQUIRE(files.Verify(nBackups, -1));
		}
	}
	SECTION("BackupFilesExisting1")
	{
		if (!g_bMicroTest)
		{
			const int nBackups = 1;
			REQUIRE(files.Reset(nBackups));
			REQUIRE(CreateBackupFile(files.FileName(), nBackups));
			REQUIRE(files.Verify(nBackups, -1));
		}
	}


	SECTION("BackupFilesClean2")
	{
		if (!g_bMicroTest)
		{
			const int nBackups = 2;
			REQUIRE(files.Clear());
			REQUIRE(!CreateBackupFile(files.FileName(), nBackups));
			REQUIRE(files.Verify(nBackups, -1));
		}
	}
	SECTION("BackupFilesExisting2")
	{
		if (!g_bMicroTest)
		{
			const int nBackups = 2;
			REQUIRE(files.Reset(nBackups));
			REQUIRE(CreateBackupFile(files.FileName(), nBackups));
			REQUIRE(files.Verify(nBackups, -1));
		}
	}


	SECTION("BackupFilesHole")
	{
		if (!g_bMicroTest)
		{
			const int nBackups = 3;
			for (int i = 0; i <= nBackups; ++i)
			{
				REQUIRE(files.Hole(nBackups, i));
				REQUIRE(CreateBackupFile(files.FileName(), nBackups));
				REQUIRE(files.Verify(nBackups, i));
			}
		}
	}
}
