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
 * 2014-11-19 Add support to read files from Windows Resources.
 * 2014-02-27 Add support for POCO xml.
 * 2013-01-30 Created
 */

#include "stdafx.h"
#include "Platform/SetupMessages.h"
#include "LibArchive/LibArchive.h"

#include "ARBCommon/StringUtil.h"

#if defined(USE_POCO)
#include "Platform/arbWarningPush.h"
#pragma warning(disable : 4244)

#include "Poco/Zip/ZipArchive.h"
#include "Poco/Zip/ZipStream.h"
#include "Poco/StreamCopier.h"
#include <fstream>
#include <sstream>

#include "Platform/arbWarningPop.h"

#elif defined(__WXWINDOWS__)
#include <wx/filesys.h>
#include <wx/fs_mem.h>
#include <wx/mstream.h>
#include <wx/string.h>

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
	CLibArchiveImpl(std::wstring const& zipFile)
#if defined(__WXWINDOWS__) && !defined(USE_POCO)
		: m_zipFile(StringUtil::stringWX(zipFile))
#elif defined(USE_POCO) && defined(ARB_HAS_ISTREAM_WCHAR)
		: m_zipFile(zipFile)
#else
		: m_zipFile(StringUtil::stringA(zipFile))
#endif
	{
		assert(!m_zipFile.empty());
	}

	void FindResourceFile(size_t& size, void*& pVoid);
#if defined(__WXWINDOWS__) && !defined(USE_POCO)
	wxString m_zipFile;
	wxString m_zipFileRes;
#elif defined(USE_POCO) && defined(ARB_HAS_ISTREAM_WCHAR)
	std::wstring m_zipFile;
	std::wstring m_zipFileRes;
#else
	std::string m_zipFile;
	std::string m_zipFileRes;
#endif
};


CLibArchive::CLibArchive(std::wstring const& zipFile)
	: m_pImpl(new CLibArchiveImpl(zipFile))
{
}


CLibArchive::~CLibArchive()
{
	delete m_pImpl;
}


void CLibArchiveImpl::FindResourceFile(size_t& size, void*& pVoid)
{
	size = 0;
	pVoid = nullptr;

#if defined(WIN32)
	std::wstring name;

#if defined(__WXWINDOWS__) && !defined(USE_POCO)
	name = StringUtil::stringW(m_zipFile);
#elif defined(USE_POCO) && defined(ARB_HAS_ISTREAM_WCHAR)
	name = m_zipFile;
#else
	name = StringUtil::stringW(m_zipFile);
#endif
	std::wstring::size_type pos = name.find_last_of('\\');
	if (pos != std::wstring::npos)
		name = name.substr(pos + 1);
	pos = name.find_last_of('/');
	if (pos != std::wstring::npos)
		name = name.substr(pos + 1);
	m_zipFileRes = name;

	name = StringUtil::ToUpper(name);
	name = StringUtil::Replace(name, L".", L"_");

	HRSRC hrSrc = ::FindResource(NULL, name.c_str(), L"DATAFILE");
	if (hrSrc)
	{
		size = ::SizeofResource(NULL, hrSrc);
		if (size > 0)
		{
			HGLOBAL hRes = ::LoadResource(NULL, hrSrc);
			if (hRes)
			{
				pVoid = ::LockResource(hRes);
				// Note, there is no need to unlock.
			}
		}
	}

	if (!pVoid)
		m_zipFileRes.clear();
#endif
}


bool CLibArchive::ExtractFile(
		std::wstring const& inArchiveFile,
		std::ostream& outData)
{
	bool rc = false;

	size_t size = 0;
	void* pVoid = NULL;

#if defined(USE_POCO)
	m_pImpl->FindResourceFile(size, pVoid);
	std::istream* pStream = NULL;
	if (size && pVoid)
	{
		pStream = new std::istringstream(std::string((char*)pVoid, size));
	}
	else
	{
		pStream = new std::ifstream(m_pImpl->m_zipFile, std::ios::binary);
	}
	if (pStream && pStream->good())
	{
		std::string archiveFile = StringUtil::stringA(inArchiveFile);
		Poco::Zip::ZipArchive arch(*pStream);
		Poco::Zip::ZipArchive::FileHeaders::const_iterator it = arch.findHeader(archiveFile);
		if (it != arch.headerEnd())
		{
			Poco::Zip::ZipInputStream zipin(*pStream, it->second);
			Poco::StreamCopier::copyStream(zipin, outData);
			rc = true;
		}
	}
	delete pStream;

#elif defined(__WXWINDOWS__)
	wxString zipfile;
	m_pImpl->FindResourceFile(size, pVoid);
	if (size && pVoid)
	{
		wxMemoryFSHandler::AddFile(m_pImpl->m_zipFileRes, pVoid, size);
		zipfile = L"memory:" + m_pImpl->m_zipFileRes;
	}
	else
	{
		zipfile = wxFileSystem::FileNameToURL(wxString(m_pImpl->m_zipFile.c_str()));
	}
	zipfile += L"#zip:" + wxString(inArchiveFile.c_str());
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
	if (size && pVoid)
		wxMemoryFSHandler::RemoveFile(m_pImpl->m_zipFileRes);

#else
#pragma PRAGMA_TODO(Add res support)
#if 0 // commented out - might just drop non poco/wx mode
	std::string archiveFile = StringUtil::stringA(inArchiveFile);
	unzFile uf = unzOpen(m_pImpl->m_zipFile.c_str());
	if (uf)
	{
		if (UNZ_OK == unzLocateFile(uf, archiveFile.c_str(), 0))
		{
			char filename_inzip[256];
			unz_file_info64 file_info;
			if (UNZ_OK == unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), nullptr, 0, nullptr, 0))
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
#endif

	return rc;
}


bool CLibArchive::ReplaceFile(
		std::wstring const& archiveFile,
		std::istream& inData)
{
#pragma PRAGMA_TODO(ReplaceFile)
	return false;
}
