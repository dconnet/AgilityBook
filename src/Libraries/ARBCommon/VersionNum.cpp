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
 * 2021-10-26 Added 'parts' to GetVersionString.
 * 2018-12-16 Convert to fmt.
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
#include "fmt/xchar.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


bool CVersionNum::Parse(std::wstring const& inVer)
{
	clear();
	std::vector<std::wstring> fields;
	if (4 == BreakLine(L'.', inVer, fields))
	{
		VERSION_NUMBER version = {0, 0, 0, 0};
		for (size_t i = 0; i < fields.size(); ++i)
		{
			version[i] = static_cast<unsigned short>(StringUtil::ToCLong(fields[i]));
		}
		Assign(version[0], version[1], version[2], version[3]);
	}
	return m_Valid;
}


std::wstring CVersionNum::GetVersionString(int parts) const
{
	switch (parts)
	{
	case 3:
		return fmt::format(L"{}.{}.{}", m_Version[0], m_Version[1], m_Version[2]);
	case 2:
		return fmt::format(L"{}.{}", m_Version[0], m_Version[1]);
	case 1:
		return fmt::format(L"{}", m_Version[0]);
	}
	return fmt::format(L"{}.{}.{}.{}", m_Version[0], m_Version[1], m_Version[2], m_Version[3]);
}


std::wstring CVersionNum::GetVersionUsage(
	wxString const& program,
	std::string const& date,
	std::string const& time,
	bool includeNewline) const
{
	auto str
		= wxString::Format(_("%s version %s\n%s"), program, GetVersionString(4).c_str(), GetCompiledOn(date, time));
	if (includeNewline)
		str << L"\n";
	return StringUtil::stringW(str);
}


wxString CVersionNum::GetCompiledOn(std::string const& date, std::string const& time)
{
	return GetCompiledOn(StringUtil::stringW(date), StringUtil::stringW(time));
}


wxString CVersionNum::GetCompiledOn(std::wstring const& date, std::wstring const& time)
{
	return wxString::Format(_("Compiled on %s at %s"), date.c_str(), time.c_str());
}
