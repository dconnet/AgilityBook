/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBConfigAction class.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBConfigAction.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class ActionCallbackStop : public IConfigActionCallback
{
public:
	ActionCallbackStop() {}
	virtual void PreDelete(std::wstring const& inMsg) {}
	virtual void PostDelete(std::wstring const& inMsg) const {}
	virtual bool CanContinue() const	{return false;}
};


class ActionCallbackContinue : public IConfigActionCallback
{
public:
	ActionCallbackContinue() {}
	virtual void PreDelete(std::wstring const& inMsg) {}
	virtual void PostDelete(std::wstring const& inMsg) const {}
	virtual bool CanContinue() const	{return true;}
};


SUITE(TestConfigAction)
{
	TEST(DeleteCalPlugin_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteCalPlugin::New(L"name");
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteCalPlugin_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteCalPlugin::New(L"name");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameOtherPoints_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameOtherPoints::New(0, L"oldname", L"newname");
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameOtherPoints_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameOtherPoints::New(0, L"oldname", L"newname");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteOtherPoints_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteOtherPoints::New(0, L"name");
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteOtherPoints_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteOtherPoints::New(0, L"name");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameVenue_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameVenue::New(0, L"oldname", L"newname");
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameVenue_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameVenue::New(0, L"oldname", L"newname");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteVenue_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteVenue::New(0, L"name");
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteVenue_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteVenue::New(0, L"name");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameMultiQ_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameMultiQ::New(0, L"venue", L"oldname", L"newname");
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameMultiQ_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameMultiQ::New(0, L"venue", L"oldname", L"newname");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteMultiQ_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteMultiQ::New(0, L"venue", L"name");
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteMultiQ_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteMultiQ::New(0, L"venue", L"name");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameDivision_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameDivision::New(0, L"venue", L"oldname", L"newname");
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameDivision_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameDivision::New(0, L"venue", L"oldname", L"newname");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteDivision_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteDivision::New(0, L"venue", L"name");
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteDivision_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteDivision::New(0, L"venue", L"name");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameLevel_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameLevel::NewLevel(0, L"venue", L"div", L"oldname", L"newname");
			CHECK(NULL != obj.get());
			obj = ARBConfigActionRenameLevel::NewSubLevel(0, L"venue", L"div", L"level", L"oldname", L"newname");
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameLevel_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameLevel::NewLevel(0, L"venue", L"div", L"oldname", L"newname");
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());

			obj = ARBConfigActionRenameLevel::NewSubLevel(0, L"venue", L"div", L"level", L"oldname", L"newname");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteLevel_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteLevel::NewLevel(0, L"venue", L"div", L"name");
			CHECK(NULL != obj.get());
			obj = ARBConfigActionDeleteLevel::NewSubLevel(0, L"venue", L"div", L"level", L"name");
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteLevel_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteLevel::NewLevel(0, L"venue", L"div", L"name");
			ARBConfigActionPtr obj2 = obj->Clone();
			CHECK(NULL != obj2.get());
			CHECK(obj.get() != obj2.get());

			obj = ARBConfigActionDeleteLevel::NewSubLevel(0, L"venue", L"div", L"level", L"name");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameTitle_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameTitle::New(0, L"venue", L"oldname", L"newname");
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameTitle_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameTitle::New(0, L"venue", L"oldname", L"newname");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteTitle_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteTitle::New(0, L"venue", L"div", L"oldname", L"newname");
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteTitle_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteTitle::New(0, L"venue", L"div", L"oldname", L"newname");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(RenameEvent_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameEvent::New(0, L"venue", L"oldname", L"newname");
			CHECK(NULL != obj.get());
		}
	}


	TEST(RenameEvent_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameEvent::New(0, L"venue", L"oldname", L"newname");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(DeleteEvent_New)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteEvent::New(0, L"venue", L"name");
			CHECK(NULL != obj.get());
		}
	}


	TEST(DeleteEvent_Clone)
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteEvent::New(0, L"venue", L"name");
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}


	TEST(List_Load)
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr actions = CreateActionList();
			ARBConfigActionList lst;
			std::wostringstream err;
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
			//		std::wstring& ioInfo,
			//		IConfigActionCallback& ioCallBack) const;
		}
	}
}
