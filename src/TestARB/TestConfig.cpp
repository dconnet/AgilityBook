/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfig class.
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2014-09-12 Add CKCSC.
 * 2013-01-13 Added more recurring tests for new style.
 * 2012-07-30 Added tests for checking recurring title styles.
 * 2011-01-22 Simplified how configs are added.
 * 2010-08-13 Updated for config 29.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ConfigHandler.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBStructure.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBDogTitle.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


// When adding a new config:
//  - Add entry here.
//  - Add entry in res/CompileDatListTest.txt
// RunARBTests.py will automatically pick up all XML files in ./res/
// Also, update "Default" test if venues/actions/etc changed.
const wchar_t* const gc_Configs[] =
{
	L"DefaultConfig.xml",
	L"Config08_v10_2.xml",
	L"Config09_v11_0.xml",
	L"Config12_v12_1.xml",
	L"Config14_v12_2.xml",
	L"Config19_v12_5.xml",
	L"Config20_v12_6.xml",
	L"Config21_v12_7.xml",
	L"Config22_v12_7.xml",
	L"Config23_v12_8.xml",
	L"Config24_v12_8.xml",
	L"Config25_v12_9.xml",
	L"Config26_v12_9.xml",
	L"Config27_v12_10.xml",
	L"Config28_v12_11.xml",
	L"Config29_v12_11.xml",
	L"Config30_v12_11.xml",
	L"Config31_v12_11.xml",
	L"Config32_v12_11.xml",
	L"Config33_v13_1.xml",
	L"Config34_v13_1.xml",
	L"Config35_v13_1.xml",
	L"Config36_v13_1.xml",
	L"Config37_v13_1.xml",
	L"Config38_v13_1.xml",
	L"Config39_v13_2.xml",
	L"Config40_v13_2.xml",
	L"Config41_v13_2.xml",
	L"Config42_v14_0.xml",
	L"Config43_v14_0.xml",
	L"Config44_v14_1.xml",
	L"Config45_v14_2.xml",
	L"Config46_v14_3.xml",
	L"Config47_v14_3.xml",
	L"Config48_v14_3.xml",
	L"Config49_v14_3.xml",
	L"Config50_v14_3.xml",
	L"Config51_v14_3.xml",
	L"Config52_v14_3.xml",
	L"Config53_v14_4.xml",
	L"Config54_v14_4.xml",
	L"Config55_v14_4.xml",
	L"Config56_v14_4.xml",
	L"Config57_v14_5.xml",
	L"Config58_v14_5.xml",
	L"Config59_v14_5.xml",
	L"Config60_v14_5.xml",
	L"Config61_v14_5.xml",
};
size_t gc_NumConfigs = sizeof(gc_Configs) / sizeof(gc_Configs[0]);


