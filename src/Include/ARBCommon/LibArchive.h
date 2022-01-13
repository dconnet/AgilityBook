#pragma once

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
 * The non-WX ansi string restriction is due to zlib. wchar_t just doesn't work.
 *
 * Revision History
 * 2014-11-20 Added support for embedded Windows resource.
 * 2014-02-27 Add support for POCO xml.
 * 2013-01-30 Created
 */

#include "LibwxARBCommon.h"

#include <string>
#include <vector>

// TODO: Change api. On Win32, this because ReplaceFile[AW]
#ifdef ReplaceFile
#undef ReplaceFile
#endif


enum class ArchiveLocation
{
	ResourceOrFileSystem, // Try resource, fallback to FS
	FileSystem,           // Only look at FS
	Resource,             // Only in resource (windows only)
};


class ARBCOMMON_API CLibArchive
{
public:
	CLibArchive(wxString const& zipFile, ArchiveLocation location = ArchiveLocation::ResourceOrFileSystem);
	~CLibArchive();

	bool IsResource() const;

	bool Exists(wxString const& archiveFile) const;
	size_t FindDirectories(wxString const& archiveDir, std::vector<wxString>& outDirectories) const;

	bool ExtractFile(wxString const& archiveFile, std::ostream& outData);

	bool ReplaceFile(wxString const& archiveFile, std::istream& inData);

private:
	class CLibArchiveImpl* m_pImpl;
	CLibArchive& operator=(CLibArchive const& rhs) = delete;
	CLibArchive(CLibArchive const& rhs) = delete;
};
