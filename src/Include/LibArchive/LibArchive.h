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

#if defined(__WXWINDOWS__)
extern bool ExtractFile(
		std::wstring const& zipFile,
		wxString const& archiveFile,
		std::ostream& outData);

extern bool ReplaceFile(
		std::wstring const& zipFile,
		wxString const& archiveFile,
		std::istream& inData);

#else
extern bool ExtractFile(
		std::string const& zipFile,
		std::string const& archiveFile,
		std::ostream& outData);

extern bool ReplaceFile(
		std::string const& zipFile,
		std::string const& archiveFile,
		std::istream& inData);
#endif
