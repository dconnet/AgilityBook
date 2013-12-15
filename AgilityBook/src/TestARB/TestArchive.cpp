/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test LibArchive
 * @author David Connet
 *
 * Revision History
 * 2013-07-29 Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "LibArchive/LibArchive.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestArchive)
{
#ifndef __WXWINDOWS__
	static char const* FileData1 = "This is test1\nOk";
	static char const* FileData2 = "This is test2\r\nOk\r\n";
	static char const* FileData3 = "This is test3\r\nReplaced\r\n";

	static std::string CreateZip()
	{
		return std::string();
	}

	TEST(ReplaceFile)
	{
		if (!g_bMicroTest)
		{
			std::string file = CreateZip();
			CHECK(!file.empty());
			if (!file.empty())
			{
				CLibArchive archive(file);
				std::istringstream data(FileData3);
				CHECK(archive.ReplaceFile("test1.txt", data));
				_unlink(file.c_str());
			}
			TODO_TEST
			// See: http://docs.wxwidgets.org/stable/wx_wxarc.html#wxarcmodify
		}
	}


	TEST(ExtractFile)
	{
		//if (!g_bMicroTest)
		{
			std::string file = CreateZip();
			CHECK(!file.empty());

			if (!file.empty())
			{
				CLibArchive archive(file);

				{
					std::ostringstream data;
					CHECK(archive.ExtractFile("test1.txt", data));
					CHECK(data.str() == FileData1);
				}

				{
					std::ostringstream data;
					CHECK(archive.ExtractFile("test2.txt", data));
					CHECK(data.str() == FileData2);
				}

				_unlink(file.c_str());
			}
		}
	}
#endif
}
