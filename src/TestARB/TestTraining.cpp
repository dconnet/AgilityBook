/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBTraining class
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-13 Created
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBTraining.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


struct TestTrainingData
{
	ElementNodePtr TrainingData;
	TestTrainingData();
};


TestTrainingData::TestTrainingData()
{
	TrainingData = ElementNode::New(TREE_TRAINING);
	TrainingData->SetValue(L"These are some notes");
	TrainingData->AddAttrib(ATTRIB_TRAINING_DATE, L"2006-3-4");
	TrainingData->AddAttrib(ATTRIB_TRAINING_NAME, L"A Name");
	TrainingData->AddAttrib(ATTRIB_TRAINING_SUBNAME, L"SubName1");
}


static void CreateTrainingList(ARBTrainingList& trainlist)
{
	ARBTrainingPtr train = ARBTraining::New();
	train->SetDate(ARBDate(2006, 9, 4));
	train->SetName(L"Test1");
	train->SetSubName(L"SubN");
	trainlist.AddTraining(train);
	train = ARBTraining::New();
	train->SetDate(ARBDate(2005, 3, 26));
	train->SetName(L"Test2");
	train->SetSubName(L"Sub3N");
	trainlist.AddTraining(train);
	train = ARBTraining::New();
	train->SetDate(ARBDate(2005, 3, 26));
	train->SetName(L"Test2");
	train->SetSubName(L"Sub2N");
	trainlist.AddTraining(train);
	trainlist.sort();
}


TEST_CASE("Training")
{
	TestTrainingData data;

	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingPtr train = ARBTraining::New();
			REQUIRE(!!train.get());
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingPtr train = ARBTraining::New();
			train->SetDate(ARBDate(2007, 9, 1));
			ARBTrainingPtr train2 = train->Clone();
			REQUIRE(!!train2.get());
			REQUIRE(train.get() != train2.get());
			REQUIRE(*train == *train2);
			REQUIRE(train->GetDate() == train2->GetDate());
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingPtr train1 = ARBTraining::New();
			train1->SetDate(ARBDate(2007, 9, 1));
			ARBTrainingPtr train2 = ARBTraining::New();
			REQUIRE(*train1 != *train2);
			*train1 = *train2;
			REQUIRE(*train1 == *train2);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingPtr train = ARBTraining::New();
			fmt::wmemory_buffer errs;
			ARBErrorCallback callback(errs);
			REQUIRE(train->Load(data.TrainingData, ARBVersion(2, 0), callback));
			ARBTrainingPtr train2 = train->Clone();
			REQUIRE(train.get() != train2.get());
			REQUIRE(*train == *train2);
			REQUIRE(train->GetDate() == train2->GetDate());
			ARBDate n = ARBDate::Today();
			train2->SetDate(n);
			REQUIRE(*train < *train2);
			REQUIRE(*train != *train2);
			REQUIRE(!(*train > *train2));
		}
	}


	SECTION("GenName")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingPtr train = ARBTraining::New();
			train->SetDate(ARBDate(2007, 3, 13));
			REQUIRE(L"3/13/2007" == train->GetGenericName());
		}
	}

	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingPtr train = ARBTraining::New();
			fmt::wmemory_buffer errs;
			ARBErrorCallback callback(errs);
			REQUIRE(train->Load(data.TrainingData, ARBVersion(1, 0), callback));
			std::wstring name = train->GetGenericName();
			REQUIRE(!name.empty());
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingPtr train = ARBTraining::New();
			fmt::wmemory_buffer errs;
			ARBErrorCallback callback(errs);
			REQUIRE(train->Load(data.TrainingData, ARBVersion(2, 0), callback));
			ElementNodePtr ele = ElementNode::New();
			REQUIRE(train->Save(ele));
		}
	}
}


