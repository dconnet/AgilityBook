/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief Test ARBConfig class.
 * @author David Connet
 *
 * Revision History
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


SUITE(TestConfig)
{
	TEST(Equality)
	{
		ARBConfig config1, config2;
		CHECK(config1 == config2);
		CConfigHandler handler;
		config1.Default(&handler);
		CHECK(config1 != config2);
		ARBConfig config3(config1);
		CHECK(config1 == config3);
	}


	TEST(Clear)
	{
		ARBConfig config1, config2;
		CConfigHandler handler;
		config1.Default(&handler);
		CHECK(config1 != config2);
		config1.clear();
		CHECK(config1 == config2);
	}


	TEST(LoadFault)
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


	TEST(LoadOtherPoints)
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


	TEST(Load)
	{
		// See comments in TestARB.cpp about adding a new config.
		int configs[] = {
			IDR_XML_CONFIG08_V10_2,
			IDR_XML_CONFIG09_V11_0,
			IDR_XML_CONFIG12_V12_1,
			IDR_XML_CONFIG14_V12_2,
			IDR_XML_CONFIG19_V12_5,
			IDR_XML_CONFIG20_V12_6,
			IDR_XML_CONFIG21_V12_7,
			IDR_XML_CONFIG22_V12_7,
			IDR_XML_CONFIG23_V12_8,
			IDR_XML_CONFIG24_V12_8,
			IDR_XML_CONFIG25_V12_9,
			IDR_XML_CONFIG26_V12_9,
			IDR_XML_CONFIG27_V12_10,
			0
		};
		for (int i = 0; configs[i] != 0; ++i)
		{
			ElementNodePtr tree = LoadXMLData(configs[i]);
			// This probably means a config file is missing.
			CHECK(tree);
			ARBConfig config;
			CHECK(LoadConfigFromTree(tree, config));
		}
	}


	TEST(Save)
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


	TEST(Default)
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
		CHECK_EQUAL(81u, config.GetActions().size());
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
		CHECK(config.GetVenues().FindVenue(wxT("SSA")));
		CHECK(config.GetVenues().FindVenue(wxT("TDAA")));
		CHECK(config.GetVenues().FindVenue(wxT("UKC")));
		CHECK(config.GetVenues().FindVenue(wxT("USDAA")));
	}


	TEST(GetDTD)
	{
		CConfigHandler handler;
		std::string dtd = ARBConfig::GetDTD(&handler);
		CHECK(0 != dtd.length());
		std::string dtd2 = ARBConfig::GetDTD(&handler, false);
		CHECK(0 != dtd2.length());
		std::string::size_type pos = dtd2.find("\r\n");
		// Expectations: dtd should be in OS-specific line ending (if SVN is set right)
#ifdef WIN32
		CHECK(pos != std::string::npos);
#else
		CHECK(pos == std::string::npos);
#endif
		if (pos != std::string::npos)
			CHECK(dtd != dtd2);
		else
			CHECK(dtd == dtd2);
	}


	TEST(GetTitleNiceName)
	{
		ARBConfig config;
		CConfigHandler handler;
		config.Default(&handler);
		wxString nice = config.GetTitleNiceName(wxT("AKC"), wxT("MX"));
		CHECK(0 != nice.length());
	}


	TEST(GetTitleCompleteName)
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


	TEST(Update)
	{
		TODO_TEST
		//bool Update(
		//		int indent,
		//		ARBConfig const& inConfigNew,
		//		wxString& ioInfo);
	}
}
