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
 * @brief Test ARBTraining class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-13 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBTraining.h"
#include "Element.h"


struct TestTrainingData
{
	ElementNodePtr TrainingData;
	TestTrainingData();
};


TestTrainingData::TestTrainingData()
{
	TrainingData = ElementNode::New(TREE_TRAINING);
	TrainingData->SetValue(wxT("These are some notes"));
	TrainingData->AddAttrib(ATTRIB_TRAINING_DATE, wxT("2006-3-4"));
	TrainingData->AddAttrib(ATTRIB_TRAINING_NAME, wxT("A Name"));
	TrainingData->AddAttrib(ATTRIB_TRAINING_SUBNAME, wxT("SubName1"));
}


static void CreateTrainingList(ARBTrainingList& trainlist)
{
	ARBTrainingPtr train = ARBTraining::New();
	train->SetDate(ARBDate(2006, 9, 4));
	train->SetName(wxT("Test1"));
	train->SetSubName(wxT("SubN"));
	trainlist.AddTraining(train);
	train = ARBTraining::New();
	train->SetDate(ARBDate(2005, 3, 26));
	train->SetName(wxT("Test2"));
	train->SetSubName(wxT("Sub3N"));
	trainlist.AddTraining(train);
	train = ARBTraining::New();
	train->SetDate(ARBDate(2005, 3, 26));
	train->SetName(wxT("Test2"));
	train->SetSubName(wxT("Sub2N"));
	trainlist.AddTraining(train);
	trainlist.sort();
}


SUITE(TestTraining)
{
	TEST(New)
	{
		ARBTrainingPtr train = ARBTraining::New();
		CHECK(NULL != train.get());
	}


	TEST(Clone)
	{
		ARBTrainingPtr train = ARBTraining::New();
		train->SetDate(ARBDate(2007, 9, 1));
		ARBTrainingPtr train2 = train->Clone();
		CHECK(NULL != train2.get());
		CHECK(train.get() != train2.get());
		CHECK(*train == *train2);
		CHECK(train->GetDate() == train2->GetDate());
	}


	TEST(OpEqual)
	{
		ARBTrainingPtr train1 = ARBTraining::New();
		train1->SetDate(ARBDate(2007, 9, 1));
		ARBTrainingPtr train2 = ARBTraining::New();
		CHECK(*train1 != *train2);
		*train1 = *train2;
		CHECK(*train1 == *train2);
	}


	TEST_FIXTURE(TestTrainingData, Compare)
	{
		ARBTrainingPtr train = ARBTraining::New();
		wxString errs;
		ARBErrorCallback callback(errs);
		CHECK(train->Load(TrainingData, ARBVersion(2, 0), callback));
		ARBTrainingPtr train2 = train->Clone();
		CHECK(train.get() != train2.get());
		CHECK(*train == *train2);
		CHECK(train->GetDate() == train2->GetDate());
		ARBDate n = ARBDate::Today();
		train2->SetDate(n);
		CHECK(*train < *train2);
		CHECK(*train != *train2);
		CHECK(!(*train > *train2));
	}


	TEST(GenName)
	{
		ARBTrainingPtr train = ARBTraining::New();
		train->SetDate(ARBDate(2007, 3, 13));
		CHECK(wxT("3/13/2007") == train->GetGenericName());
	}

	TEST_FIXTURE(TestTrainingData, Load)
	{
		ARBTrainingPtr train = ARBTraining::New();
		wxString errs;
		ARBErrorCallback callback(errs);
		CHECK(train->Load(TrainingData, ARBVersion(1, 0), callback));
		wxString name = train->GetGenericName();
		CHECK(!name.empty());
	}


	TEST_FIXTURE(TestTrainingData, Save)
	{
		ARBTrainingPtr train = ARBTraining::New();
		wxString errs;
		ARBErrorCallback callback(errs);
		CHECK(train->Load(TrainingData, ARBVersion(2, 0), callback));
		ElementNodePtr ele = ElementNode::New();
		CHECK(train->Save(ele));
	}
}


