/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteCalPlugin::New(wxT("name"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteCalPlugin_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteCalPlugin::New(wxT("name"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(DeleteCalPlugin_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameOtherPoints_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameOtherPoints::New(wxT("oldname"), wxT("newname"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameOtherPoints_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameOtherPoints::New(wxT("oldname"), wxT("newname"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(RenameOtherPoints_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteOtherPoints_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteOtherPoints::New(wxT("name"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteOtherPoints_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteOtherPoints::New(wxT("name"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(DeleteOtherPoints_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameVenue_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameVenue::New(wxT("oldname"), wxT("newname"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameVenue_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameVenue::New(wxT("oldname"), wxT("newname"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(RenameVenue_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteVenue_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteVenue::New(wxT("name"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteVenue_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteVenue::New(wxT("name"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(DeleteVenue_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameMultiQ_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameMultiQ::New(wxT("venue"), wxT("oldname"), wxT("newname"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameMultiQ_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameMultiQ::New(wxT("venue"), wxT("oldname"), wxT("newname"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(RenameMultiQ_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteMultiQ_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteMultiQ::New(wxT("venue"), wxT("name"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteMultiQ_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteMultiQ::New(wxT("venue"), wxT("name"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(DeleteMultiQ_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameDivision_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameDivision::New(wxT("venue"), wxT("oldname"), wxT("newname"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameDivision_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameDivision::New(wxT("venue"), wxT("oldname"), wxT("newname"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(RenameDivision_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteDivision_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteDivision::New(wxT("venue"), wxT("name"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteDivision_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteDivision::New(wxT("venue"), wxT("name"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(DeleteDivision_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameLevel_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameLevel::NewLevel(wxT("venue"), wxT("div"), wxT("oldname"), wxT("newname"));
			CHECK(NULL != obj.get());
			obj = ARBConfigActionRenameLevel::NewSubLevel(wxT("venue"), wxT("div"), wxT("level"), wxT("oldname"), wxT("newname"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameLevel_Clone)
	{
		if (!g_bMicroTest)
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
	}


	TEST(RenameLevel_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteLevel_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteLevel::NewLevel(wxT("venue"), wxT("div"), wxT("name"));
			CHECK(NULL != obj.get());
			obj = ARBConfigActionDeleteLevel::NewSubLevel(wxT("venue"), wxT("div"), wxT("level"), wxT("name"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteLevel_Clone)
	{
		if (!g_bMicroTest)
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
	}


	TEST(DeleteLevel_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameTitle_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameTitle::New(wxT("venue"), wxT("oldname"), wxT("newname"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameTitle_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameTitle::New(wxT("venue"), wxT("oldname"), wxT("newname"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(RenameTitle_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteTitle_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteTitle::New(wxT("venue"), wxT("div"), wxT("oldname"), wxT("newname"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteTitle_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteTitle::New(wxT("venue"), wxT("div"), wxT("oldname"), wxT("newname"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(DeleteTitle_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameEvent_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameEvent::New(wxT("venue"), wxT("oldname"), wxT("newname"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameEvent_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameEvent::New(wxT("venue"), wxT("oldname"), wxT("newname"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(RenameEvent_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteEvent_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteEvent::New(wxT("venue"), wxT("name"));
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteEvent_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteEvent::New(wxT("venue"), wxT("name"));
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());
		}
	}


	TEST(DeleteEvent_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//virtual bool Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(List_Load)
	{
		if (!g_bMicroTest)
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
	}


	TEST(List_Apply)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//int Apply(
			//		ARBConfig& ioConfig,
			//		ARBDogList* ioDogs,
			//		wxString& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}
}
