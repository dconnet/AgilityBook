#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Extract a file from a zip archive
 * @author David Connet
 *
 * The non-WX ansi string restriction is due to zlib. wchar_t just doesn't work.
 *
 * Revision History
 * @li 2013-01-30 DRC Created
 */

#include <string>


class CLibArchive
{
public:
#if defined(__WXWINDOWS__)
	CLibArchive(std::wstring const& zipFile);
#else
	CLibArchive(std::string const& zipFile);
#endif
	~CLibArchive();

	bool ExtractFile(
#if defined(__WXWINDOWS__)
			wxString const& archiveFile,
#else
			std::string const& archiveFile,
#endif
			std::ostream& outData);

	bool ReplaceFile(
#if defined(__WXWINDOWS__)
			wxString const& archiveFile,
#else
			std::string const& archiveFile,
#endif
			std::istream& inData);

private:
	class CLibArchiveImpl* m_pImpl;
	CLibArchive& operator=(CLibArchive const& rhs);
	CLibArchive(CLibArchive const& rhs);
};