SUITE(TestTrainingList)
{
	TEST_FIXTURE(TestTrainingData, Load)
	{
		ARBTrainingList train;
		wxString errs;
		ARBErrorCallback callback(errs);
		CHECK(train.Load(TrainingData, ARBVersion(1, 0), callback));
		CHECK(train.Load(TrainingData, ARBVersion(2, 0), callback));
		CHECK_EQUAL(2u, train.size());
		ElementNodePtr ele = ElementNode::New(wxT("Doesnt matter"));
		ele->SetValue(wxT("These are some notes"));
		ele->AddAttrib(ATTRIB_TRAINING_NAME, wxT("Hollister, CA"));
		ele->AddAttrib(ATTRIB_TRAINING_SUBNAME, wxT("PASA"));
		CHECK(!train.Load(ele, ARBVersion(2, 0), callback));
		CHECK_EQUAL(2u, train.size());
	}


	TEST_FIXTURE(TestTrainingData, Load2)
	{
		ElementNodePtr ele = ElementNode::New(TREE_TRAINING);
		ele->SetValue(wxT("These are some notes"));
		ele->AddAttrib(ATTRIB_TRAINING_NAME, wxT("Hollister, CA"));
		ele->AddAttrib(ATTRIB_TRAINING_SUBNAME, wxT("PASA"));
		ARBTrainingList train;
		wxString errs;
		ARBErrorCallback callback(errs);
		CHECK(!train.Load(ele, ARBVersion(2, 0), callback));
		ele->AddAttrib(ATTRIB_TRAINING_DATE, wxT("2008-1-13"));
		CHECK(train.Load(ele, ARBVersion(2, 0), callback));
		CHECK_EQUAL(1u, train.size());
	}


	TEST(sort)
	{
		ARBTrainingList trainlist;
		ARBTrainingPtr train1 = ARBTraining::New();
		train1->SetDate(ARBDate(2006, 9, 4));
		train1->SetName(wxT("Test1"));
		train1->SetSubName(wxT("SubN"));
		train1->SetNote(wxT("A note"));
		ARBTrainingPtr train2 = train1->Clone();
		train2->SetDate(ARBDate(2005, 3, 26));
		trainlist.AddTraining(train1);
		trainlist.AddTraining(train2);

		ARBTrainingList trainlist2;
		trainlist.Clone(trainlist2);
		CHECK(trainlist == trainlist2);

		trainlist.sort();
		CHECK(trainlist != trainlist2);
	}


	TEST(GetAllNames)
	{
		ARBTrainingList trainlist;
		CreateTrainingList(trainlist);
		std::set<wxString> names;
		CHECK_EQUAL(2u, trainlist.GetAllNames(names));
	}


	TEST(GetAllSubNames)
	{
		ARBTrainingList trainlist;
		CreateTrainingList(trainlist);
		std::set<wxString> names;
		CHECK_EQUAL(3u, trainlist.GetAllSubNames(names));
	}


	TEST(Find)
	{
		ARBTrainingList trainlist;
		CreateTrainingList(trainlist);

		ARBTrainingPtr train = trainlist[0]->Clone();

		CHECK(trainlist.FindTraining(train));
		train->SetNote(wxT("a change"));
		CHECK(!trainlist.FindTraining(train));
	}


	TEST(AddDelete)
	{
		ARBTrainingList trainlist;
		ARBTrainingPtr train1 = ARBTraining::New();
		train1->SetDate(ARBDate(2006, 9, 4));
		train1->SetName(wxT("Test1"));
		train1->SetSubName(wxT("SubN"));
		train1->SetNote(wxT("A note"));
		ARBTrainingPtr train2 = train1->Clone();
		train2->SetDate(ARBDate(2005, 3, 26));
		trainlist.AddTraining(train1);
		trainlist.AddTraining(train2);
		trainlist.sort();
		ARBTrainingPtr train3 = trainlist[0]->Clone();
		CHECK(*train2 == *train3);
		train3->SetNote(wxT("Test"));
		CHECK(*train2 != *train3);
		trainlist.AddTraining(train3);
		trainlist.AddTraining(train1->Clone());
		trainlist.sort();
		CHECK_EQUAL(4u, trainlist.size());
		CHECK(trainlist.DeleteTraining(train1));
		CHECK_EQUAL(3u, trainlist.size());
		CHECK(trainlist.DeleteTraining(train1));
		CHECK_EQUAL(2u, trainlist.size());
		CHECK(!trainlist.DeleteTraining(train1));
		CHECK_EQUAL(2u, trainlist.size());
		CHECK(*trainlist[0] != *trainlist[1]);
		ARBTrainingList trainlist2;
		trainlist.Clone(trainlist2);
		CHECK(trainlist == trainlist2);
		CHECK(trainlist[0].get() != trainlist2[0].get());
		CHECK(*trainlist[0] == *trainlist2[0]);
	}
}
