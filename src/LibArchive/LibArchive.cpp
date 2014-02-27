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
 * 2013-01-30 Created
 */

#include "stdafx.h"
#include "LibArchive/LibArchive.h"

#if defined(USE_POCO)
#include "Poco/Zip/ZipArchive.h"
#include "Poco/Zip/ZipStream.h"
#include "Poco/StreamCopier.h"
#include <fstream>

#elif defined(__WXWINDOWS__)
#include <wx/filesys.h>
#include <wx/mstream.h>

#else
#include "unzip.h"
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#define BUFFER_SIZE	1024


class CLibArchiveImpl
{
public:
#if defined(__WXWINDOWS__) && !defined(USE_POCO)
	CLibArchiveImpl(std::wstring const& zipFile)
		: m_zipFile(zipFile)
	{
		assert(!m_zipFile.empty());
	}
	std::wstring m_zipFile;
#else
	CLibArchiveImpl(std::string const& zipFile)
		: m_zipFile(zipFile)
	{
		assert(!m_zipFile.empty());
	}
	std::string m_zipFile;
#endif
};


#if defined(__WXWINDOWS__) && !defined(USE_POCO)
CLibArchive::CLibArchive(std::wstring const& zipFile)
#else
CLibArchive::CLibArchive(std::string const& zipFile)
#endif
	: m_pImpl(new CLibArchiveImpl(zipFile))
{
}


CLibArchive::~CLibArchive()
{
	delete m_pImpl;
}

#if defined(__WXWINDOWS__) && !defined(USE_POCO)

bool CLibArchive::ExtractFile(
		wxString const& archiveFile,
		std::ostream& outData)
{
	bool rc = false;

	wxString zipfile = wxFileSystem::FileNameToURL(wxString(m_pImpl->m_zipFile.c_str()));
	zipfile += L"#zip:" + archiveFile;
	wxFileSystem filesys;
	wxFSFile* file = filesys.OpenFile(zipfile);
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
	else
	{
		wxLogMessage(zipfile);
		assert(file);
	}
	return rc;
}


bool CLibArchive::ReplaceFile(
		wxString const& archiveFile,
		std::istream& inData)
{
#pragma PRAGMA_TODO(ReplaceFile)
	return false;
}

#else

bool CLibArchive::ExtractFile(
		std::string const& archiveFile,
		std::ostream& outData)
{
	bool rc = false;

#if defined(USE_POCO)
	std::ifstream in(m_pImpl->m_zipFile, std::ios::binary);
	Poco::Zip::ZipArchive arch(in);
	Poco::Zip::ZipArchive::FileHeaders::const_iterator it = arch.findHeader(archiveFile);
	if (it != arch.headerEnd())
	{
		Poco::Zip::ZipInputStream zipin(in, it->second);
		Poco::StreamCopier::copyStream(zipin, outData);
		rc = true;
	}

#else
	unzFile uf = unzOpen(m_pImpl->m_zipFile.c_str());
	if (uf)
	{
		if (UNZ_OK == unzLocateFile(uf, archiveFile.c_str(), 0))
		{
			char filename_inzip[256];
			unz_file_info64 file_info;
			if (UNZ_OK == unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0))
			{
				uInt size_buf = BUFFER_SIZE;
				char* buf = new char[size_buf];
				if (UNZ_OK == unzOpenCurrentFile(uf))
				{
					rc = true;
					int err;
					do
					{
						err = unzReadCurrentFile(uf, buf, size_buf);
						if (err < 0)
							break;
						else if (err > 0)
						{
							outData.write(buf, err);
						}
					} while (err > 0);
				}
				delete [] buf;
			}
		}
		unzClose(uf);
	}
#endif

	return rc;
}


bool CLibArchive::ReplaceFile(
		std::string const& archiveFile,
		std::istream& inData)
{
#pragma PRAGMA_TODO(ReplaceFile)
	return false;
}

#endif
