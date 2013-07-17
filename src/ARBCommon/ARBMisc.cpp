/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Utility classes
 * @author David Connet
 *
 * Revision History
 * @li 2013-07-17 DRC Created
 */

#include "stdafx.h"
#include "ARBCommon/ARBMisc.h"

#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include <iostream>
#include <math.h>
#include <sstream>
#include <time.h>

#if defined(__WXWINDOWS__)
#include <wx/string.h>
#endif

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

std::wstring SanitizeStringForHTML(
		std::wstring const& inRawData,
		bool bConvertCR)
{
	std::wstring::size_type pos = inRawData.find_first_of(L"&<>");
	if (std::wstring::npos == pos && bConvertCR)
		pos = inRawData.find_first_of(L"\r\n");
	if (std::wstring::npos == pos)
		return inRawData;
	std::wostringstream data;
	for (size_t nChar = 0; nChar < inRawData.length(); ++nChar)
	{
		switch (inRawData[nChar])
		{
		case L'&':
			data << L"&amp;";
			break;
		case L'<':
			data << L"&lt;";
			break;
		case L'>':
			data << L"&gt;";
			break;
		case L'\r':
			if (bConvertCR)
			{
				if (nChar + 1 < inRawData.length() && '\n' == inRawData[nChar+1])
					continue;
				else
					data << L"<br/>";
			}
			else
				data << inRawData[nChar];
			break;
		case '\n':
			if (bConvertCR)
				data << L"<br/>";
			else
				data << inRawData[nChar];
			break;
		default:
			data << inRawData[nChar];
			break;
		}
	}
	return data.str();
}

/////////////////////////////////////////////////////////////////////////////

bool GetOSInfo(int& verMajor, int& verMinor)
{
	verMajor = verMinor = 0;

#if defined(__WXWINDOWS__)
	wxPlatformInfo info;
	verMajor = info.GetOSMajorVersion();
	verMinor = info.GetOSMinorVersion();
#ifdef __WXMAC__
#ifndef __WXOSX_COCOA__
#if !wxCHECK_VERSION(2, 9, 5)
	verMajor = (verMajor == 16 ? 10 : verMajor);
	verMinor = (verMinor >> 4);
#endif
#endif
#endif
	return true;

#else
#pragma PRAGMA_TODO(non-wx version)
	return false;
#endif
}

/////////////////////////////////////////////////////////////////////////////

std::wstring GetOSInfo(bool bVerbose)
{
	std::wostringstream str;

#if defined(__WXWINDOWS__)
	int majVer;
	int minVer;
	if (!GetOSInfo(majVer, minVer))
		return std::wstring();

	wxPlatformInfo info;
	str << L"OS: "
		<< info.GetOperatingSystemIdName()
		<< L" "
		<< majVer
		<< L"."
		<< minVer
		<< L"\n";

	str << L"Architecture: "
		<< info.GetArchName()
		<< L", "
		<< info.GetEndiannessName()
		<< L"\n";


#else
#pragma PRAGMA_TODO(non-wx version)

#endif
	return str.str();
}
