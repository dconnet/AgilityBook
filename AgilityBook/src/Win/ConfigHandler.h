#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Implementation of IARBConfigHandler
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-03 DRC Ported to wxWidgets.
 * @li 2008-10-31 DRC Created
 */

#include "ARB/ARBConfig.h"


class CConfigHandler : public IARBConfigHandler
{
public:
	/**
	 * Load a file from a wxWidget archive (zip file)
	 * @param zipFile Full path name.
	 * @param archiveFile Name of file in zip file to extract.
	 * @param outData Data from archiveFile.
	 * @return Success
	 */
	static bool LoadWxFile(
			std::wstring const& zipFile,
			std::wstring const& archiveFile,
			std::ostream& outData);

	CConfigHandler();

	virtual ElementNodePtr LoadDefaultConfig() const;
	virtual std::string LoadDTD() const;
};
