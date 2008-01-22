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
 * @brief Test ARBTraining class
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-13 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBTraining.h"
#include "Element.h"


FIXTURE(Training)


namespace
{
	ElementNodePtr TrainingData;
}


SETUP(Training)
{
	WIN_ASSERT_TRUE(CommonSetup());

	TrainingData = ElementNode::New(TREE_TRAINING);
	TrainingData->SetValue(_T("These are some notes"));
	TrainingData->AddAttrib(ATTRIB_TRAINING_DATE, _T("2006-3-4"));
	TrainingData->AddAttrib(ATTRIB_TRAINING_NAME, _T("A Name"));
	TrainingData->AddAttrib(ATTRIB_TRAINING_SUBNAME, _T("SubName1"));
}


TEARDOWN(Training)
{
	TrainingData.reset();
	WIN_ASSERT_TRUE(CommonTeardown());
}


static void CreateTrainingList(ARBTrainingList& trainlist)
{
	ARBTrainingPtr train = ARBTraining::New();
	train->SetDate(ARBDate(2006, 9, 4));
	train->SetName(_T("Test1"));
	train->SetSubName(_T("SubN"));
	trainlist.AddTraining(train);
	train = ARBTraining::New();
	train->SetDate(ARBDate(2005, 3, 26));
	train->SetName(_T("Test2"));
	train->SetSubName(_T("Sub3N"));
	trainlist.AddTraining(train);
	train = ARBTraining::New();
	train->SetDate(ARBDate(2005, 3, 26));
	train->SetName(_T("Test2"));
	train->SetSubName(_T("Sub2N"));
	trainlist.AddTraining(train);
	trainlist.sort();
}


BEGIN_TEST(Training_New)
{
	ARBTrainingPtr train = ARBTraining::New();
	WIN_ASSERT_NOT_NULL(train.get());
}
END_TEST


BEGIN_TEST(Training_Clone)
{
	ARBTrainingPtr train = ARBTraining::New();
	train->SetDate(ARBDate(2007, 9, 1));
	ARBTrainingPtr train2 = train->Clone();
	WIN_ASSERT_NOT_NULL(train2.get());
	WIN_ASSERT_NOT_EQUAL(train.get(), train2.get());
	WIN_ASSERT_EQUAL(*train, *train2);
	WIN_ASSERT_EQUAL(train->GetDate(), train2->GetDate());
}
END_TEST


BEGIN_TEST(Training_OpEqual)
{
	ARBTrainingPtr train1 = ARBTraining::New();
	train1->SetDate(ARBDate(2007, 9, 1));
	ARBTrainingPtr train2 = ARBTraining::New();
	WIN_ASSERT_NOT_EQUAL(*train1, *train2);
	*train1 = *train2;
	WIN_ASSERT_EQUAL(*train1, *train2);
}
END_TEST


BEGIN_TESTF(Training_Compare, Training)
{
	ARBTrainingPtr train = ARBTraining::New();
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(train->Load(TrainingData, ARBVersion(2, 0), callback));
	ARBTrainingPtr train2 = train->Clone();
	WIN_ASSERT_NOT_EQUAL(train.get(), train2.get());
	WIN_ASSERT_EQUAL(*train, *train2);
	WIN_ASSERT_EQUAL(train->GetDate(), train2->GetDate());
	ARBDate n = ARBDate::Today();
	train2->SetDate(n);
	WIN_ASSERT_TRUE(*train < *train2);
	WIN_ASSERT_NOT_EQUAL(*train, *train2);
	WIN_ASSERT_FALSE(*train > *train2);
}
END_TESTF


BEGIN_TEST(Training_GenName)
{
	ARBTrainingPtr train = ARBTraining::New();
	train->SetDate(ARBDate(2007, 3, 13));
	WIN_ASSERT_STRING_EQUAL(_T("3/13/2007"), train->GetGenericName().c_str());
}
END_TEST


BEGIN_TESTF(Training_Load, Training)
{
	ARBTrainingPtr train = ARBTraining::New();
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(train->Load(TrainingData, ARBVersion(1, 0), callback));
	tstring name = train->GetGenericName();
	WIN_ASSERT_FALSE(name.empty());
}
END_TESTF


BEGIN_TESTF(Training_Save, Training)
{
	ARBTrainingPtr train = ARBTraining::New();
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(train->Load(TrainingData, ARBVersion(2, 0), callback));
	ElementNodePtr ele = ElementNode::New();
	WIN_ASSERT_TRUE(train->Save(ele));
}
END_TESTF


BEGIN_TESTF(TrainingList_Load, Training)
{
	ARBTrainingList train;
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_TRUE(train.Load(TrainingData, ARBVersion(1, 0), callback));
	WIN_ASSERT_TRUE(train.Load(TrainingData, ARBVersion(2, 0), callback));
	WIN_ASSERT_EQUAL(2u, train.size());
	ElementNodePtr ele = ElementNode::New(_T("Doesnt matter"));
	ele->SetValue(_T("These are some notes"));
	ele->AddAttrib(ATTRIB_TRAINING_NAME, _T("Hollister, CA"));
	ele->AddAttrib(ATTRIB_TRAINING_SUBNAME, _T("PASA"));
	WIN_ASSERT_FALSE(train.Load(ele, ARBVersion(2, 0), callback));
	WIN_ASSERT_EQUAL(2u, train.size());
}
END_TESTF


