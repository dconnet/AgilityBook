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
 * @li 2011-01-22 DRC Simplified how configs are added.
 * @li 2010-08-13 DRC Updated for config 29.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBAgilityRecordBook.h"
#include "ARBStructure.h"
#include "ARBConfig.h"
#include "ARBDogTitle.h"
#include "ConfigHandler.h"
#include "Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


// When adding a new config:
//  - Add entry here. That's it.
// RunARBTests.py will automatically pick up all XML files in ./res/
// Also, update "Default" test if venues/actions/etc changed.
const wxChar* const gc_Configs[] =
{
	wxT("DefaultConfig.xml"),
	wxT("Config08_v10_2.xml"),
	wxT("Config09_v11_0.xml"),
	wxT("Config12_v12_1.xml"),
	wxT("Config14_v12_2.xml"),
	wxT("Config19_v12_5.xml"),
	wxT("Config20_v12_6.xml"),
	wxT("Config21_v12_7.xml"),
	wxT("Config22_v12_7.xml"),
	wxT("Config23_v12_8.xml"),
	wxT("Config24_v12_8.xml"),
	wxT("Config25_v12_9.xml"),
	wxT("Config26_v12_9.xml"),
	wxT("Config27_v12_10.xml"),
	wxT("Config28_v12_11.xml"),
	wxT("Config29_v12_11.xml"),
	wxT("Config30_v12_11.xml"),
	wxT("Config31_v12_11.xml"),
	wxT("Config32_v12_11.xml"),
	wxT("Config33_v13_1.xml"),
	wxT("Config34_v13_1.xml"),
	wxT("Config35_v13_1.xml"),
};
size_t gc_NumConfigs = sizeof(gc_Configs) / sizeof(gc_Configs[0]);


