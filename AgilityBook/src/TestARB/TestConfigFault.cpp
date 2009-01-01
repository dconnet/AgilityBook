/*
 * Copyright © 2008-2009 David Connet. All Rights Reserved.
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
 * @brief Test ARBConfigFault and ARBConfigFaultList class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigFault.h"
#include "Element.h"


struct ConfigFaultData
{
	ElementNodePtr ConfigFault1; // v1.0
	ElementNodePtr ConfigFault2; // v2+
	ConfigFaultData();
};


ConfigFaultData::ConfigFaultData()
{
	ConfigFault1 = ElementNode::New(TREE_FAULTTYPE);
	ConfigFault1->AddAttrib(_T("Name"), _T("Refusal"));
	ConfigFault2 = ElementNode::New(TREE_FAULTTYPE);
	ConfigFault2->SetValue(_T("Bar Down"));
}


SUITE(TestConfigFault)
{
	TEST(New)
	{
		ARBConfigFaultPtr fault = ARBConfigFault::New();
		CHECK(NULL != fault.get());
	}


	TEST(Clone)
	{
		ARBConfigFaultPtr fault1 = ARBConfigFault::New();
		fault1->SetName(_T("Fault!"));
		ARBConfigFaultPtr fault2 = fault1->Clone();
		CHECK(NULL != fault2.get());
		CHECK(fault1.get() != fault2.get());
		CHECK(*fault1 == *fault2);
		fault1->SetName(_T("Here"));
		CHECK(fault1->GetName() != fault2->GetName());
	}


	TEST(OpEqual)
	{
		ARBConfigFaultPtr fault1 = ARBConfigFault::New();
		fault1->SetName(_T("Fault!"));
		ARBConfigFaultPtr fault2 = ARBConfigFault::New();
		CHECK(*fault1 != *fault2);
		*fault1 = *fault2;
		CHECK(*fault1 == *fault2);
	}


	TEST(GenName)
	{
		ARBConfigFaultPtr fault1 = ARBConfigFault::New();
		fault1->SetName(_T("Fault!"));
		CHECK(_T("Fault!") == fault1->GetGenericName());
	}


	TEST_FIXTURE(ConfigFaultData, Load1)
	{
		ARBConfigFaultPtr fault1 = ARBConfigFault::New();
		tstring errs;
		ARBErrorCallback callback(errs);
		CHECK(fault1->Load(ConfigFault1, ARBVersion(1, 0), callback));
		tstring name = fault1->GetGenericName();
		CHECK(!name.empty());
	}


	TEST_FIXTURE(ConfigFaultData, Load2)
	{
		ARBConfigFaultPtr fault1 = ARBConfigFault::New();
		tstring errs;
		ARBErrorCallback callback(errs);
		CHECK(fault1->Load(ConfigFault2, ARBVersion(2, 0), callback));
		tstring name = fault1->GetGenericName();
		CHECK(!name.empty());
	}


	TEST_FIXTURE(ConfigFaultData, Load3)
	{
		ARBConfigFaultPtr fault1 = ARBConfigFault::New();
		tstring errs;
		ARBErrorCallback callback(errs);
		CHECK(!fault1->Load(ConfigFault2, ARBVersion(1, 0), callback));
		tstring name = fault1->GetGenericName();
		CHECK(name.empty());
	}


	TEST_FIXTURE(ConfigFaultData, Save)
	{
		ARBConfigFaultPtr fault1 = ARBConfigFault::New();
		tstring errs;
		ARBErrorCallback callback(errs);
		fault1->Load(ConfigFault2, ARBVersion(2, 0), callback);
		ElementNodePtr ele = ElementNode::New();
		CHECK(fault1->Save(ele));
	}
}


SUITE(TestConfigFaultList)
{
	TEST_FIXTURE(ConfigFaultData, Load1)
	{
		ARBConfigFaultList faultlist;
		tstring errs;
		ARBErrorCallback callback(errs);
		CHECK(faultlist.Load(ConfigFault1, ARBVersion(1, 0), callback));
		CHECK(faultlist.Load(ConfigFault2, ARBVersion(2, 0), callback));
		ElementNodePtr ele = ElementNode::New(_T("Doesnt matter"));
		ele->SetValue(_T("a fault"));
		CHECK(!faultlist.Load(ele, ARBVersion(2, 0), callback));
		CHECK_EQUAL(2u, faultlist.size());
	}


	TEST_FIXTURE(ConfigFaultData, Find)
	{
		ARBConfigFaultList faultlist;
		tstring errs;
		ARBErrorCallback callback(errs);
		CHECK(faultlist.Load(ConfigFault1, ARBVersion(1, 0), callback));
		CHECK(faultlist.Load(ConfigFault2, ARBVersion(2, 0), callback));
		CHECK(faultlist.FindFault(_T("Refusal")));
		CHECK(!faultlist.FindFault(_T("refusal")));
	}


	TEST(AddDelete)
	{
		ARBConfigFaultList faultlist;
		tstring errs;
		ARBErrorCallback callback(errs);
		CHECK(faultlist.AddFault(_T("fault")));
		CHECK_EQUAL(1u, faultlist.size());
		CHECK(faultlist.AddFault(_T("fault2")));
		CHECK_EQUAL(2u, faultlist.size());
		CHECK(faultlist.AddFault(_T("fault")));
		CHECK_EQUAL(3u, faultlist.size());
		CHECK(faultlist.DeleteFault(_T("fault")));
		CHECK_EQUAL(2u, faultlist.size());
	}
}
