/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test tidy
 * @author David Connet
 *
 * Revision History
 * 2020-02-25 Created
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ConfigHandler.h"
#include "TestARB.h"

#include "ARBCommon/ARBUtils.h"
#include "fmt/printf.h"
#include "LibTidyHtml/LibTidyHtml.h"

#include <stdarg.h>

#if defined(__WXWINDOWS__)
#include <wx/stdpaths.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("LibTidy")
{
	SECTION("tidy")
	{
		if (!g_bMicroTest)
		{
			std::wstring datafile = GetDataFile();
			std::stringstream filedata;
			bool bOk = CConfigHandler::LoadWxFile(datafile, L"tidytest.xml", filedata);
			REQUIRE(bOk);

            std::string data(filedata.str());

			fmt::wmemory_buffer err;
            std::string debug;
			ElementNodePtr tidyTree = TidyHtmlData(data, err, &debug);
            REQUIRE(tidyTree);
			//if (!tidyTree)
			//{
			//	std::wstring msg = fmt::to_string(err);
			//	if (msg.empty())
			//		msg = fmt::format(_("IDS_ERR_PARSING_DATA").wx_str(), inAddress);
			//	wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE);
			//}
		}
	}
}
