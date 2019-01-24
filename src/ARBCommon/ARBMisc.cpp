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
 * 2018-12-16 Convert to fmt.
 * 2018-04-26 Moved ShortToRoman from ARBConfigTitle.
 * 2014-06-19 Added IsWin7OrBetter.
 * 2013-07-17 Created
 */

#include "stdafx.h"
#include "ARBCommon/ARBMisc.h"

#include "ARBCommon/StringUtil.h"
#include "fmt/format.h"
#include <math.h>

// For testing in ARB
//#if defined(__WXWINDOWS__)
//#undef __WXWINDOWS__
//#endif

#if defined(__WXWINDOWS__)
#include <wx/string.h>
#elif defined(_WIN32)
#pragma comment(lib, "version.lib")
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
	fmt::wmemory_buffer data;
	for (size_t nChar = 0; nChar < inRawData.length(); ++nChar)
	{
		switch (inRawData[nChar])
		{
		case L'&':
			fmt::format_to(data, L"{}", L"&amp;");
			break;
		case L'<':
			fmt::format_to(data, L"{}", L"&lt;");
			break;
		case L'>':
			fmt::format_to(data, L"{}", L"&gt;");
			break;
		case L'\r':
			if (bConvertCR)
			{
				if (nChar + 1 < inRawData.length() && '\n' == inRawData[nChar+1])
					continue;
				else
					fmt::format_to(data, L"{}", L"<br/>");
			}
			else
				fmt::format_to(data, L"{}", inRawData[nChar]);
			break;
		case '\n':
			if (bConvertCR)
				fmt::format_to(data, L"<br/>");
			else
				fmt::format_to(data, L"{}", inRawData[nChar]);
			break;
		default:
			fmt::format_to(data, L"{}", inRawData[nChar]);
			break;
		}
	}
	return fmt::to_string(data);
}

/////////////////////////////////////////////////////////////////////////////

bool GetOSInfo(int& verMajor, int& verMinor)
{
	verMajor = verMinor = 0;

#if defined(__WXWINDOWS__)
	wxPlatformInfo info;
	verMajor = info.GetOSMajorVersion();
	verMinor = info.GetOSMinorVersion();
	return true;

#elif defined(_WIN32)
	std::wstring kernel(L"kernel32.dll");
	DWORD dwHandle;
	DWORD dwLen = GetFileVersionInfoSize(kernel.c_str(), &dwHandle);
	if (!dwLen) 
		return false;

	void* lpData = malloc(dwLen);
	if (!lpData) 
		return false;

	if (!GetFileVersionInfo(kernel.c_str(), dwHandle, dwLen, lpData))
	{
		free(lpData);
		return false;
	}

	UINT BufLen;
	VS_FIXEDFILEINFO* pFileInfo = nullptr;
	if (VerQueryValue(lpData, L"\\", (void**)&pFileInfo, &BufLen))
	{
		verMajor= HIWORD(pFileInfo->dwFileVersionMS);
		verMinor = LOWORD(pFileInfo->dwFileVersionMS);
		//*BuildNumber = HIWORD(pFileInfo->dwFileVersionLS);
		//*RevisionNumber = LOWORD(pFileInfo->dwFileVersionLS);
		free(lpData);
		return true;
	}
	free(lpData);
	return false;

#else
#error Unknown OS
#endif
}

/////////////////////////////////////////////////////////////////////////////

static std::wstring GetOSName()
{
	std::wstring str;

	int majVer;
	int minVer;
	if (!GetOSInfo(majVer, minVer))
		return std::wstring();

#if defined(__WXWINDOWS__)
	wxPlatformInfo info;
	str = fmt::format(L"{} {}.{}", info.GetOperatingSystemIdName().wx_str(), majVer, minVer);

#elif defined(_WIN32)
	switch (majVer)
	{
	default:
		str = fmt::format(L"Windows {}.{}", majVer, minVer);
		break;
	case 6:
		switch (minVer)
		{
		default:
			str = fmt::format(L"Windows {}.{}", majVer, minVer);
			break;
		case 0:
			str = L"Windows Vista";
			break;
		case 1:
			str = L"Windows 7";
			break;
		case 2:
			str = L"Windows 8";
			break;
		case 3:
			str = L"Windows 8.1";
			break;
		}
		break;
	case 5:
		switch (minVer)
		{
		default:
			str = fmt::format(L"Windows {}.{}", majVer, minVer);
			break;
		case 2:
			str = L"Windows XP"; // Not really, 64bitXP, or Server
			break;
		case 1:
			str = L"Windows XP";
			break;
		case 0:
			str = L"Windows 2000";
			break;
		}
		break;
	}

#else
#error Unknown OS
#endif

	return str;
}


