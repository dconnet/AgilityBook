#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Tidy html
 * @author David Connet
 *
 * Revision History
 * @li 2013-06-10 DRC Separated from Win/CalendarSiteUSDAA.cpp
 */

#include "ARBCommon/Element.h"
#include <string>
#include <sstream>

extern ElementNodePtr TidyHtmlData(
		std::string const& data,
		std::wostringstream& err);
