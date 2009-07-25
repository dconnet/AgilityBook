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
 * @brief Test ARBDogRunScoring class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogRunScoring.h"
#include "Element.h"


SUITE(TestDogRunScoring)
{
	TEST(TranslateConfigScoring)
	{
		TODO_TEST
	//	static ARBDogRunScoring::ScoringType TranslateConfigScoring(ARBConfigScoring::ScoringStyle inType);
	}


	TEST(ctor)
	{
		TODO_TEST
	//	ARBDogRunScoring();
	//	ARBDogRunScoring(ARBDogRunScoring const& rhs);
	}


	TEST(OpEqual)
	{
		TODO_TEST
	//	ARBDogRunScoring& operator=(ARBDogRunScoring const& rhs);
	}


	TEST(Compare)
	{
		TODO_TEST
	//	bool operator==(ARBDogRunScoring const& rhs) const;
	//	bool operator!=(ARBDogRunScoring const& rhs) const
	}


	TEST(Load)
	{
		TODO_TEST
	//	bool Load(
	//			short inConfigVersion,
	//			ARBConfigEventPtr inEvent,
	//			ARBConfigScoringPtr inEventScoring,
	//			ElementNodePtr inTree,
	//			ARBVersion const& inVersion,
	//			ARBErrorCallback& ioCallback);
	}


	TEST(Save)
	{
		TODO_TEST
	//	bool Save(ElementNodePtr ioTree) const;
	}


	TEST(GetMinYPS)
	{
		TODO_TEST
	//	bool GetMinYPS(
	//			bool inTableInYPS,
	//			double& outYPS) const;
	}


	TEST(GetYPS)
	{
		TODO_TEST
	//	bool GetYPS(
	//			bool inTableInYPS,
	//			double& outYPS) const;
	}


	TEST(GetObstaclesPS)
	{
		TODO_TEST
	//	bool GetObstaclesPS(bool inTableInYPS, double& outOPS) const;
	}


	TEST(GetTimeFaults)
	{
		TODO_TEST
	//	double GetTimeFaults(ARBConfigScoringPtr inScoring) const;
	}
}
