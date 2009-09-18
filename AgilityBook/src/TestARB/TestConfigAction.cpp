/*
 * Copyright (c) 2008-2009 David Connet. All Rights Reserved.
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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigAction.h"
#include "Element.h"


class ActionCallbackStop : public IConfigActionCallback
{
public:
	ActionCallbackStop() {}
	virtual void PreDelete(wxString const& inMsg) {}
	virtual void PostDelete(wxString const& inMsg) const {}
	virtual bool CanContinue() const	{return false;}
};


class ActionCallbackContinue : public IConfigActionCallback
{
public:
	ActionCallbackContinue() {}
	virtual void PreDelete(wxString const& inMsg) {}
	virtual void PostDelete(wxString const& inMsg) const {}
	virtual bool CanContinue() const	{return true;}
};


SUITE(TestConfigAction)
{
	TEST(DeleteCalPlugin_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteCalPlugin::New(wxT("name"));
		CHECK(NULL != obj.get());
	}


	TEST(DeleteCalPlugin_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteCalPlugin::New(wxT("name"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(DeleteCalPlugin_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(RenameOtherPoints_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameOtherPoints::New(wxT("oldname"), wxT("newname"));
		CHECK(NULL != obj.get());
	}


	TEST(RenameOtherPoints_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameOtherPoints::New(wxT("oldname"), wxT("newname"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(RenameOtherPoints_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(DeleteOtherPoints_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteOtherPoints::New(wxT("name"));
		CHECK(NULL != obj.get());
	}


	TEST(DeleteOtherPoints_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteOtherPoints::New(wxT("name"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(DeleteOtherPoints_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(RenameVenue_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameVenue::New(wxT("oldname"), wxT("newname"));
		CHECK(NULL != obj.get());
	}


	TEST(RenameVenue_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameVenue::New(wxT("oldname"), wxT("newname"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(RenameVenue_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(DeleteVenue_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteVenue::New(wxT("name"));
		CHECK(NULL != obj.get());
	}


	TEST(DeleteVenue_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteVenue::New(wxT("name"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(DeleteVenue_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(RenameMultiQ_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameMultiQ::New(wxT("venue"), wxT("oldname"), wxT("newname"));
		CHECK(NULL != obj.get());
	}


	TEST(RenameMultiQ_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameMultiQ::New(wxT("venue"), wxT("oldname"), wxT("newname"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(RenameMultiQ_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(DeleteMultiQ_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteMultiQ::New(wxT("venue"), wxT("name"));
		CHECK(NULL != obj.get());
	}


	TEST(DeleteMultiQ_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteMultiQ::New(wxT("venue"), wxT("name"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(DeleteMultiQ_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(RenameDivision_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameDivision::New(wxT("venue"), wxT("oldname"), wxT("newname"));
		CHECK(NULL != obj.get());
	}


	TEST(RenameDivision_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameDivision::New(wxT("venue"), wxT("oldname"), wxT("newname"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(RenameDivision_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(DeleteDivision_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteDivision::New(wxT("venue"), wxT("name"));
		CHECK(NULL != obj.get());
	}


	TEST(DeleteDivision_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteDivision::New(wxT("venue"), wxT("name"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(DeleteDivision_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(RenameLevel_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameLevel::NewLevel(wxT("venue"), wxT("div"), wxT("oldname"), wxT("newname"));
		CHECK(NULL != obj.get());
		obj = ARBConfigActionRenameLevel::NewSubLevel(wxT("venue"), wxT("div"), wxT("level"), wxT("oldname"), wxT("newname"));
		CHECK(NULL != obj.get());
	}


	TEST(RenameLevel_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameLevel::NewLevel(wxT("venue"), wxT("div"), wxT("oldname"), wxT("newname"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());

		obj = ARBConfigActionRenameLevel::NewSubLevel(wxT("venue"), wxT("div"), wxT("level"), wxT("oldname"), wxT("newname"));
		obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(RenameLevel_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(DeleteLevel_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteLevel::NewLevel(wxT("venue"), wxT("div"), wxT("name"));
		CHECK(NULL != obj.get());
		obj = ARBConfigActionDeleteLevel::NewSubLevel(wxT("venue"), wxT("div"), wxT("level"), wxT("name"));
		CHECK(NULL != obj.get());
	}


	TEST(DeleteLevel_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteLevel::NewLevel(wxT("venue"), wxT("div"), wxT("name"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());

		obj = ARBConfigActionDeleteLevel::NewSubLevel(wxT("venue"), wxT("div"), wxT("level"), wxT("name"));
		obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(DeleteLevel_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(RenameTitle_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameTitle::New(wxT("venue"), wxT("oldname"), wxT("newname"));
		CHECK(NULL != obj.get());
	}


	TEST(RenameTitle_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameTitle::New(wxT("venue"), wxT("oldname"), wxT("newname"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(RenameTitle_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(DeleteTitle_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteTitle::New(wxT("venue"), wxT("div"), wxT("oldname"), wxT("newname"));
		CHECK(NULL != obj.get());
	}


	TEST(DeleteTitle_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteTitle::New(wxT("venue"), wxT("div"), wxT("oldname"), wxT("newname"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(DeleteTitle_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(RenameEvent_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameEvent::New(wxT("venue"), wxT("oldname"), wxT("newname"));
		CHECK(NULL != obj.get());
	}


	TEST(RenameEvent_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionRenameEvent::New(wxT("venue"), wxT("oldname"), wxT("newname"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(RenameEvent_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(DeleteEvent_New)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteEvent::New(wxT("venue"), wxT("name"));
		CHECK(NULL != obj.get());
	}


	TEST(DeleteEvent_Clone)
	{
		ARBConfigActionPtr obj = ARBConfigActionDeleteEvent::New(wxT("venue"), wxT("name"));
		ARBConfigActionPtr obj2 = obj->Clone();
		CHECK(NULL != obj2.get());
		CHECK(obj.get() != obj2.get());
	}


	TEST(DeleteEvent_Apply)
	{
		TODO_TEST
		//virtual bool Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}


	TEST(List_Load)
	{
		ElementNodePtr actions = CreateActionList();
		ARBConfigActionList lst;
		wxString err;
		ARBErrorCallback callback(err);
		for (int i = 0; i < actions->GetElementCount(); ++i)
		{
			if (!actions->GetElementNode(i))
				continue;
			CHECK(lst.Load(actions->GetElementNode(i), ARBVersion(1, 0), callback));
		}
	}


	TEST(List_Apply)
	{
		TODO_TEST
		//int Apply(
		//		ARBConfig& ioConfig,
		//		ARBDogList* ioDogs,
		//		wxString& ioInfo,
		//		IConfigActionCallback& ioCallBack) const;
	}
}