BEGIN_TESTF(TrainingList_Load2, Training)
{
	ElementNodePtr ele = ElementNode::New(TREE_TRAINING);
	ele->SetValue(_T("These are some notes"));
	ele->AddAttrib(ATTRIB_TRAINING_NAME, _T("Hollister, CA"));
	ele->AddAttrib(ATTRIB_TRAINING_SUBNAME, _T("PASA"));
	ARBTrainingList train;
	tstring errs;
	ARBErrorCallback callback(errs);
	WIN_ASSERT_FALSE(train.Load(ele, ARBVersion(2, 0), callback));
	ele->AddAttrib(ATTRIB_TRAINING_DATE, _T("2008-1-13"));
	WIN_ASSERT_TRUE(train.Load(ele, ARBVersion(2, 0), callback));
	WIN_ASSERT_EQUAL(1u, train.size());
}
END_TESTF


BEGIN_TEST(TrainingList_sort)
{
	ARBTrainingList trainlist;
	ARBTrainingPtr train1 = ARBTraining::New();
	train1->SetDate(ARBDate(2006, 9, 4));
	train1->SetName(_T("Test1"));
	train1->SetSubName(_T("SubN"));
	train1->SetNote(_T("A note"));
	ARBTrainingPtr train2 = train1->Clone();
	train2->SetDate(ARBDate(2005, 3, 26));
	trainlist.AddTraining(train1);
	trainlist.AddTraining(train2);

	ARBTrainingList trainlist2;
	trainlist.Clone(trainlist2);
	WIN_ASSERT_EQUAL(trainlist, trainlist2);

	trainlist.sort();
	WIN_ASSERT_NOT_EQUAL(trainlist, trainlist2);
}
END_TEST


BEGIN_TEST(TrainingList_GetAllNames)
{
	ARBTrainingList trainlist;
	CreateTrainingList(trainlist);
	std::set<tstring> names;
	WIN_ASSERT_EQUAL(2u, trainlist.GetAllNames(names));
}
END_TEST


BEGIN_TEST(TrainingList_GetAllSubNames)
{
	ARBTrainingList trainlist;
	CreateTrainingList(trainlist);
	std::set<tstring> names;
	WIN_ASSERT_EQUAL(3u, trainlist.GetAllSubNames(names));
}
END_TEST


BEGIN_TEST(TrainingList_Find)
{
	ARBTrainingList trainlist;
	CreateTrainingList(trainlist);

	ARBTrainingPtr train = trainlist[0]->Clone();

	WIN_ASSERT_TRUE(trainlist.FindTraining(train));
	train->SetNote(_T("a change"));
	WIN_ASSERT_FALSE(trainlist.FindTraining(train));
}
END_TEST


BEGIN_TEST(TrainingList_AddDelete)
{
	ARBTrainingList trainlist;
	ARBTrainingPtr train1 = ARBTraining::New();
	train1->SetDate(ARBDate(2006, 9, 4));
	train1->SetName(_T("Test1"));
	train1->SetSubName(_T("SubN"));
	train1->SetNote(_T("A note"));
	ARBTrainingPtr train2 = train1->Clone();
	train2->SetDate(ARBDate(2005, 3, 26));
	trainlist.AddTraining(train1);
	trainlist.AddTraining(train2);
	trainlist.sort();
	ARBTrainingPtr train3 = trainlist[0]->Clone();
	WIN_ASSERT_EQUAL(*train2, *train3);
	train3->SetNote(_T("Test"));
	WIN_ASSERT_NOT_EQUAL(*train2, *train3);
	trainlist.AddTraining(train3);
	trainlist.AddTraining(train1->Clone());
	trainlist.sort();
	WIN_ASSERT_EQUAL(4u, trainlist.size());
	WIN_ASSERT_TRUE(trainlist.DeleteTraining(train1));
	WIN_ASSERT_EQUAL(3u, trainlist.size());
	WIN_ASSERT_TRUE(trainlist.DeleteTraining(train1));
	WIN_ASSERT_EQUAL(2u, trainlist.size());
	WIN_ASSERT_FALSE(trainlist.DeleteTraining(train1));
	WIN_ASSERT_EQUAL(2u, trainlist.size());
	WIN_ASSERT_NOT_EQUAL(*trainlist[0], *trainlist[1]);
	ARBTrainingList trainlist2;
	trainlist.Clone(trainlist2);
	WIN_ASSERT_EQUAL(trainlist, trainlist2);
	WIN_ASSERT_NOT_EQUAL(trainlist[0].get(), trainlist2[0].get());
	WIN_ASSERT_EQUAL(*trainlist[0], *trainlist2[0]);
}
END_TEST
