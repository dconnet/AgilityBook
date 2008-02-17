/*
 * Copyright © 2008 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 * @brief Test ARBConfig class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "resource.h"
#include "TestARB.h"

#include "ARBAgilityRecordBook.h"
#include "ARBStructure.h"
#include "ARBConfig.h"
#include "ARBDogTitle.h"
#include "Element.h"


SUITE(TestConfig)
{
	TEST(Equality)
	{
		ARBConfig config1, config2;
		CHECK(config1 == config2);
		config1.Default();
		CHECK(config1 != config2);
		ARBConfig config3(config1);
		CHECK(config1 == config3);
	}


	TEST(Clear)
	{
		ARBConfig config1, config2;
		config1.Default();
		CHECK(config1 != config2);
		config1.clear();
		CHECK(config1 == config2);
	}


	TEST(LoadFault)
	{
		ElementNodePtr data = ElementNode::New();
		ARBConfig config;
		tstring err;
		ARBErrorCallback callback(err);
		CHECK(!config.LoadFault(ElementNodePtr(), ARBVersion(1,0), callback));
		CHECK(!config.LoadFault(ElementNodePtr(), ARBVersion(2,0), callback));
		CHECK(!config.LoadFault(data, ARBVersion(1,0), callback));
		CHECK(!config.LoadFault(data, ARBVersion(2,0), callback));
		data->SetName(TREE_FAULTTYPE);
		data->SetValue(_T("A fault"));
		CHECK(!config.LoadFault(data, ARBVersion(1,0), callback));
		CHECK(config.LoadFault(data, ARBVersion(2,0), callback));
		data->AddAttrib(_T("Name"), _T("A fault"));
		CHECK(config.LoadFault(data, ARBVersion(1,0), callback));
	}


	TEST(LoadOtherPoints)
	{
		ElementNodePtr data = ElementNode::New();
		ARBConfig config;
		tstring err;
		ARBErrorCallback callback(err);
		CHECK(!config.LoadOtherPoints(ElementNodePtr(), ARBVersion(1,0), callback));
		CHECK(!config.LoadOtherPoints(data, ARBVersion(1,0), callback));
		data->SetName(TREE_OTHERPTS);
		CHECK(!config.LoadOtherPoints(data, ARBVersion(1,0), callback));
		data->AddAttrib(ATTRIB_OTHERPTS_NAME, _T("Some Breed points"));
		CHECK(!config.LoadOtherPoints(data, ARBVersion(1,0), callback));
		data->AddAttrib(ATTRIB_OTHERPTS_COUNT, _T("2"));
		CHECK(!config.LoadOtherPoints(data, ARBVersion(1,0), callback));
		data->AddAttrib(ATTRIB_OTHERPTS_COUNT, _T("All"));
		CHECK(config.LoadOtherPoints(data, ARBVersion(1,0), callback));
	}


	TEST(Load)
	{
		UINT configs[] = {
			IDR_XML_CONFIG08_V10_2,
			IDR_XML_CONFIG09_V11_0,
			IDR_XML_CONFIG12_V12_1,
			IDR_XML_CONFIG14_V12_2,
			IDR_XML_CONFIG19_V12_5,
			IDR_XML_CONFIG20_V12_6,
			0
		};
		for (int i = 0; configs[i] != 0; ++i)
		{
			ElementNodePtr tree = LoadXMLData(configs[i]);
			ARBConfig config;
			CHECK(LoadConfigFromTree(tree, config));
		}
	}


	TEST(Save)
	{
		ARBConfig config;
		config.Default();
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
		config.Default();
		CHECK_EQUAL(0u, config.GetCalSites().size());
		CHECK_EQUAL(80u, config.GetActions().size());
		CHECK_EQUAL(0u, config.GetFaults().size());
		CHECK_EQUAL(5u, config.GetOtherPoints().size());
		CHECK_EQUAL(13u, config.GetVenues().size());
		CHECK(config.GetVenues().FindVenue(_T("AAC")));
		CHECK(config.GetVenues().FindVenue(_T("AKC")));
		CHECK(config.GetVenues().FindVenue(_T("ASCA")));
		CHECK(config.GetVenues().FindVenue(_T("CKC")));
		CHECK(config.GetVenues().FindVenue(_T("CPE")));
		CHECK(config.GetVenues().FindVenue(_T("DOCNA")));
		CHECK(config.GetVenues().FindVenue(_T("FCI")));
		CHECK(config.GetVenues().FindVenue(_T("NADAC")));
		CHECK(config.GetVenues().FindVenue(_T("SCC")));
		CHECK(config.GetVenues().FindVenue(_T("SSA")));
		CHECK(config.GetVenues().FindVenue(_T("TDAA")));
		CHECK(config.GetVenues().FindVenue(_T("UKC")));
		CHECK(config.GetVenues().FindVenue(_T("USDAA")));
	}


	TEST(GetDTD)
	{
		std::string dtd = ARBConfig::GetDTD();
		CHECK(0 != dtd.length());
		std::string dtd2 = ARBConfig::GetDTD(false);
		CHECK(0 != dtd2.length());
		CHECK(dtd != dtd2);
	}


	TEST(GetTitleNiceName)
	{
		ARBConfig config;
		config.Default();
		tstring nice = config.GetTitleNiceName(_T("AKC"), _T("MX"));
		CHECK(0 != nice.length());
	}


	TEST(GetTitleCompleteName)
	{
		ARBConfig config;
		config.Default();
		ARBDogTitlePtr title = ARBDogTitle::New();
		title->SetVenue(_T("AKC"));
		title->SetName(_T("MX"), 1, false);
		tstring name1 = config.GetTitleCompleteName(title);
		CHECK(0 != name1.length());
		tstring name2 = config.GetTitleCompleteName(title, false);
		CHECK(0 != name2.length());
		CHECK(name1 != name2);
		tstring nice = config.GetTitleNiceName(_T("AKC"), _T("MX"));
		nice += _T(" [MX]");
		CHECK(nice == name2);
	}


	TEST(Update)
	{
		TODO_TEST
		//bool Update(
		//		int indent,
		//		ARBConfig const& inConfigNew,
		//		otstringstream& ioInfo);
	}
}