static std::wstring GetArchName()
{
#if defined(__WXWINDOWS__)
	wxPlatformInfo info;
	return StringUtil::stringW(info.GetArchName());

#elif defined(_WIN32)
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(si));
	GetNativeSystemInfo(&si);
	switch (si.wProcessorArchitecture)
	{
	default:
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
		return L"??";
	case PROCESSOR_ARCHITECTURE_AMD64:
		return L"x64";
	case PROCESSOR_ARCHITECTURE_ARM:
		return L"ARM";
	case PROCESSOR_ARCHITECTURE_IA64:
		return L"ia64";
	case PROCESSOR_ARCHITECTURE_INTEL:
		return L"x86";
	}

#else
#error Unknown OS
#endif
}


static std::wstring GetEndiannessName()
{
#if defined(__WXWINDOWS__)
	wxPlatformInfo info;
	return StringUtil::stringW(info.GetEndiannessName());

#else
	// Copied from wxWidgets 2.9.5: utilscmn.cpp: wxIsPlatformLittleEndian
	// Are we little or big endian? This method is from Harbison & Steele.
	union
	{
		long l;
		char c[sizeof(long)];
	} u;
	u.l = 1;

	bool isLittleEndian = (u.c[0] == 1);
	if (isLittleEndian)
		return L"Little endian";
	else
		return L"Big endian";

#endif
}


std::wstring GetOSInfo(bool bVerbose)
{
	fmt::wmemory_buffer str;

	if (bVerbose)
		fmt::format_to(str, L"OS: ");

	fmt::format_to(str, L"{}", GetOSName());

	if (bVerbose)
	{
		fmt::format_to(str, L"\nArchitecture: %s, %s\n", GetArchName(), GetEndiannessName());
	}

	return fmt::to_string(str);
}

/////////////////////////////////////////////////////////////////////////////

#ifdef WIN32

bool CheckOS(DWORD dwMajor, DWORD dwMinor, int op)
{
	OSVERSIONINFOEX osvi = {0};
	DWORDLONG dwlConditionMask = 0;

	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	osvi.dwMajorVersion = dwMajor;
	osvi.dwMinorVersion = dwMinor;
	osvi.wServicePackMajor = 0;
	osvi.wServicePackMinor = 0;

#pragma warning (push)
// warning C4244: 'argument' : conversion from 'int' to 'BYTE', possible loss of data
#pragma warning (disable : 4244)
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMINOR, op);
#pragma warning (pop)

	return !!VerifyVersionInfo(&osvi, 
			VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
			dwlConditionMask);
}

#endif

bool IsWin7OrBetter()
{
#ifdef WIN32
	return CheckOS(6, 1, VER_GREATER_EQUAL);
#else
	return false;
#endif
}


/////////////////////////////////////////////////////////////////////////////

/*
// Roman number conversion (modified from C# code on CodeProject)
static short RomanDigit(wchar_t digit)
{
	switch (digit)
	{
	case 'I':
		return 1;
	case 'V':
		return 5;
	case 'X':
		return 10;
	case 'L':
		return 50;
	case 'C':
		return 100;
	case 'D':
		return 500;
	case 'M':
		return 1000;
	default :
		//We're in a limited world - don't worry about out-of-range
		//throw new ArgumentOutOfRangeException("digit");
		return 0;
	}
}
static short RomanToShort(std::wstring number)
{
	short result = 0;
	short oldValue = 1000;
	for (std::wstring::const_iterator index = number.begin(); index != number.end(); ++index)
	{
		short newValue = RomanDigit(*index);
		if (newValue > oldValue)
			result = result + newValue - 2 * oldValue;
		else
			result += newValue;
		oldValue = newValue;
	}
	return result;
}
*/

std::wstring ShortToRoman(short value)
{
	static const wchar_t* romanDigits[9][4] =
	{
		{L"M",    L"C",    L"X",    L"I"   },
		{L"MM",   L"CC",   L"XX",   L"II"  },
		{L"MMM",  L"CCC",  L"XXX",  L"III" },
		{nullptr, L"CD",   L"XL",   L"IV"  },
		{nullptr, L"D",    L"L",    L"V"   },
		{nullptr, L"DC",   L"LX",   L"VI"  },
		{nullptr, L"DCC",  L"LXX",  L"VII" },
		{nullptr, L"DCCC", L"LXXX", L"VIII"},
		{nullptr, L"CM",   L"XC",   L"IX"  }
	};
	fmt::wmemory_buffer result;
	for (int index = 0; index < 4; ++index)
	{
		short power = static_cast<short>(pow(10.0, 3 - index));
		short digit = value / power;
		value -= digit * power;
		if (digit > 0)
			fmt::format_to(result, L"{}", romanDigits[digit - 1][index]);
	}
	return fmt::to_string(result);
}
