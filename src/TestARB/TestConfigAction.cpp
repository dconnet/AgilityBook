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
 * @brief Test ARBConfigAction class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigAction.h"
#include "Element.h"


FIXTURE(ConfigAction)


SETUP(ConfigAction)
{
	WIN_ASSERT_TRUE(CommonSetup());
}


TEARDOWN(ConfigAction)
{
	WIN_ASSERT_TRUE(CommonTeardown());
}


class ActionCallbackStop : public IConfigActionCallback
{
public:
	ActionCallbackStop() {}
	virtual void PreDelete(tstring const& inMsg) {}
	virtual void PostDelete(tstring const& inMsg) const {}
	virtual bool CanContinue() const	{return false;}
};


class ActionCallbackContinue : public IConfigActionCallback
{
public:
	ActionCallbackContinue() {}
	virtual void PreDelete(tstring const& inMsg) {}
	virtual void PostDelete(tstring const& inMsg) const {}
	virtual bool CanContinue() const	{return true;}
};


BEGIN_TEST(ConfigActionDeleteCalPlugin_New)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteCalPlugin::New(_T("name"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteCalPlugin_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteCalPlugin::New(_T("name"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteCalPlugin_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionRenameOtherPoints_New)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameOtherPoints::New(_T("oldname"), _T("newname"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameOtherPoints_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameOtherPoints::New(_T("oldname"), _T("newname"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameOtherPoints_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionDeleteOtherPoints_New)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteOtherPoints::New(_T("name"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteOtherPoints_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteOtherPoints::New(_T("name"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteOtherPoints_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionRenameVenue_New)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameVenue::New(_T("oldname"), _T("newname"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameVenue_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameVenue::New(_T("oldname"), _T("newname"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameVenue_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionDeleteVenue_New)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteVenue::New(_T("name"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteVenue_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteVenue::New(_T("name"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteVenue_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionRenameMultiQ_New)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameMultiQ::New(_T("venue"), _T("oldname"), _T("newname"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameMultiQ_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameMultiQ::New(_T("venue"), _T("oldname"), _T("newname"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameMultiQ_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionDeleteMultiQ_New)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteMultiQ::New(_T("venue"), _T("name"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteMultiQ_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteMultiQ::New(_T("venue"), _T("name"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteMultiQ_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionRenameDivision_New)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameDivision::New(_T("venue"), _T("oldname"), _T("newname"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameDivision_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameDivision::New(_T("venue"), _T("oldname"), _T("newname"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameDivision_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionDeleteDivision_New)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteDivision::New(_T("venue"), _T("name"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteDivision_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteDivision::New(_T("venue"), _T("name"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteDivision_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionRenameLevel_New)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameLevel::NewLevel(_T("venue"), _T("div"), _T("oldname"), _T("newname"));
	WIN_ASSERT_NOT_NULL(obj.get());
	obj = ARBConfigActionRenameLevel::NewSubLevel(_T("venue"), _T("div"), _T("level"), _T("oldname"), _T("newname"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameLevel_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameLevel::NewLevel(_T("venue"), _T("div"), _T("oldname"), _T("newname"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());

	obj = ARBConfigActionRenameLevel::NewSubLevel(_T("venue"), _T("div"), _T("level"), _T("oldname"), _T("newname"));
	obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameLevel_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionDeleteLevel_New)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteLevel::NewLevel(_T("venue"), _T("div"), _T("name"));
	WIN_ASSERT_NOT_NULL(obj.get());
	obj = ARBConfigActionDeleteLevel::NewSubLevel(_T("venue"), _T("div"), _T("level"), _T("name"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteLevel_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteLevel::NewLevel(_T("venue"), _T("div"), _T("name"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());

	obj = ARBConfigActionDeleteLevel::NewSubLevel(_T("venue"), _T("div"), _T("level"), _T("name"));
	obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteLevel_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionRenameTitle_New)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameTitle::New(_T("venue"), _T("oldname"), _T("newname"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameTitle_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameTitle::New(_T("venue"), _T("oldname"), _T("newname"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameTitle_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionDeleteTitle_New)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteTitle::New(_T("venue"), _T("div"), _T("oldname"), _T("newname"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteTitle_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteTitle::New(_T("venue"), _T("div"), _T("oldname"), _T("newname"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteTitle_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionRenameEvent_New)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameEvent::New(_T("venue"), _T("oldname"), _T("newname"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameEvent_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionRenameEvent::New(_T("venue"), _T("oldname"), _T("newname"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionRenameEvent_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TEST(ConfigActionDeleteEvent_New)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteEvent::New(_T("venue"), _T("name"));
	WIN_ASSERT_NOT_NULL(obj.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteEvent_Clone)
{
	ARBConfigActionPtr obj = ARBConfigActionDeleteEvent::New(_T("venue"), _T("name"));
	ARBConfigActionPtr obj2 = obj->Clone();
	WIN_ASSERT_NOT_NULL(obj2.get());
	WIN_ASSERT_NOT_EQUAL(obj.get(), obj2.get());
}
END_TEST


BEGIN_TEST(ConfigActionDeleteEvent_Apply)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//virtual bool Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TEST


BEGIN_TESTF(ConfigActionList_Load, ConfigAction)
{
	ElementNodePtr actions = CreateActionList();
	ARBConfigActionList lst;
	tstring err;
	ARBErrorCallback callback(err);
	for (int i = 0; i < actions->GetElementCount(); ++i)
	{
		if (!actions->GetElementNode(i))
			continue;
		WIN_ASSERT_TRUE(lst.Load(actions->GetElementNode(i), ARBVersion(1, 0), callback));
	}
}
END_TESTF


BEGIN_TESTF(ConfigActionList_Apply, ConfigAction)
{
	WIN_ASSERT_FAIL(_T("TODO: Write test"));
	//int Apply(
	//		ARBConfig& ioConfig,
	//		ARBDogList* ioDogs,
	//		otstringstream& ioInfo,
	//		IConfigActionCallback& ioCallBack) const;
}
END_TESTF
