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

#include "ARBConfig.h"


class CConfigHandler : public IARBConfigHandler
{
public:
	static bool LoadWxFile(
			wxString const& zipFile, ///< Full path name
			wxString const& archiveFile,
			std::string& outData);

	CConfigHandler();

	virtual ElementNodePtr LoadDefaultConfig() const;
	virtual std::string LoadDTD(bool bNormalizeCRNL) const;
};
