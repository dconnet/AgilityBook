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

#if defined(_DLL) && defined(_WIN32)
#if defined(LIBARCHIVE_EXPORT)
#define LIBARCHIVE_API	__declspec(dllexport)
#else
#define LIBARCHIVE_API	__declspec(dllimport)
#endif
#else
#define LIBARCHIVE_API
#endif

#include <string>

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


class LIBARCHIVE_API CLibArchive
{
public:
	CLibArchive(std::wstring const& zipFile, ArchiveLocation location = ArchiveLocation::ResourceOrFileSystem);
	~CLibArchive();

	bool IsResource() const;

	bool ExtractFile(
			std::wstring const& archiveFile,
			std::ostream& outData);

	bool ReplaceFile(
			std::wstring const& archiveFile,
			std::istream& inData);

private:
	class CLibArchiveImpl* m_pImpl;
	CLibArchive& operator=(CLibArchive const& rhs) = delete;
	CLibArchive(CLibArchive const& rhs) = delete;
};