SUITE(TestConfig)
{
	TEST(Equality)
	{
		if (!g_bMicroTest)
		{
			ARBConfig config1, config2;
			CHECK(config1 == config2);
			CConfigHandler handler;
			config1.Default(&handler);
			CHECK(config1 != config2);
			ARBConfig config3(config1);
			CHECK(config1 == config3);
		}
	}


	TEST(Clear)
	{
		if (!g_bMicroTest)
		{
			ARBConfig config1, config2;
			CConfigHandler handler;
			config1.Default(&handler);
			CHECK(config1 != config2);
			config1.clear();
			CHECK(config1 == config2);
		}
	}


	TEST(LoadFault)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr data = ElementNode::New();
			ARBConfig config;
			wxString err;
			ARBErrorCallback callback(err);
			CHECK(!config.LoadFault(ElementNodePtr(), ARBVersion(1,0), callback));
			CHECK(!config.LoadFault(ElementNodePtr(), ARBVersion(2,0), callback));
			CHECK(!config.LoadFault(data, ARBVersion(1,0), callback));
			CHECK(!config.LoadFault(data, ARBVersion(2,0), callback));
			data->SetName(TREE_FAULTTYPE);
			data->SetValue(wxT("A fault"));
			CHECK(!config.LoadFault(data, ARBVersion(1,0), callback));
			CHECK(config.LoadFault(data, ARBVersion(2,0), callback));
			data->AddAttrib(wxT("Name"), wxT("A fault"));
			CHECK(config.LoadFault(data, ARBVersion(1,0), callback));
		}
	}


	TEST(LoadOtherPoints)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr data = ElementNode::New();
			ARBConfig config;
			wxString err;
			ARBErrorCallback callback(err);
			CHECK(!config.LoadOtherPoints(ElementNodePtr(), ARBVersion(1,0), callback));
			CHECK(!config.LoadOtherPoints(data, ARBVersion(1,0), callback));
			data->SetName(TREE_OTHERPTS);
			CHECK(!config.LoadOtherPoints(data, ARBVersion(1,0), callback));
			data->AddAttrib(ATTRIB_OTHERPTS_NAME, wxT("Some Breed points"));
			CHECK(!config.LoadOtherPoints(data, ARBVersion(1,0), callback));
			data->AddAttrib(ATTRIB_OTHERPTS_COUNT, wxT("2"));
			CHECK(!config.LoadOtherPoints(data, ARBVersion(1,0), callback));
			data->AddAttrib(ATTRIB_OTHERPTS_COUNT, wxT("All"));
			CHECK(config.LoadOtherPoints(data, ARBVersion(1,0), callback));
		}
	}


	TEST(Load)
	{
		if (!g_bMicroTest)
		{
			for (size_t i = 1; i < gc_NumConfigs; ++i)
			{
				ElementNodePtr tree = LoadXMLData(i);
				// This probably means a config file is missing.
				CHECK(tree);
				ARBConfig config;
				CHECK(LoadConfigFromTree(tree, config));
			}
		}
	}


	TEST(Save)
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			CConfigHandler handler;
			config.Default(&handler);
			ElementNodePtr tree = ElementNode::New();
			CHECK(config.Save(tree));
			CHECK_EQUAL(1, tree->GetNodeCount(ElementNode::Element_Node));
			int idx = tree->FindElement(TREE_CONFIG);
			CHECK(idx >= 0);
		}
	}


	TEST(Default)
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			CHECK_EQUAL(0u, config.GetCalSites().size());
			CHECK_EQUAL(0u, config.GetActions().size());
			CHECK_EQUAL(0u, config.GetFaults().size());
			CHECK_EQUAL(0u, config.GetOtherPoints().size());
			CHECK_EQUAL(0u, config.GetVenues().size());
			CConfigHandler handler;
			config.Default(&handler);
			CHECK_EQUAL(0u, config.GetCalSites().size());
			CHECK_EQUAL(138u, config.GetActions().size());
			CHECK_EQUAL(0u, config.GetFaults().size());
			CHECK_EQUAL(5u, config.GetOtherPoints().size());
			CHECK_EQUAL(13u, config.GetVenues().size());
			CHECK(config.GetVenues().FindVenue(wxT("AAC")));
			CHECK(config.GetVenues().FindVenue(wxT("AKC")));
			CHECK(config.GetVenues().FindVenue(wxT("ASCA")));
			CHECK(config.GetVenues().FindVenue(wxT("CKC")));
			CHECK(config.GetVenues().FindVenue(wxT("CPE")));
			CHECK(config.GetVenues().FindVenue(wxT("DOCNA")));
			CHECK(config.GetVenues().FindVenue(wxT("FCI")));
			CHECK(config.GetVenues().FindVenue(wxT("NADAC")));
			CHECK(config.GetVenues().FindVenue(wxT("SCC")));
			CHECK(config.GetVenues().FindVenue(wxT("TDAA")));
			CHECK(config.GetVenues().FindVenue(wxT("UKC")));
			CHECK(config.GetVenues().FindVenue(wxT("UKI")));
			CHECK(config.GetVenues().FindVenue(wxT("USDAA")));
		}
	}


	TEST(GetDTD)
	{
		if (!g_bMicroTest)
		{
			CConfigHandler handler;
			std::string dtd = ARBConfig::GetDTD(&handler);
			CHECK(0 != dtd.length());
			std::string dtd2 = ARBConfig::GetDTD(&handler, false);
			CHECK(0 != dtd2.length());
			std::string::size_type pos = dtd2.find("\r\n");
			// Expectations: dtd should be in OS-specific line ending (if SVN is set right)
#ifdef __WXMSW__
			CHECK(pos != std::string::npos);
#else
			CHECK(pos == std::string::npos);
#endif
			if (pos != std::string::npos)
				CHECK(dtd != dtd2);
			else
				CHECK(dtd == dtd2);
		}
	}


	TEST(GetTitleNiceName)
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			CConfigHandler handler;
			config.Default(&handler);
			wxString nice = config.GetTitleNiceName(wxT("AKC"), wxT("MX"));
			CHECK(0 != nice.length());
		}
	}


	TEST(GetTitleCompleteName)
	{
		if (!g_bMicroTest)
		{
			ARBConfig config;
			CConfigHandler handler;
			config.Default(&handler);
			ARBDogTitlePtr title = ARBDogTitle::New();
			title->SetVenue(wxT("AKC"));
			title->SetName(wxT("MX"), 1, false, eTitleNumber);
			wxString name1 = config.GetTitleCompleteName(title);
			CHECK(0 != name1.length());
			wxString name2 = config.GetTitleCompleteName(title, false);
			CHECK(0 != name2.length());
			CHECK(name1 != name2);
			wxString nice = config.GetTitleNiceName(wxT("AKC"), wxT("MX"));
			nice += wxT(" [MX]");
			CHECK(nice == name2);
		}
	}


	TEST(Update)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//bool Update(
			//		int indent,
			//		ARBConfig const& inConfigNew,
			//		wxString& ioInfo);
		}
	}
}
