/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief BreakLine and CSV handling.
 * @author David Connet
 *
 * Revision History
 * @li 2010-10-30 DRC Moved BreakLine from Globals.cpp, added CSV routines.
 */

#include "stdafx.h"
#include "BreakLine.h"

#include <wx/tokenzr.h>


size_t BreakLine(
		wxChar inSep,
		wxString inStr,
		std::vector<wxString>& outFields)
{
	outFields.clear();
	wxString::size_type pos = inStr.find(inSep);
	while (wxString::npos != pos)
	{
		outFields.push_back(inStr.substr(0, pos));
		inStr = inStr.substr(pos+1);
		pos = inStr.find(inSep);
	}
	outFields.push_back(inStr);
	return outFields.size();
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
		wxChar const inSep,
		wxString inRecord,
		std::vector<wxString>& ioFields,
		bool bContinuation)
{
	if (!bContinuation)
		ioFields.clear();
	ReadStatus status = DataOk;
	bool bAddEmpty = false;
	while (!inRecord.empty())
	{
		wxString str;
		wxString::size_type posSep = inRecord.find(inSep);
		if (bContinuation || wxT('"') == inRecord[0])
		{
			wxString::size_type posQuote = inRecord.find(wxT('"'), bContinuation ? 0 : 1);
			if (wxString::npos == posQuote)
			{
				str = inRecord;
				inRecord.clear();
				status = DataNeedMore;
			}
			else
			{
				wxString::iterator iStr = inRecord.begin();
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
					else if (*iStr == wxT('"'))
					{
						// This is the last char.
						if (iStr + 1 == inRecord.end())
						{
							if (!bInQuote)
								return DataError;
							else
								bInQuote = false;
						}
						else
						{
							if (*(iStr + 1) == wxT('"'))
							{
								str << *iStr;
								++iStr;
							}
							else if (*(iStr + 1) == inSep)
								bInQuote = false;
							else
							{
								if (bInQuote)
									return DataError;
								str << *iStr;
							}
						}
					}
					else
						str << *iStr;
				}
				if (iStr == inRecord.end())
					inRecord.clear();
				else
					inRecord = wxString(iStr, inRecord.end());
			}
			bContinuation = false;
		}
		else
		{
			if (wxString::npos == posSep)
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
			if (wxString::npos != str.find(wxT('"')))
				return DataError;
		}
		if (bContinuation && 0 < ioFields.size())
			ioFields[ioFields.size()-1] += str;
		else
			ioFields.push_back(str);
	}
	if (bAddEmpty)
		ioFields.push_back(wxString());
	return status;
}


wxString WriteCSV(
		wxChar const inSep,
		std::vector<wxString> const& inFields)
{
	size_t fld = 0;
	wxString val;
	for (std::vector<wxString>::const_iterator i = inFields.begin();
		i != inFields.end();
		++i, ++fld)
	{
		if (0 < fld)
			val << inSep;
		if (wxString::npos != i->find(wxT('"'))
		|| wxString::npos != i->find(wxT('\n'))
		|| wxString::npos != i->find(inSep))
		{
			wxString str(*i);
			val << wxT('"');
			while (!str.empty())
			{
				wxString::size_type pos = str.find(wxT('"'));
				if (wxString::npos == pos)
				{
					val << str;
					str.clear();
				}
				else
				{
					val << str.substr(0, pos);
					val << wxT("\"\"");
					str = str.substr(pos + 1);
				}
			}
			val << wxT('"');
		}
		else
			val << *i;
	}
	return val;
}
