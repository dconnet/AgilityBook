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


FIXTURE(Config)


SETUP(Config)
{
	WIN_ASSERT_TRUE(CommonSetup());
}


TEARDOWN(Config)
{
	WIN_ASSERT_TRUE(CommonTeardown());
}


BEGIN_TESTF(Config_Equality, Config)
{
	ARBConfig config1, config2;
	WIN_ASSERT_EQUAL(config1, config2);
	config1.Default();
	WIN_ASSERT_NOT_EQUAL(config1, config2);
	ARBConfig config3(config1);
	WIN_ASSERT_EQUAL(config1, config3);
}
END_TESTF


BEGIN_TESTF(Config_Clear, Config)
{
	ARBConfig config1, config2;
	config1.Default();
	WIN_ASSERT_NOT_EQUAL(config1, config2);
	config1.clear();
	WIN_ASSERT_EQUAL(config1, config2);
}
END_TESTF


BEGIN_TESTF(Config_LoadFault, Config)
{
	ElementNodePtr data = ElementNode::New();
	ARBConfig config;
	tstring err;
	ARBErrorCallback callback(err);
	WIN_ASSERT_FALSE(config.LoadFault(ElementNodePtr(), ARBVersion(1,0), callback));
	WIN_ASSERT_FALSE(config.LoadFault(ElementNodePtr(), ARBVersion(2,0), callback));
	WIN_ASSERT_FALSE(config.LoadFault(data, ARBVersion(1,0), callback));
	WIN_ASSERT_FALSE(config.LoadFault(data, ARBVersion(2,0), callback));
	data->SetName(TREE_FAULTTYPE);
	data->SetValue(_T("A fault"));
	WIN_ASSERT_FALSE(config.LoadFault(data, ARBVersion(1,0), callback));
	WIN_ASSERT_TRUE(config.LoadFault(data, ARBVersion(2,0), callback));
	data->AddAttrib(_T("Name"), _T("A fault"));
	WIN_ASSERT_TRUE(config.LoadFault(data, ARBVersion(1,0), callback));
}
END_TESTF


BEGIN_TESTF(Config_LoadOtherPoints, Config)
{
	ElementNodePtr data = ElementNode::New();
	ARBConfig config;
	tstring err;
	ARBErrorCallback callback(err);
	WIN_ASSERT_FALSE(config.LoadOtherPoints(ElementNodePtr(), ARBVersion(1,0), callback));
	WIN_ASSERT_FALSE(config.LoadOtherPoints(data, ARBVersion(1,0), callback));
	data->SetName(TREE_OTHERPTS);
	WIN_ASSERT_FALSE(config.LoadOtherPoints(data, ARBVersion(1,0), callback));
	data->AddAttrib(ATTRIB_OTHERPTS_NAME, _T("Some Breed points"));
	WIN_ASSERT_FALSE(config.LoadOtherPoints(data, ARBVersion(1,0), callback));
	data->AddAttrib(ATTRIB_OTHERPTS_COUNT, _T("2"));
	WIN_ASSERT_FALSE(config.LoadOtherPoints(data, ARBVersion(1,0), callback));
	data->AddAttrib(ATTRIB_OTHERPTS_COUNT, _T("All"));
	WIN_ASSERT_TRUE(config.LoadOtherPoints(data, ARBVersion(1,0), callback));
}
END_TESTF


BEGIN_TESTF(Config_Load, Config)
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
		WIN_ASSERT_TRUE(LoadConfigFromTree(tree, config));
	}
}
END_TESTF


BEGIN_TESTF(Config_Save, Config)
{
	ARBConfig config;
	config.Default();
	ElementNodePtr tree = ElementNode::New();
	WIN_ASSERT_TRUE(config.Save(tree));
	WIN_ASSERT_EQUAL(1, tree->GetNodeCount(ElementNode::Element_Node));
	int idx = tree->FindElement(TREE_CONFIG);
	WIN_ASSERT_TRUE(idx >= 0);
}
END_TESTF


BEGIN_TESTF(Config_Default, Config)
{
	ARBConfig config;
	WIN_ASSERT_EQUAL(0u, config.GetCalSites().size());
	WIN_ASSERT_EQUAL(0u, config.GetActions().size());
	WIN_ASSERT_EQUAL(0u, config.GetFaults().size());
	WIN_ASSERT_EQUAL(0u, config.GetOtherPoints().size());
	WIN_ASSERT_EQUAL(0u, config.GetVenues().size());
	config.Default();
	WIN_ASSERT_EQUAL(0u, config.GetCalSites().size());
	WIN_ASSERT_EQUAL(80u, config.GetActions().size());
	WIN_ASSERT_EQUAL(0u, config.GetFaults().size());
	WIN_ASSERT_EQUAL(5u, config.GetOtherPoints().size());
	WIN_ASSERT_EQUAL(13u, config.GetVenues().size());
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("AAC")));
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("AKC")));
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("ASCA")));
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("CKC")));
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("CPE")));
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("DOCNA")));
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("FCI")));
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("NADAC")));
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("SCC")));
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("SSA")));
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("TDAA")));
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("UKC")));
	WIN_ASSERT_TRUE(config.GetVenues().FindVenue(_T("USDAA")));
}
END_TESTF


BEGIN_TESTF(Config_GetDTD, Config)
{
	std::string dtd = ARBConfig::GetDTD();
	WIN_ASSERT_NOT_ZERO(dtd.length());
	std::string dtd2 = ARBConfig::GetDTD(false);
	WIN_ASSERT_NOT_ZERO(dtd2.length());
	WIN_ASSERT_NOT_EQUAL(dtd, dtd2);
}
END_TESTF


BEGIN_TESTF(Config_GetTitleNiceName, Config)
{
	ARBConfig config;
	config.Default();
	tstring nice = config.GetTitleNiceName(_T("AKC"), _T("MX"));
	WIN_ASSERT_NOT_ZERO(nice.length());
}
END_TESTF


BEGIN_TESTF(Config_GetTitleCompleteName, Config)
{
	ARBConfig config;
	config.Default();
	ARBDogTitlePtr title = ARBDogTitle::New();
	title->SetVenue(_T("AKC"));
	title->SetName(_T("MX"), 1, false);
	tstring name1 = config.GetTitleCompleteName(title);
	WIN_ASSERT_NOT_ZERO(name1.length());
	tstring name2 = config.GetTitleCompleteName(title, false);
	WIN_ASSERT_NOT_ZERO(name2.length());
	WIN_ASSERT_NOT_EQUAL(name1, name2);
	tstring nice = config.GetTitleNiceName(_T("AKC"), _T("MX"));
	nice += _T(" [MX]");
	WIN_ASSERT_STRING_EQUAL(nice.c_str(), name2.c_str());
}
END_TESTF


BEGIN_TESTF(Config_Update, Config)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//bool Update(
	//		int indent,
	//		ARBConfig const& inConfigNew,
	//		otstringstream& ioInfo);
}
END_TESTF
