/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Utility classes
 * @author David Connet
 *
 * Revision History
 * 2020-04-10 Update to fmt 6.2.
 * 2018-12-16 Convert to fmt.
 * 2018-08-15 Changed ARBVersion to use std::array
 * 2016-09-04 Add a ToString wrapper.
 * 2015-12-22 Changed bAlwaysStripZeros to eStripZeros.
 * 2015-04-22 Specifically use std::abs, on mac it used abs(int).
 * 2014-06-09 Move string->arbversion parsing to ARBVersion.
 * 2013-07-17 Moved SanitizeStringForHTML to ARBMisc.
 * 2012-12-12 Use fabs instead of abs. Works on Mac too.
 * 2012-08-13 Moved ARB_Q to separate file.
 * 2012-05-04 Add bAlwaysStripZeros to ARBDouble::ToString.
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2009-11-24 Optimize locale usage when reading/writing the ARB file.
 * 2009-09-28 Fix abs() on Mac.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-09-13 Add support for wxWidgets 2.9.
 * 2008-06-29 Moved string stuff to ARBString.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Removed ARBDouble.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARBCommon/ARBTypes.h"

#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "fmt/xchar.h"
#include <math.h>
#include <time.h>

#if defined(__WXWINDOWS__)
#include <wx/string.h>
#endif

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

// Trailing zeros are trimmed unless inPrec=2.
// Then they are only trimmed if all zero (and inPrec=2).
std::wstring ARBDouble::ToString(double inValue, int inPrec, bool bUseCurrentLocale, ZeroStrip eStripZeros)
{
	std::wstring retVal;

	// libfmt v6 changed how formatting works.
	// It is now locale-independent (backwards from before).
	// As of v6.2, 'f' format is not supported with locales ('n'/'L' imply 'g')
	// So give up on fmt for locale formating, use wxWidgets instead.
	if (bUseCurrentLocale)
	{
		if (0 < inPrec)
			retVal = wxString::Format(L"%.*f", inPrec, inValue);
		else
			retVal = wxString::Format(L"%g", inValue);
	}
	else
	{
		if (0 < inPrec)
			retVal = fmt::format(L"{:.{}f}", inValue, inPrec);
		else
			retVal = fmt::format(L"{:g}", inValue);
	}

	wchar_t pt = '.';
	if (bUseCurrentLocale)
		pt = CLocaleWrapper::GetDecimalPt();

	if (ZeroStrip::AsIs != eStripZeros)
	{
		std::wstring::size_type pos = retVal.find(pt);
		if (std::wstring::npos != pos)
		{
			// Strip trailing zeros iff they are all 0.
			if (2 == inPrec && ZeroStrip::Compatible == eStripZeros)
			{
				std::wstring twoZeros;
				twoZeros = pt;
				twoZeros += L"00";
				if (retVal.substr(pos) == twoZeros)
				{
					// Input is ".00", so simplify
					if (0 == pos)
						retVal = L"0";
					// Strip the ".00".
					else
						retVal = retVal.substr(0, pos);
				}
			}
			// Strip all trailing 0s.
			else
			{
				size_t len = retVal.length();
				size_t oldLen = len;
				while (0 < len && retVal[len - 1] == '0')
					--len;
				if (0 < len && retVal[len - 1] == pt)
					--len;
				if (len != oldLen)
					retVal = retVal.substr(0, len);
			}
		}
	}
	return retVal;
}


std::wstring ARBDouble::ToString(double inValue, int inPrec, ARBDouble::ZeroStrip eStripZeros)
{
	return ARBDouble::ToString(inValue, inPrec, true, eStripZeros);
}


// http://groups.google.com/group/comp.lang.c++.moderated/msg/518274ddc6fb8541?hl=en&
bool ARBDouble::equal(double const& inVal1, double const& inVal2, double inPrec)
{
	int mag1, mag2;
	frexp(inVal1, &mag1);
	frexp(inVal2, &mag2);
	if (mag1 != mag2)
		return false;

	double epsilon = ldexp(inPrec, mag1);
	double diff = std::abs(inVal1 - inVal2);
	return diff <= epsilon;
}

/////////////////////////////////////////////////////////////////////////////

ARBVersion::ARBVersion(std::wstring inStr)
	: m_Version({0, 0})
{
	std::wstring::size_type pos = inStr.find('.');
	if (std::wstring::npos != pos)
	{
		m_Version[0] = static_cast<unsigned short>(StringUtil::ToCLong(inStr));
		inStr = inStr.substr(pos + 1);
		m_Version[1] = static_cast<unsigned short>(StringUtil::ToCLong(inStr));
	}
	else
	{
		m_Version[0] = static_cast<unsigned short>(StringUtil::ToCLong(inStr));
	}
}

std::wstring ARBVersion::str() const
{
	return fmt::format(L"{}.{}", Major(), Minor());
}
