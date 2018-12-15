#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Tidy html
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2017-12-19 Added pRawFileBaseName for debugging.
 * 2013-06-10 Separated from Win/CalendarSiteUSDAA.cpp
 */

#include "ARBCommon/Element.h"
#include <string>

extern ElementNodePtr TidyHtmlData(
		std::string const& data,
		fmt::wmemory_buffer& err,
		std::string const* pRawFileBaseName = nullptr);
