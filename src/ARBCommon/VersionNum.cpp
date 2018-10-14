/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CVersionNum class
 * @author David Connet
 *
 * Revision History
 * 2018-10-14 Treat a 0.0.0.0 vernum as not valid, even when specifically set.
 * 2018-08-15 Changed VERSION_NUMBER to std::array
 * 2012-07-28 Crap, v2.3.6 was hosed. Fix version parsing.
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-28 Removed Windows VERSIONNUM support (use VersionNumber.h)
 * 2009-01-01 Ported to wxWidgets.
 * 2007-10-24 Changed ctor and added <= operators
 * 2006-12-10 Fixed operator< and operator>.
 * 2004-03-04 Created
 */

#include "stdafx.h"
#include "ARBCommon/VersionNum.h"

#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/BreakLine.h"
#include "ARBCommon/StringUtil.h"
#include <sstream>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


bool CVersionNum::Parse(std::wstring const& inVer)
{
	clear();
	std::vector<std::wstring> fields;
	if (4 == BreakLine(L'.', inVer, fields))
	{
		VERSION_NUMBER version = { 0, 0, 0, 0 };
		for (size_t i = 0; i < fields.size(); ++i)
		{
			version[i] = static_cast<unsigned short>(StringUtil::ToCLong(fields[i]));
		}
		Assign(version[0], version[1], version[2], version[3]);
	}
	return m_Valid;
}


std::wstring CVersionNum::GetVersionString() const
{
	std::wostringstream str;
	str << m_Version[0]
		<< L"." << m_Version[1]
		<< L"." << m_Version[2]
		<< L"." << m_Version[3];
	return str.str();
}
