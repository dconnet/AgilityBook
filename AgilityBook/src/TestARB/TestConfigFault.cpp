/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfigFault and ARBConfigFaultList class.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBConfigFault.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


struct ConfigFaultData
{
	ElementNodePtr ConfigFault1; // v1.0
	ElementNodePtr ConfigFault2; // v2+
	ConfigFaultData();
};


ConfigFaultData::ConfigFaultData()
{
	ConfigFault1 = ElementNode::New(TREE_FAULTTYPE);
	ConfigFault1->AddAttrib(L"Name", L"Refusal");
	ConfigFault2 = ElementNode::New(TREE_FAULTTYPE);
	ConfigFault2->SetValue(L"Bar Down");
}


SUITE(TestConfigFault)
{
	TEST(New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault = ARBConfigFault::New();
			CHECK(!!fault.get());
		}
	}


	TEST(Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			fault1->SetName(L"Fault!");
			ARBConfigFaultPtr fault2 = fault1->Clone();
			CHECK(!!fault2.get());
			CHECK(fault1.get() != fault2.get());
			CHECK(*fault1 == *fault2);
			fault1->SetName(L"Here");
			CHECK(fault1->GetName() != fault2->GetName());
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			fault1->SetName(L"Fault!");
			ARBConfigFaultPtr fault2 = ARBConfigFault::New();
			CHECK(*fault1 != *fault2);
			*fault1 = *fault2;
			CHECK(*fault1 == *fault2);
		}
	}


	TEST(GenName)
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			fault1->SetName(L"Fault!");
			CHECK(L"Fault!" == fault1->GetGenericName());
		}
	}


	TEST_FIXTURE(ConfigFaultData, Load1)
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(fault1->Load(ConfigFault1, ARBVersion(1, 0), callback));
			std::wstring name = fault1->GetGenericName();
			CHECK(!name.empty());
		}
	}


	TEST_FIXTURE(ConfigFaultData, Load2)
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(fault1->Load(ConfigFault2, ARBVersion(2, 0), callback));
			std::wstring name = fault1->GetGenericName();
			CHECK(!name.empty());
		}
	}


	TEST_FIXTURE(ConfigFaultData, Load3)
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(!fault1->Load(ConfigFault2, ARBVersion(1, 0), callback));
			std::wstring name = fault1->GetGenericName();
			CHECK(name.empty());
		}
	}


	TEST_FIXTURE(ConfigFaultData, Save)
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			fault1->Load(ConfigFault2, ARBVersion(2, 0), callback);
			ElementNodePtr ele = ElementNode::New();
			CHECK(fault1->Save(ele));
		}
	}
}


SUITE(TestConfigFaultList)
{
	TEST_FIXTURE(ConfigFaultData, Load1)
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultList faultlist;
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(faultlist.Load(ConfigFault1, ARBVersion(1, 0), callback));
			CHECK(faultlist.Load(ConfigFault2, ARBVersion(2, 0), callback));
			ElementNodePtr ele = ElementNode::New(L"Doesnt matter");
			ele->SetValue(L"a fault");
			CHECK(!faultlist.Load(ele, ARBVersion(2, 0), callback));
			CHECK_EQUAL(2u, faultlist.size());
		}
	}


	TEST_FIXTURE(ConfigFaultData, Find)
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultList faultlist;
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(faultlist.Load(ConfigFault1, ARBVersion(1, 0), callback));
			CHECK(faultlist.Load(ConfigFault2, ARBVersion(2, 0), callback));
			CHECK(faultlist.FindFault(L"Refusal"));
			CHECK(!faultlist.FindFault(L"refusal"));
		}
	}


	TEST(AddDelete)
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultList faultlist;
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			CHECK(faultlist.AddFault(L"fault"));
			CHECK_EQUAL(1u, faultlist.size());
			CHECK(faultlist.AddFault(L"fault2"));
			CHECK_EQUAL(2u, faultlist.size());
			CHECK(faultlist.AddFault(L"fault"));
			CHECK_EQUAL(3u, faultlist.size());
			CHECK(faultlist.DeleteFault(L"fault"));
			CHECK_EQUAL(2u, faultlist.size());
		}
	}
}
