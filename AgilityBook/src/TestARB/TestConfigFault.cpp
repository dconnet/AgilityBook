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
 * 2017-11-09 Convert from UnitTest++ to Catch
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


TEST_CASE("ConfigFault")
{
	ConfigFaultData data;

	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault = ARBConfigFault::New();
			REQUIRE(!!fault.get());
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			fault1->SetName(L"Fault!");
			ARBConfigFaultPtr fault2 = fault1->Clone();
			REQUIRE(!!fault2.get());
			REQUIRE(fault1.get() != fault2.get());
			REQUIRE(*fault1 == *fault2);
			fault1->SetName(L"Here");
			REQUIRE(fault1->GetName() != fault2->GetName());
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			fault1->SetName(L"Fault!");
			ARBConfigFaultPtr fault2 = ARBConfigFault::New();
			REQUIRE(*fault1 != *fault2);
			*fault1 = *fault2;
			REQUIRE(*fault1 == *fault2);
		}
	}


	SECTION("GenName")
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			fault1->SetName(L"Fault!");
			REQUIRE(L"Fault!" == fault1->GetGenericName());
		}
	}


	SECTION("Load1")
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			REQUIRE(fault1->Load(data.ConfigFault1, ARBVersion(1, 0), callback));
			std::wstring name = fault1->GetGenericName();
			REQUIRE(!name.empty());
		}
	}


	SECTION("Load2")
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			REQUIRE(fault1->Load(data.ConfigFault2, ARBVersion(2, 0), callback));
			std::wstring name = fault1->GetGenericName();
			REQUIRE(!name.empty());
		}
	}


	SECTION("Load3")
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			REQUIRE(!fault1->Load(data.ConfigFault2, ARBVersion(1, 0), callback));
			std::wstring name = fault1->GetGenericName();
			REQUIRE(name.empty());
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultPtr fault1 = ARBConfigFault::New();
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			fault1->Load(data.ConfigFault2, ARBVersion(2, 0), callback);
			ElementNodePtr ele = ElementNode::New();
			REQUIRE(fault1->Save(ele));
		}
	}
}


TEST_CASE("ConfigFaultList")
{
	ConfigFaultData data;

	SECTION("Load1")
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultList faultlist;
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			REQUIRE(faultlist.Load(data.ConfigFault1, ARBVersion(1, 0), callback));
			REQUIRE(faultlist.Load(data.ConfigFault2, ARBVersion(2, 0), callback));
			ElementNodePtr ele = ElementNode::New(L"Doesnt matter");
			ele->SetValue(L"a fault");
			REQUIRE(!faultlist.Load(ele, ARBVersion(2, 0), callback));
			REQUIRE(2u == faultlist.size());
		}
	}


	SECTION("Find")
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultList faultlist;
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			REQUIRE(faultlist.Load(data.ConfigFault1, ARBVersion(1, 0), callback));
			REQUIRE(faultlist.Load(data.ConfigFault2, ARBVersion(2, 0), callback));
			REQUIRE(faultlist.FindFault(L"Refusal"));
			REQUIRE(!faultlist.FindFault(L"refusal"));
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			ARBConfigFaultList faultlist;
			std::wostringstream errs;
			ARBErrorCallback callback(errs);
			REQUIRE(faultlist.AddFault(L"fault"));
			REQUIRE(1u == faultlist.size());
			REQUIRE(faultlist.AddFault(L"fault2"));
			REQUIRE(2u == faultlist.size());
			REQUIRE(faultlist.AddFault(L"fault"));
			REQUIRE(3u == faultlist.size());
			REQUIRE(faultlist.DeleteFault(L"fault"));
			REQUIRE(2u == faultlist.size());
		}
	}
}