TEST_CASE("Config")
{
	SECTION("Equality")
	{
		if (!g_bMicroTest)
		{
			ARBConfig config1, config2;
			REQUIRE(config1 == config2);
			CConfigHandler handler;
			config1.Default(&handler);
			REQUIRE(config1 != config2);
			ARBConfig config3(config1);
			REQUIRE(config1 == config3);
		}
	}


	SECTION("Clear")
	{
		if (!g_bMicroTest)
		{
			ARBConfig config1, config2;
			CConfigHandler handler;
			config1.Default(&handler);
			REQUIRE(config1 != config2);
			config1.clear();
			REQUIRE(config1 == config2);
		}
	}


	SECTION("LoadFault")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr data = ElementNode::New();
			ARBConfig config;
			fmt::wmemory_buffer err;
			ARBErrorCallback callback(err);
			REQUIRE(!config.LoadFault(ElementNodePtr(), ARBVersion(1,0), callback));
			REQUIRE(!config.LoadFault(ElementNodePtr(), ARBVersion(2,0), callback));
			REQUIRE(!config.LoadFault(data, ARBVersion(1,0), callback));
			REQUIRE(!config.LoadFault(data, ARBVersion(2,0), callback));
			data->SetName(TREE_FAULTTYPE);
			data->SetValue(L"A fault");
			REQUIRE(!config.LoadFault(data, ARBVersion(1,0), callback));
			REQUIRE(config.LoadFault(data, ARBVersion(2,0), callback));
			data->AddAttrib(L"Name", L"A fault");
			REQUIRE(config.LoadFault(data, ARBVersion(1,0), callback));
		}
	}


	SECTION("LoadOtherPoints")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr data = ElementNode::New();
			ARBConfig config;
			fmt::wmemory_buffer err;
			ARBErrorCallback callback(err);
			REQUIRE(!config.LoadOtherPoints(ElementNodePtr(), ARBVersion(1,0), callback));
			REQUIRE(!config.LoadOtherPoints(data, ARBVersion(1,0), callback));
			data->SetName(TREE_OTHERPTS);
			REQUIRE(!config.LoadOtherPoints(data, ARBVersion(1,0), callback));
			data->AddAttrib(ATTRIB_OTHERPTS_NAME, L"Some Breed points");
			REQUIRE(!config.LoadOtherPoints(data, ARBVersion(1,0), callback));
			data->AddAttrib(ATTRIB_OTHERPTS_COUNT, L"2");
			REQUIRE(!config.LoadOtherPoints(data, ARBVersion(1,0), callback));
			data->AddAttrib(ATTRIB_OTHERPTS_COUNT, L"All");
			REQUIRE(config.LoadOtherPoints(data, ARBVersion(1,0), callback));
		}
	}


	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			for (size_t i = 1; i < gc_NumConfigs; ++i)
			{
				ElementNodePtr tree = LoadXMLData(i);
				// This probably means a config file is missing from res/CompileDatListTest.txt.
				REQUIRE(tree);
				ARBConfig config;
				REQUIRE(LoadConfigFromTree(tree, config));
			}
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			CConfigHandler handler;
			config.Default(&handler);
			ElementNodePtr tree = ElementNode::New();
			REQUIRE(config.Save(tree));
			REQUIRE(1 == tree->GetNodeCount(ARBElementType::Node));
			int idx = tree->FindElement(TREE_CONFIG);
			REQUIRE(idx >= 0);
		}
	}


	SECTION("Default")
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			REQUIRE(0u == config.GetCalSites().size());
			REQUIRE(0u == config.GetActions().size());
			REQUIRE(0u == config.GetFaults().size());
			REQUIRE(0u == config.GetOtherPoints().size());
			REQUIRE(0u == config.GetVenues().size());
			CConfigHandler handler;
			config.Default(&handler);
			REQUIRE(0u == config.GetCalSites().size());
			REQUIRE(206u == config.GetActions().size());
			REQUIRE(0u == config.GetFaults().size());
			REQUIRE(5u == config.GetOtherPoints().size());
			REQUIRE(16u == config.GetVenues().size());
			REQUIRE(config.GetVenues().FindVenue(L"AAC"));
			REQUIRE(config.GetVenues().FindVenue(L"AKC"));
			REQUIRE(config.GetVenues().FindVenue(L"ASCA"));
			REQUIRE(config.GetVenues().FindVenue(L"BHA"));
			REQUIRE(config.GetVenues().FindVenue(L"CKC"));
			REQUIRE(config.GetVenues().FindVenue(L"CKCSC"));
			REQUIRE(config.GetVenues().FindVenue(L"CPE"));
			REQUIRE(config.GetVenues().FindVenue(L"DOCNA"));
			REQUIRE(config.GetVenues().FindVenue(L"FCI"));
			REQUIRE(config.GetVenues().FindVenue(L"NADAC"));
			REQUIRE(config.GetVenues().FindVenue(L"SCC"));
			REQUIRE(config.GetVenues().FindVenue(L"TDAA"));
			REQUIRE(config.GetVenues().FindVenue(L"UKC"));
			REQUIRE(config.GetVenues().FindVenue(L"UKI"));
			REQUIRE(config.GetVenues().FindVenue(L"USDAA"));
			REQUIRE(config.GetVenues().FindVenue(L"VALOR"));
		}
	}


	SECTION("GetDTD")
	{
		if (!g_bMicroTest)
		{
			CConfigHandler handler;
			std::string dtd = ARBConfig::GetDTD(&handler);
			REQUIRE(0 != dtd.length());
			std::string::size_type pos = dtd.find("\r\n");
			// Expectations: dtd should be in OS-specific line ending (if SVN is set right)
			REQUIRE(pos == std::string::npos);
		}
	}


	SECTION("GetTitleNiceName")
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			CConfigHandler handler;
			config.Default(&handler);
			std::wstring nice = config.GetTitleNiceName(L"AKC", L"MX");
			REQUIRE(0 != nice.length());
		}
	}


	SECTION("GetCompleteName")
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			CConfigHandler handler;
			config.Default(&handler);

			static struct
			{
				wchar_t const* pVenue;
				wchar_t const* pTitle;
				wchar_t const* pResult;
			} sc_Titles[] = {
				{L"AKC", L"MX", L"[MX] Master Agility Excellent"},
				{L"AKC", L"FTC", L"[FTC+] FAST Century Title"},
				{L"NADAC", L"EAC", L"[EAC+] Elite Agility Certificate"},
				{nullptr, nullptr, nullptr},
			};

			for (size_t i = 0; sc_Titles[i].pVenue; ++i)
			{
				ARBConfigTitlePtr configTitle;
				REQUIRE(config.GetVenues().FindTitle(sc_Titles[i].pVenue, sc_Titles[i].pTitle, &configTitle));
				if (!configTitle)
					continue;

				std::wstring name = configTitle->GetCompleteName(-1);
				REQUIRE(name == sc_Titles[i].pResult);
			}
		}
	}


	SECTION("GetTitleCompleteName")
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			CConfigHandler handler;
			config.Default(&handler);

			ARBConfigTitlePtr configTitle;
			REQUIRE(config.GetVenues().FindTitle(L"AKC", L"MX", &configTitle));

			ARBDogTitlePtr title = ARBDogTitle::New();
			title->SetVenue(L"AKC");
			title->SetName(L"MX", 1, configTitle);
			// [MX] desc
			std::wstring name1 = config.GetTitleCompleteName(title);
			REQUIRE(0 != name1.length());
			// desc [MX]
			std::wstring name2 = config.GetTitleCompleteName(title, false);
			REQUIRE(0 != name2.length());
			REQUIRE(name1 != name2);
			std::wstring nice = config.GetTitleNiceName(L"AKC", L"MX");
			nice += L" [MX]";
			REQUIRE(nice == name2);
		}
	}


	SECTION("RecurringTitle")
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			CConfigHandler handler;
			config.Default(&handler);

			static struct
			{
				wchar_t const* pVenue;
				wchar_t const* pTitle;
				short repeat;
				wchar_t const* pResult;
			} sc_Titles[] = {
				{L"AKC", L"MACH", 1, L"MACH"},
				{L"AKC", L"MACH", 2, L"MACH2"},
				{L"AKC", L"FTC", 1, L"FTC1"},
				{L"AKC", L"FTC", 2, L"FTC2"},
				{L"ASCA", L"ATCH", 2, L"ATCH-II"},
				{L"NADAC", L"NAC", 1, L"NAC"},
				{L"NADAC", L"NAC", 2, L"NAC-400"},
				{L"NADAC", L"NAC", 3, L"NAC-600"},
				{L"NADAC", L"EAC", 1, L"EAC"},
				{L"NADAC", L"EAC", 2, L"EAC-600"},
				{L"NADAC", L"EAC", 3, L"EAC-800"},
				{L"NADAC", L"NJC", 1, L"NJC"},
				{L"NADAC", L"NJC", 2, L"NJC-200"},
				{L"NADAC", L"NJC", 3, L"NJC-300"},
				{L"NADAC", L"EJC", 1, L"EJC"},
				{L"NADAC", L"EJC", 2, L"EJC-300"},
				{L"NADAC", L"EJC", 3, L"EJC-400"},
				{nullptr, nullptr, 0, nullptr},
			};

			for (size_t i = 0; sc_Titles[i].pVenue; ++i)
			{
				ARBConfigTitlePtr configTitle;
				REQUIRE(config.GetVenues().FindTitle(sc_Titles[i].pVenue, sc_Titles[i].pTitle, &configTitle));

				ARBDogTitlePtr title = ARBDogTitle::New();
				title->SetVenue(sc_Titles[i].pVenue);

				title->SetName(sc_Titles[i].pTitle, sc_Titles[i].repeat, configTitle);
				std::wstring result = title->GetGenericName();
				REQUIRE(result == sc_Titles[i].pResult);
			}
		}
	}


	SECTION("Update")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool Update(
			//		int indent,
			//		ARBConfig const& inConfigNew,
			//		std::wstring& ioInfo);
		}
	}
}
