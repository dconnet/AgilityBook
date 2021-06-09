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

#if defined(_DLL) && defined(_WIN32)
#if defined(LIBTIDYHTML_EXPORT)
#define LIBTIDYHTML_API __declspec(dllexport)
#else
#define LIBTIDYHTML_API __declspec(dllimport)
#endif
#else
#define LIBTIDYHTML_API
#endif

#include "ARBCommon/Element.h"
#include <string>

LIBTIDYHTML_API ElementNodePtr
TidyHtmlData(std::string const& data, fmt::wmemory_buffer& err, std::string const* pRawFileBaseName = nullptr);
