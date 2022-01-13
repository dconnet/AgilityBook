/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Extract a file from a zip archive
 * @author David Connet
 *
 * Revision History
 * 2022-01-13 Only support wxWidgets
 * 2014-11-19 Add support to read files from Windows Resources.
 * 2014-02-27 Add support for POCO xml.
 * 2013-01-30 Created
 */

#include "stdafx.h"
#include "ARBCommon/LibArchive.h"

#include "ARBCommon/StringUtil.h"

#include <wx/filesys.h>
#include <wx/fs_mem.h>
#include <wx/mstream.h>
#include <wx/string.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#define BUFFER_SIZE 1024


class CLibArchiveImpl
{
public:
	CLibArchiveImpl(wxString const& zipFile, ArchiveLocation location);

	bool IsResource() const
	{
		return m_bResource;
	}
	wxString GetZipfileName();
	wxFSFile* GetZipStream(wxString const& inArchiveFile);

	bool m_bResource;
	size_t m_resSize;
	void* m_resData;
	wxString m_zipFile;
	wxString m_zipFileRes;
};


CLibArchiveImpl::CLibArchiveImpl(wxString const& zipFile, ArchiveLocation location)
	: m_bResource(false)
	, m_resSize(0)
	, m_resData(nullptr)
	, m_zipFile(zipFile)
{
	assert(!m_zipFile.empty());
#if defined(WIN32)
	if (ArchiveLocation::ResourceOrFileSystem == location || ArchiveLocation::Resource == location)
	{
		std::wstring name;

		name = StringUtil::stringW(m_zipFile);
		std::wstring::size_type pos = name.find_last_of('\\');
		if (pos != std::wstring::npos)
			name = name.substr(pos + 1);
		pos = name.find_last_of('/');
		if (pos != std::wstring::npos)
			name = name.substr(pos + 1);
		m_zipFileRes = name;

		name = StringUtil::ToUpper(name);
		name = StringUtil::Replace(name, L".", L"_");

		HRSRC hrSrc = ::FindResource(nullptr, name.c_str(), L"DATAFILE");
		if (hrSrc)
		{
			m_resSize = ::SizeofResource(nullptr, hrSrc);
			if (m_resSize > 0)
			{
				HGLOBAL hRes = ::LoadResource(nullptr, hrSrc);
				if (hRes)
				{
					m_resData = ::LockResource(hRes);
					// Note, there is no need to unlock.
				}
			}
		}

		if (m_resData)
		{
			m_bResource = true;
			wxMemoryFSHandler::AddFile(m_zipFileRes, m_resData, m_resSize);
		}
		else
		{
			if (ArchiveLocation::Resource == location)
				m_zipFile.clear();
			m_zipFileRes.clear();
		}
	}
#endif
}


wxString CLibArchiveImpl::GetZipfileName()
{
	wxString zipfile;
	if (!m_zipFile.empty())
	{
		if (IsResource())
		{
			zipfile = L"memory:" + m_zipFileRes;
		}
		else
		{
			zipfile = wxFileSystem::FileNameToURL(m_zipFile);
		}
	}
	return zipfile;
}


wxFSFile* CLibArchiveImpl::GetZipStream(wxString const& inArchiveFile)
{
	if (m_zipFile.empty())
		return nullptr;

	wxString zipfile = GetZipfileName();
	zipfile += L"#zip:" + inArchiveFile;
	wxFileSystem filesys;
	return filesys.OpenFile(zipfile);
}


/////////////////////////////////////////////////////////////////////////////

CLibArchive::CLibArchive(wxString const& zipFile, ArchiveLocation location)
	: m_pImpl(new CLibArchiveImpl(zipFile, location))
{
}


CLibArchive::~CLibArchive()
{
	delete m_pImpl;
}


bool CLibArchive::IsResource() const
{
	return m_pImpl && m_pImpl->IsResource();
}


bool CLibArchive::Exists(wxString const& archiveFile) const
{
	bool exists = false;
	wxFSFile* file = m_pImpl->GetZipStream(archiveFile);
	if (file)
	{
		exists = true;
		delete file;
	}
	return exists;
}


size_t CLibArchive::FindDirectories(wxString const& inArchiveDir, std::vector<wxString>& outDirectories) const
{
	outDirectories.clear();
	if (m_pImpl->m_zipFile.empty())
		return 0;

	wxString zipfile = m_pImpl->GetZipfileName();
	wxFileSystem filesys;

	std::unique_ptr<wxFSFile> file;
	file.reset(filesys.OpenFile(zipfile));
	if (!file.get())
		return 0;
	wxInputStream* input = file->GetStream();
	if (!input)
		return 0;
	wxZipInputStream zip(*input);
	if (!zip.IsOk())
		return 0;

	wxFileName archiveDir(inArchiveDir, wxString());
	auto archiveDirs = archiveDir.GetDirs();

	std::unique_ptr<wxZipEntry> entry;
	for (entry.reset(zip.GetNextEntry()); entry.get() != NULL; entry.reset(zip.GetNextEntry()))
	{
		wxString name = entry->GetName();
		wxFileName filename(name);
		auto dirs = filename.GetDirs();
		if (!dirs.empty() && dirs.size() >= archiveDirs.size())
		{
			bool found = true;
			for (size_t i = 0; i < archiveDirs.size(); ++i)
			{
				if (archiveDirs[i] != dirs[i])
				{
					found = false;
					break;
				}
			}
			if (found)
				outDirectories.push_back(dirs[archiveDirs.size()]);
		}
	}
	return outDirectories.size();
}


bool CLibArchive::ExtractFile(wxString const& inArchiveFile, std::ostream& outData)
{
	bool rc = false;
	if (m_pImpl->m_zipFile.empty())
		return rc;

	wxFSFile* file = m_pImpl->GetZipStream(inArchiveFile);
	if (file)
	{
		size_t size = 0;
		wxInputStream* input = file->GetStream();
		while (input->CanRead())
		{
			char buffer[BUFFER_SIZE];
			size_t num = BUFFER_SIZE;
			input->Read(buffer, num);
			outData.write(buffer, input->LastRead());
			size += input->LastRead();
		}
		delete file;
		rc = true;
	}

	return rc;
}


bool CLibArchive::ReplaceFile(wxString const& archiveFile, std::istream& inData)
{
	bool rc = false;
	if (m_pImpl->m_zipFile.empty())
		return rc;
#pragma PRAGMA_TODO(ReplaceFile)
	return rc;
}
