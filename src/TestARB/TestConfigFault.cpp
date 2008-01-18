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


FIXTURE(ConfigFault)


namespace
{
	ElementNodePtr ConfigFault1; // v1.0
	ElementNodePtr ConfigFault2; // v2+
}


SETUP(ConfigFault)
{
	WIN_ASSERT_TRUE(CommonSetup());

	ConfigFault1 = ElementNode::New(TREE_FAULTTYPE);
	ConfigFault1->AddAttrib(_T("Name"), _T("Refusal"));
	ConfigFault2 = ElementNode::New(TREE_FAULTTYPE);
	ConfigFault2->SetValue(_T("Bar Down"));
}


TEARDOWN(ConfigFault)
{
	ConfigFault1.reset();
	ConfigFault2.reset();

	WIN_ASSERT_TRUE(CommonTeardown());
}


BEGIN_TEST(ConfigFault_New)
{
	ARBConfigFaultPtr fault = ARBConfigFault::New();
	WIN_ASSERT_NOT_NULL(fault.get());
}
END_TEST


BEGIN_TEST(ConfigFault_Clone)
{
	ARBConfigFaultPtr fault1 = ARBConfigFault::New();
	fault1->SetName(_T("Fault!"));
	ARBConfigFaultPtr fault2 = fault1->Clone();
	WIN_ASSERT_NOT_NULL(fault2.get());
	WIN_ASSERT_NOT_EQUAL(fault1.get(), fault2.get());
	WIN_ASSERT_EQUAL(*fault1, *fault2);
	fault1->SetName(_T("Here"));
	WIN_ASSERT_NOT_EQUAL(fault1->GetName(), fault2->GetName());
}
END_TEST


BEGIN_TEST(ConfigFault_OpEqual)
{
	ARBConfigFaultPtr fault1 = ARBConfigFault::New();
	fault1->SetName(_T("Fault!"));
	ARBConfigFaultPtr fault2 = ARBConfigFault::New();
	WIN_ASSERT_NOT_EQUAL(*fault1, *fault2);
	*fault1 = *fault2;
	WIN_ASSERT_EQUAL(*fault1, *fault2);
}
END_TEST


BEGIN_TEST(ConfigFault_GenName)
{
	ARBConfigFaultPtr fault1 = ARBConfigFault::New();
	fault1->SetName(_T("Fault!"));
	WIN_ASSERT_EQUAL(_T("Fault!"), fault1->GetGenericName());
}
END_TEST


BEGIN_TESTF(ConfigFault_Load1, ConfigFault)
{
	ARBConfigFaultPtr fault1 = ARBConfigFault::New();
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(fault1->Load(ConfigFault1, ARBVersion(1, 0), callback));
	tstring name = fault1->GetGenericName();
	WIN_ASSERT_FALSE(name.empty());
}
END_TESTF


BEGIN_TESTF(ConfigFault_Load2, ConfigFault)
{
	ARBConfigFaultPtr fault1 = ARBConfigFault::New();
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(fault1->Load(ConfigFault2, ARBVersion(2, 0), callback));
	tstring name = fault1->GetGenericName();
	WIN_ASSERT_FALSE(name.empty());
}
END_TESTF


BEGIN_TESTF(ConfigFault_Load3, ConfigFault)
{
	ARBConfigFaultPtr fault1 = ARBConfigFault::New();
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_FALSE(fault1->Load(ConfigFault2, ARBVersion(1, 0), callback));
	tstring name = fault1->GetGenericName();
	WIN_ASSERT_TRUE(name.empty());
}
END_TESTF


BEGIN_TESTF(ConfigFault_Save, ConfigFault)
{
	ARBConfigFaultPtr fault1 = ARBConfigFault::New();
	tstring errs;
	ARBErrorCallback callback(errs);
    fault1->Load(ConfigFault2, ARBVersion(2, 0), callback);
	ElementNodePtr ele = ElementNode::New();
	WIN_ASSERT_TRUE(fault1->Save(ele));
}
END_TESTF


BEGIN_TESTF(ConfigFaultList_Load1, ConfigFault)
{
	ARBConfigFaultList faultlist;
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(faultlist.Load(ConfigFault1, ARBVersion(1, 0), callback));
	WIN_ASSERT_TRUE(faultlist.Load(ConfigFault2, ARBVersion(2, 0), callback));
	ElementNodePtr ele = ElementNode::New(_T("Doesnt matter"));
	ele->SetValue(_T("a fault"));
	WIN_ASSERT_FALSE(faultlist.Load(ele, ARBVersion(2, 0), callback));
	WIN_ASSERT_EQUAL(2u, faultlist.size());
}
END_TESTF


BEGIN_TESTF(ConfigFaultList_Find, ConfigFault)
{
	ARBConfigFaultList faultlist;
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(faultlist.Load(ConfigFault1, ARBVersion(1, 0), callback));
	WIN_ASSERT_TRUE(faultlist.Load(ConfigFault2, ARBVersion(2, 0), callback));
	WIN_ASSERT_TRUE(faultlist.FindFault(_T("Refusal")));
	WIN_ASSERT_FALSE(faultlist.FindFault(_T("refusal")));
}
END_TESTF


BEGIN_TESTF(ConfigFaultList_AddDelete, ConfigFault)
{
	ARBConfigFaultList faultlist;
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(faultlist.AddFault(_T("fault")));
	WIN_ASSERT_EQUAL(1u, faultlist.size());
	WIN_ASSERT_TRUE(faultlist.AddFault(_T("fault2")));
	WIN_ASSERT_EQUAL(2u, faultlist.size());
	WIN_ASSERT_TRUE(faultlist.AddFault(_T("fault")));
	WIN_ASSERT_EQUAL(3u, faultlist.size());
	WIN_ASSERT_TRUE(faultlist.DeleteFault(_T("fault")));
	WIN_ASSERT_EQUAL(2u, faultlist.size());
}
END_TESTF
