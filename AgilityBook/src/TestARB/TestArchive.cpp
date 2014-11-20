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

#include "ARBCommon/StringUtil.h"
#include "LibArchive/LibArchive.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


SUITE(TestArchive)
{
	static char const* FileData1 = "This is test1\r\nOk";
	static char const* FileData2 = "This is test2\r\nOk\r\n";

	TEST(ExtractFileFromRes)
	{
		if (!g_bMicroTest)
		{
			CLibArchive archive(L"TestArchive.zip");

			{
				std::ostringstream data;
#ifdef WIN32
				CHECK(archive.ExtractFile(L"test1.txt", data));
				CHECK(data.str() == FileData1);
#else
				CHECK(!archive.ExtractFile(L"test1.txt", data));
#endif
			}

			{
				std::ostringstream data;
#ifdef WIN32
				CHECK(archive.ExtractFile(L"test2.txt", data));
				CHECK(data.str() == FileData2);
#else
				CHECK(!archive.ExtractFile(L"test2.txt", data));
#endif
			}
		}
	}

#ifndef __WXWINDOWS__
	static char const* FileData3 = "This is test3\r\nReplaced\r\n";

	static std::wstring CreateZip()
	{
		return std::wstring();
	}

	TEST(ReplaceFile)
	{
		if (!g_bMicroTest)
		{
			std::wstring file = CreateZip();
			CHECK(!file.empty());
			if (!file.empty())
			{
				CLibArchive archive(file);
				std::istringstream data(FileData3);
				CHECK(archive.ReplaceFile(L"test1.txt", data));
				_unlink(StringUtil::stringA(file).c_str());
			}
			TODO_TEST
			// See: http://docs.wxwidgets.org/stable/wx_wxarc.html#wxarcmodify
		}
	}


	TEST(ExtractFile)
	{
		//if (!g_bMicroTest)
		{
			std::wstring file = CreateZip();
			CHECK(!file.empty());

			if (!file.empty())
			{
				CLibArchive archive(file);

				{
					std::ostringstream data;
					CHECK(archive.ExtractFile(L"test1.txt", data));
					CHECK(data.str() == FileData1);
				}

				{
					std::ostringstream data;
					CHECK(archive.ExtractFile(L"test2.txt", data));
					CHECK(data.str() == FileData2);
				}

				_unlink(StringUtil::stringA(file).c_str());
			}
		}
	}
#endif
}
