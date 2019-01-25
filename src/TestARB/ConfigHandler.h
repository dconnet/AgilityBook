#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Implementation of IARBConfigHandler
 * @author David Connet
 *
 * Revision History
 * 2009-01-03 Ported to wxWidgets.
 * 2008-10-31 Created
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
	virtual ~CConfigHandler() {}

	ElementNodePtr LoadDefaultConfig() const override;
	std::string LoadDTD() const override;
};
