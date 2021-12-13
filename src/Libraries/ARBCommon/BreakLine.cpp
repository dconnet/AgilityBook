/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief BreakLine and CSV handling.
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2012-07-25 Fix a CSV read problem with multiline continuation data.
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2010-10-30 Moved BreakLine from Globals.cpp, added CSV routines.
 */

#include "stdafx.h"
#include "ARBCommon/BreakLine.h"

#include "fmt/xchar.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


size_t BreakLine(wchar_t inSep, std::wstring const& inStr, std::vector<std::wstring>& outFields, bool inRemoveEmpties)
{
	outFields.clear();

	if (inStr.empty())
		return 0;

	size_t fld = 0;
	wchar_t const* pos = inStr.c_str();
	wchar_t const* npos = wcschr(pos, inSep);
	while (npos)
	{
		std::wstring str(pos, npos - pos);
		if (!(inRemoveEmpties && str.empty()))
			outFields.push_back(str);
		++fld;
		pos = npos + 1;
		npos = wcschr(pos, inSep);
	}
	if (pos)
	{
		std::wstring str(pos);
		if (!(inRemoveEmpties && str.empty()))
			outFields.push_back(str);
	}
	++fld;
	return fld;
}


/* Grammar copied from RFC4180
 *
 * The ABNF grammar [2] appears as follows:
 *
 * file = [header CRLF] record *(CRLF record) [CRLF]
 * header = name *(COMMA name)
 * record = field *(COMMA field)
 * name = field
 * field = (escaped / non-escaped)
 * escaped = DQUOTE *(TEXTDATA / COMMA / CR / LF / 2DQUOTE) DQUOTE
 * non-escaped = *TEXTDATA
 * COMMA = %x2C
 * CR = %x0D ;as per section 6.1 of RFC 2234 [2]
 * DQUOTE =  %x22 ;as per section 6.1 of RFC 2234 [2]
 * LF = %x0A ;as per section 6.1 of RFC 2234 [2]
 * CRLF = CR LF ;as per section 6.1 of RFC 2234 [2]
 * TEXTDATA =  %x20-21 / %x23-2B / %x2D-7E
 */

ReadStatus ReadCSV(
	wchar_t inSep,
	std::wstring inRecord,
	std::vector<std::wstring>& ioFields,
	bool bContinuation,
	std::wstring newLine)
{
	if (!bContinuation)
		ioFields.clear();
	ReadStatus status = ReadStatus::Ok;
	if (bContinuation && inRecord.empty())
	{
		ioFields[ioFields.size() - 1] += newLine;
		status = ReadStatus::NeedMore;
	}
	bool bAddEmpty = false;
	while (!inRecord.empty())
	{
		std::wstring str;
		std::wstring::size_type posSep = inRecord.find(inSep);
		if (bContinuation || L'"' == inRecord[0])
		{
			std::wstring::size_type posQuote = inRecord.find(L'"', bContinuation ? 0 : 1);
			if (std::wstring::npos == posQuote)
			{
				if (bContinuation)
					str = inRecord;
				else
					str = inRecord.substr(1);
				inRecord.clear();
				status = ReadStatus::NeedMore;
			}
			else
			{
				fmt::wmemory_buffer data;
				std::wstring::iterator iStr = inRecord.begin();
				if (!bContinuation)
					++iStr;
				bool bInQuote = true;
				for (; iStr != inRecord.end(); ++iStr)
				{
					if (!bInQuote && *iStr == inSep)
					{
						++iStr;
						break;
					}
					else if (*iStr == L'"')
					{
						// This is the last char.
						if (iStr + 1 == inRecord.end())
						{
							if (!bInQuote)
								return ReadStatus::Error;
							else
								bInQuote = false;
						}
						else
						{
							if (*(iStr + 1) == L'"')
							{
								fmt::format_to(std::back_inserter(data), L"{}", *iStr);
								++iStr;
							}
							else if (*(iStr + 1) == inSep)
								bInQuote = false;
							else
							{
								if (bInQuote)
									return ReadStatus::Error;
								fmt::format_to(std::back_inserter(data), L"{}", *iStr);
							}
						}
					}
					else
						fmt::format_to(std::back_inserter(data), L"{}", *iStr);
				}
				str = fmt::to_string(data);
				if (iStr == inRecord.end())
					inRecord.clear();
				else
					inRecord = std::wstring(iStr, inRecord.end());
			}
		}
		else
		{
			if (std::wstring::npos == posSep)
			{
				str = inRecord;
				inRecord.clear();
			}
			else
			{
				str = inRecord.substr(0, posSep);
				inRecord = inRecord.substr(posSep + 1);
				if (inRecord.empty())
					bAddEmpty = true;
			}
			// If there is a quote in the string,
			// the field itself must be quoted.
			if (std::wstring::npos != str.find(L'"'))
				return ReadStatus::Error;
		}
		if (bContinuation && 0 < ioFields.size())
			ioFields[ioFields.size() - 1] += newLine + str;
		else
			ioFields.push_back(str);
		bContinuation = false;
	}
	if (bAddEmpty)
		ioFields.push_back(std::wstring());
	return status;
}


std::wstring WriteCSV(wchar_t inSep, std::vector<std::wstring> const& inFields, bool includeQuote)
{
	size_t fld = 0;
	fmt::wmemory_buffer val;
	for (std::vector<std::wstring>::const_iterator i = inFields.begin(); i != inFields.end(); ++i, ++fld)
	{
		if (0 < fld)
			fmt::format_to(std::back_inserter(val), L"{}", inSep);
		fmt::format_to(std::back_inserter(val), L"{}", WriteCSVField(inSep, *i, includeQuote));
	}
	return fmt::to_string(val);
}


std::wstring WriteCSVField(wchar_t inSep, std::wstring const& inField, bool includeQuote)
{
	fmt::wmemory_buffer val;
	if (std::wstring::npos != inField.find(L'"') || std::wstring::npos != inField.find(L'\n')
		|| std::wstring::npos != inField.find(inSep))
	{
		std::wstring str(inField);
		fmt::format_to(std::back_inserter(val), L"{}", L"\"");
		while (!str.empty())
		{
			std::wstring::size_type pos = str.find(L'"');
			if (std::wstring::npos == pos)
			{
				fmt::format_to(std::back_inserter(val), L"{}", str);
				str.clear();
			}
			else
			{
				fmt::format_to(std::back_inserter(val), L"{}\"\"", str.substr(0, pos));
				str = str.substr(pos + 1);
			}
		}
		fmt::format_to(std::back_inserter(val), L"{}", L"\"");
	}
	else if (includeQuote)
	{
		fmt::format_to(std::back_inserter(val), L"\"{}\"", inField);
	}
	else
	{
		fmt::format_to(std::back_inserter(val), L"{}", inField);
	}
	return fmt::to_string(val);
}
