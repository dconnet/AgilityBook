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
 * 2018-12-16 Convert to fmt.
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "TestARB.h"

#include "ARB/ARBConfigAction.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class ActionCallbackStop : public IConfigActionCallback
{
public:
	ActionCallbackStop()
	{
	}
	void PreDelete(std::wstring const& inMsg) override
	{
	}
	void PostDelete(std::wstring const& inMsg) const override
	{
	}
	bool CanContinue() const override
	{
		return false;
	}
};


class ActionCallbackContinue : public IConfigActionCallback
{
public:
	ActionCallbackContinue()
	{
	}
	void PreDelete(std::wstring const& inMsg) override
	{
	}
	void PostDelete(std::wstring const& inMsg) const override
	{
	}
	bool CanContinue() const override
	{
		return true;
	}
};


TEST_CASE("ConfigAction")
{
	SECTION("DeleteCalPlugin_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteCalPlugin::New(L"name");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("DeleteCalPlugin_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteCalPlugin::New(L"name");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("DeleteCalPlugin_Apply")
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


	SECTION("RenameOtherPoints_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameOtherPoints::New(0, L"oldname", L"newname");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("RenameOtherPoints_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameOtherPoints::New(0, L"oldname", L"newname");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("RenameOtherPoints_Apply")
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


	SECTION("DeleteOtherPoints_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteOtherPoints::New(0, L"name");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("DeleteOtherPoints_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteOtherPoints::New(0, L"name");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("DeleteOtherPoints_Apply")
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


	SECTION("RenameVenue_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameVenue::New(0, L"oldname", L"newname");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("RenameVenue_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameVenue::New(0, L"oldname", L"newname");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("RenameVenue_Apply")
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


	SECTION("DeleteVenue_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteVenue::New(0, L"name");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("DeleteVenue_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteVenue::New(0, L"name");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("DeleteVenue_Apply")
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


	SECTION("RenameMultiQ_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameMultiQ::New(0, L"venue", L"oldname", L"newname");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("RenameMultiQ_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameMultiQ::New(0, L"venue", L"oldname", L"newname");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("RenameMultiQ_Apply")
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


	SECTION("DeleteMultiQ_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteMultiQ::New(0, L"venue", L"name");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("DeleteMultiQ_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteMultiQ::New(0, L"venue", L"name");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("DeleteMultiQ_Apply")
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


	SECTION("RenameDivision_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameDivision::New(0, L"venue", L"oldname", L"newname");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("RenameDivision_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameDivision::New(0, L"venue", L"oldname", L"newname");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("RenameDivision_Apply")
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


	SECTION("DeleteDivision_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteDivision::New(0, L"venue", L"name");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("DeleteDivision_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteDivision::New(0, L"venue", L"name");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("DeleteDivision_Apply")
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


	SECTION("RenameLevel_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameLevel::NewLevel(0, L"venue", L"div", L"oldname", L"newname");
			REQUIRE(!!obj.get());
			obj = ARBConfigActionRenameLevel::NewSubLevel(0, L"venue", L"div", L"level", L"oldname", L"newname");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("RenameLevel_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameLevel::NewLevel(0, L"venue", L"div", L"oldname", L"newname");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());

			obj = ARBConfigActionRenameLevel::NewSubLevel(0, L"venue", L"div", L"level", L"oldname", L"newname");
			obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("RenameLevel_Apply")
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


	SECTION("DeleteLevel_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteLevel::NewLevel(0, L"venue", L"div", L"name");
			REQUIRE(!!obj.get());
			obj = ARBConfigActionDeleteLevel::NewSubLevel(0, L"venue", L"div", L"level", L"name");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("DeleteLevel_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteLevel::NewLevel(0, L"venue", L"div", L"name");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());

			obj = ARBConfigActionDeleteLevel::NewSubLevel(0, L"venue", L"div", L"level", L"name");
			obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("DeleteLevel_Apply")
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


	SECTION("RenameTitle_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameTitle::New(0, L"venue", L"oldname", L"newname");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("RenameTitle_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameTitle::New(0, L"venue", L"oldname", L"newname");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("RenameTitle_Apply")
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


	SECTION("DeleteTitle_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteTitle::New(0, L"venue", L"div", L"oldname", L"newname");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("DeleteTitle_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteTitle::New(0, L"venue", L"div", L"oldname", L"newname");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("DeleteTitle_Apply")
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


	SECTION("RenameEvent_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameEvent::New(0, L"venue", L"oldname", L"newname");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("RenameEvent_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionRenameEvent::New(0, L"venue", L"oldname", L"newname");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("RenameEvent_Apply")
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


	SECTION("DeleteEvent_New")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteEvent::New(0, L"venue", L"name");
			REQUIRE(!!obj.get());
		}
	}


	SECTION("DeleteEvent_Clone")
	{
		if (!g_bMicroTest)
		{
			ARBConfigActionPtr obj = ARBConfigActionDeleteEvent::New(0, L"venue", L"name");
			ARBConfigActionPtr obj2 = obj->Clone();
			REQUIRE(!!obj2.get());
			REQUIRE(obj.get() != obj2.get());
		}
	}


	SECTION("DeleteEvent_Apply")
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


	SECTION("List_Load")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr actions = CreateActionList();
			ARBConfigActionList lst;
			fmt::wmemory_buffer err;
			ARBErrorCallback callback(err);
			for (int i = 0; i < actions->GetElementCount(); ++i)
			{
				if (!actions->GetElementNode(i))
					continue;
				REQUIRE(lst.Load(actions->GetElementNode(i), ARBVersion(1, 0), callback));
			}
		}
	}


	SECTION("List_Apply")
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
