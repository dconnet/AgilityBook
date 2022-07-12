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


namespace dconSoft
{


class CConfigHandler : public ARB::IARBConfigHandler
{
	DECLARE_NO_COPY_IMPLEMENTED(CConfigHandler)
public:
	CConfigHandler();
	virtual ~CConfigHandler();

	ARBCommon::ElementNodePtr LoadDefaultConfig() const override;
	std::string LoadDTD() const override;
};

} // namespace dconSoft