TEST_CASE("TrainingList")
{
	TestTrainingData data;

	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingList train;
			fmt::wmemory_buffer errs;
			ARBErrorCallback callback(errs);
			REQUIRE(train.Load(data.TrainingData, ARBVersion(1, 0), callback));
			REQUIRE(train.Load(data.TrainingData, ARBVersion(2, 0), callback));
			REQUIRE(2u == train.size());
			ElementNodePtr ele = ElementNode::New(L"Doesnt matter");
			ele->SetValue(L"These are some notes");
			ele->AddAttrib(ATTRIB_TRAINING_NAME, L"Hollister, CA");
			ele->AddAttrib(ATTRIB_TRAINING_SUBNAME, L"PASA");
			REQUIRE(!train.Load(ele, ARBVersion(2, 0), callback));
			REQUIRE(2u == train.size());
		}
	}


	SECTION("Load2")
	{
		if (!g_bMicroTest)
		{
			ElementNodePtr ele = ElementNode::New(TREE_TRAINING);
			ele->SetValue(L"These are some notes");
			ele->AddAttrib(ATTRIB_TRAINING_NAME, L"Hollister, CA");
			ele->AddAttrib(ATTRIB_TRAINING_SUBNAME, L"PASA");
			ARBTrainingList train;
			fmt::wmemory_buffer errs;
			ARBErrorCallback callback(errs);
			REQUIRE(!train.Load(ele, ARBVersion(2, 0), callback));
			ele->AddAttrib(ATTRIB_TRAINING_DATE, L"2008-1-13");
			REQUIRE(train.Load(ele, ARBVersion(2, 0), callback));
			REQUIRE(1u == train.size());
		}
	}


	SECTION("sort")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingList trainlist;
			ARBTrainingPtr train1 = ARBTraining::New();
			train1->SetDate(ARBDate(2006, 9, 4));
			train1->SetName(L"Test1");
			train1->SetSubName(L"SubN");
			train1->SetNote(L"A note");
			ARBTrainingPtr train2 = train1->Clone();
			train2->SetDate(ARBDate(2005, 3, 26));
			trainlist.AddTraining(train1);
			trainlist.AddTraining(train2);

			ARBTrainingList trainlist2;
			trainlist.Clone(trainlist2);
			REQUIRE(trainlist == trainlist2);

			trainlist.sort();
			REQUIRE(trainlist != trainlist2);
		}
	}


	SECTION("GetAllNames")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingList trainlist;
			CreateTrainingList(trainlist);
			std::set<std::wstring> names;
			REQUIRE(2u == trainlist.GetAllNames(names));
		}
	}


	SECTION("GetAllSubNames")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingList trainlist;
			CreateTrainingList(trainlist);
			std::set<std::wstring> names;
			REQUIRE(3u == trainlist.GetAllSubNames(names));
		}
	}


	SECTION("Find")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingList trainlist;
			CreateTrainingList(trainlist);

			ARBTrainingPtr train = trainlist[0]->Clone();

			REQUIRE(trainlist.FindTraining(train));
			train->SetNote(L"a change");
			REQUIRE(!trainlist.FindTraining(train));
		}
	}


	SECTION("AddDelete")
	{
		if (!g_bMicroTest)
		{
			ARBTrainingList trainlist;
			ARBTrainingPtr train1 = ARBTraining::New();
			train1->SetDate(ARBDate(2006, 9, 4));
			train1->SetName(L"Test1");
			train1->SetSubName(L"SubN");
			train1->SetNote(L"A note");
			ARBTrainingPtr train2 = train1->Clone();
			train2->SetDate(ARBDate(2005, 3, 26));
			trainlist.AddTraining(train1);
			trainlist.AddTraining(train2);
			trainlist.sort();
			ARBTrainingPtr train3 = trainlist[0]->Clone();
			REQUIRE(*train2 == *train3);
			train3->SetNote(L"Test");
			REQUIRE(*train2 != *train3);
			trainlist.AddTraining(train3);
			trainlist.AddTraining(train1->Clone());
			trainlist.sort();
			REQUIRE(4u == trainlist.size());
			REQUIRE(trainlist.DeleteTraining(train1));
			REQUIRE(3u == trainlist.size());
			REQUIRE(trainlist.DeleteTraining(train1));
			REQUIRE(2u == trainlist.size());
			REQUIRE(!trainlist.DeleteTraining(train1));
			REQUIRE(2u == trainlist.size());
			REQUIRE(*trainlist[0] != *trainlist[1]);
			ARBTrainingList trainlist2;
			trainlist.Clone(trainlist2);
			REQUIRE(trainlist == trainlist2);
			REQUIRE(trainlist[0].get() != trainlist2[0].get());
			REQUIRE(*trainlist[0] == *trainlist2[0]);
		}
	}
}
