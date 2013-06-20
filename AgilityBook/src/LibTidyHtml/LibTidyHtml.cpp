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

#include "stdafx.h"
#include "LibTidyHtml/LibTidyHtml.h"

#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "tidy/include/tidy.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#else
#include <errno.h>
#endif


ElementNodePtr TidyHtmlData(
		std::string const& data,
		std::wostringstream& err)
{
	ElementNodePtr tree;

	if (!data.empty())
	{
		TidyDoc tdoc = tidyCreate();
		tidyOptSetBool(tdoc, TidyXhtmlOut, yes);
		tidyOptSetBool(tdoc, TidyNumEntities, yes);
		if (0 > tidyParseString(tdoc, data.c_str()))
		{
			tidyRelease(tdoc);
			return tree;
		}
		if (0 > tidyCleanAndRepair(tdoc))
		{
			tidyRelease(tdoc);
			return tree;
		}
		tidyRunDiagnostics(tdoc);
		uint len = 0;
		if (tidySaveString(tdoc, NULL, &len) != -ENOMEM)
		{
			tidyRelease(tdoc);
			return tree;
		}
		char* pData = new char[len+1];
		strncpy(pData, data.c_str(), len);
		if (0 > tidySaveString(tdoc, pData, &len))
		{
			tidyRelease(tdoc);
			delete [] pData;
			return tree;
		}
		tidyRelease(tdoc);
		// Note, Tidy does not null terminate the buffer.
		pData[len] = 0;

#ifdef _DEBUG
//Test code to look at 'tidy'd data
//{
//std::string out(inAddress);
//out += ".out";
//wxFFile raw(StringUtil::stringWX(out), L"wb");
//raw.Write(pData, strlen(pData));
//}
#endif

		tree = ElementNode::New();
		if (!tree->LoadXML(pData, len, err))
		{
			tree.reset();
		}
		else
		{
#ifdef _DEBUG
//Test code to look at xml data
//{
//std::string out(inAddress);
//out += ".tree";
//wxFFileOutputStream raw(StringUtil::stringWX(out), L"wb");
//tree->SaveXML(raw);
//}
#endif
		}
		delete [] pData;
		pData = NULL;
	}
	return tree;
}
