/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDogRun and ARBDogRunList classes.
 * @author David Connet
 *
 * Revision History
 * 2019-01-17 Add some sanity tests for GetLifetimePoints.
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ConfigHandler.h"

#include "ARB/ARBDog.h"
#include "ARB/ARBDogClub.h"
#include "ARB/ARBDogRun.h"
#include "ARB/ARBDogTrial.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;

TEST_CASE("DogRun")
{
	SECTION("New")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	static ARBDogRunPtr New();
		}
	}


	SECTION("Clone")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDogRunPtr Clone() const;
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDogRun& operator=(ARBDogRun const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool operator==(ARBDogRun const& rhs) const;
			//	bool operator!=(ARBDogRun const& rhs) const
		}
	}


	SECTION("GenName")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	virtual wxString GetGenericName() const;
		}
	}


	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Load(
			//			ARBConfig const& inConfig,
			//			ARBDogClubList const& inClubs,
			//			ElementNodePtr inTree,
			//			ARBVersion const& inVersion,
			//			ARBErrorCallback& ioCallback);
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Save(ElementNodePtr ioTree) const;
		}
	}


	SECTION("NumOtherPointsInUse")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int NumOtherPointsInUse(wxString const& inOther) const;
		}
	}


	SECTION("RenameOtherPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameOtherPoints(
			//			wxString const& inOldName,
			//			wxString const& inNewName);
		}
	}


	SECTION("DeleteOtherPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteOtherPoints(wxString const& inName);
		}
	}


	SECTION("GetSpeedPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	short GetSpeedPoints(ARBConfigScoringPtr inScoring) const;
		}
	}


	SECTION("GetTitlePoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	double GetTitlePoints(
			//			ARBConfigScoringPtr inScoring,
			//			bool* outClean = nullptr,
			//			double* outLifeTime = nullptr,
			//			double* outPlacement = nullptr) const;
		}
	}


	SECTION("GetLifetimePoints")
	{
		if (!g_bMicroTest)
		{
			CConfigHandler handler;
			ARBConfig config;
			config.Default(&handler);

			ARBDate date(2014, 8, 31);
			wxString venue(L"NADAC");
			wxString division(L"Std Skilled");
			wxString level(L"Intro");

			ARBConfigVenuePtr pVenue;
			REQUIRE(config.GetVenues().FindVenue(venue, &pVenue));
			REQUIRE(pVenue);
			REQUIRE(1u == pVenue->GetLifetimeNames().size());
			ARBConfigLifetimeNamePtr pLifetimeName = pVenue->GetLifetimeNames()[0];

			ARBDogClubList clubs;
			clubs.AddClub(L"Club Name", venue);

			const struct RealUserData
			{
				wxString event;
				wxString courseFaults;
				wxString sct;
				wxString time;
				wxString yards;
				wxString bonus;
				wxString place;
				wxString q;
				wxString scorepts;
				wxString titlepts;
				double computedpts;
			} runs[] = {
				// Extreme Chances is T+F, 0 title points, so all is done with bonus
				// (these are real runs from a user)
				{L"Extreme Chances", L"0", L"0", L"0", L"0", L"15", L"0", L"Q", L"0", L"15", 15},
				{L"Extreme Chances", L"0", L"0", L"0", L"0", L"15", L"0", L"Q", L"0", L"15", 15},
				{L"Extreme Chances", L"2", L"0", L"12.85", L"71", L"5", L"1", L"Q", L"14.85", L"5", 5},
				{L"Extreme Chances", L"1", L"0", L"13.58", L"7`", L"10", L"1", L"Q", L"14.58", L"10", 10},
				// Barrelers is F/T, 10 title points on 0 faults, 5 with 5f
				// (these are not real runs!)
				{L"Regular", L"0", L"0", L"0", L"0", L"0", L"0", L"Q", L"0", L"10", 10},
				{L"Regular", L"2", L"0", L"0", L"0", L"10", L"0", L"Q", L"0", L"15", 15},
			};

			for (size_t n = 0; n < _countof(runs); ++n)
			{
				ARBConfigEventPtr pEvent;
				ARBConfigScoringPtr pScoring;
				REQUIRE(config.GetVenues().FindEvent(venue, runs[n].event, division, level, date, &pEvent, &pScoring));
				REQUIRE(pEvent);
				REQUIRE(pScoring);

				ElementNodePtr pData = ElementNode::New(L"Run");
				pData->AddAttrib(L"Date", date);
				pData->AddAttrib(L"Division", division);
				pData->AddAttrib(L"Event", runs[n].event);
				pData->AddAttrib(L"Height", L"20");
				pData->AddAttrib(L"Level", level);
				pData->AddElementNode(L"Judge")->SetValue(L"Judge Name");
				ElementNodePtr pByTime = pData->AddElementNode(L"ByTime");
				pByTime->AddAttrib(L"CourseFaults", runs[n].courseFaults);
				pByTime->AddAttrib(L"SCT", runs[n].sct);
				pByTime->AddAttrib(L"Time", runs[n].time);
				pByTime->AddAttrib(L"Yards", runs[n].yards);
				pByTime->AddAttrib(L"bonusPts", runs[n].bonus);
				ElementNodePtr pPlace = pData->AddElementNode(L"Placement");
				pPlace->AddAttrib(L"Place", runs[n].place);
				pPlace->AddAttrib(L"Q", runs[n].q);
				pPlace->AddAttrib(L"scorePts", runs[n].scorepts);
				pPlace->AddAttrib(L"titlePts", runs[n].titlepts);

				ARBDogRunPtr pRun = ARBDogRun::New();
				wxString errMsg;
				ARBErrorCallback err(errMsg);
				REQUIRE(pRun->Load(config, clubs, pData, ARBVersion(14, 5), err));

				REQUIRE(runs[n].computedpts == pRun->GetTitlePoints(pScoring));
				REQUIRE(runs[n].computedpts == pRun->GetLifetimePoints(pScoring, pLifetimeName->GetName()));
			}
		}
	}


	SECTION("GetPlacementPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	double GetPlacementPoints(ARBConfigScoringPtr const& inScoring) const;
		}
	}


	SECTION("GetScore")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	double GetScore(ARBConfigScoringPtr inScoring) const;
		}
	}


	SECTION("Coursing")
	{
		if (!g_bMicroTest)
		{
			CConfigHandler handler;
			ARBConfig config;
			config.Default(&handler);

			ARBDate date(2024, 6, 8);

			static const struct
			{
				wxString venue;
				wxString division;
				wxString level;
				wxString evt;
				double yards;
				double time;
				wxString yps;
				wxString mph;
			} tests[] = {
				{L"FCAT", L"FCAT", L"FCAT", L"FCAT", 100.0, 7.96, L"12.56", L"25.70"},
				{L"SW", L"Standard", L"Standard", L"Drag Race 100", 100.0, 20.2, L"4.95", L"10.13"},
				{L"SW", L"Veterans", L"Veterans", L"Drag Race 100", 100.0, 20.2, L"4.95", L"10.13"},
				{L"SW", L"Standard", L"Standard", L"Drag Race 50", 50.0, 10.1, L"4.95", L"10.13"},
			};

			for (auto const& test : tests)
			{
				ARBConfigVenuePtr pVenue;
				REQUIRE(config.GetVenues().FindVenue(test.venue, &pVenue));
				REQUIRE(pVenue);

				ARBConfigEventPtr pEvent;
				ARBConfigScoringPtr pScoring;
				REQUIRE(config.GetVenues()
							.FindEvent(test.venue, test.evt, test.division, test.level, date, &pEvent, &pScoring));
				REQUIRE(pEvent);
				REQUIRE(pScoring);

				ARBDogClubList clubs;
				clubs.AddClub(L"Club Name", test.venue);

				ARBDogRunPtr run = ARBDogRun::New();
				run->SetClub(clubs[0]);
				run->SetDate(date);
				run->SetEvent(test.evt);
				run->SetDivision(test.division);
				run->SetLevel(test.level);
				run->GetScoring().SetType(ARBScoringType::BySpeed, false);
				run->GetScoring().SetYards(test.yards);
				run->GetScoring().SetTime(test.time);

				double yps = 0.0;
				REQUIRE(run->GetScoring().GetYPS(false, yps));
				REQUIRE(ARBDouble::ToString(yps, 2) == test.yps);

				double mph = 0.0;
				int prec = 2;
				REQUIRE(run->GetScoring().GetObstaclesPS(false, false, mph, prec));
				REQUIRE(ARBDouble::ToString(mph, prec) == test.mph);

				// Note: For coursing (FCAT, SpeedWay), user manually enters bonus pts.
			}
		}
	}
}


TEST_CASE("DogRunList")
{
	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool Load(
			//			ARBConfig const& inConfig,
			//			ARBDogClubList const& inClubs,
			//			ElementNodePtr inTree,
			//			ARBVersion const& inVersion,
			//			ARBErrorCallback& ioCallback);
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		}
	}


	SECTION("sort")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	void sort();
		}
	}


	SECTION("GetStartDate")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDate GetStartDate() const;
		}
	}


	SECTION("GetEndDate")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	ARBDate GetEndDate() const;
		}
	}


	SECTION("AddRun")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool AddRun(ARBDogRunPtr inRun);
		}
	}


	SECTION("DeleteRun")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	bool DeleteRun(ARBDogRunPtr inRun);
		}
	}
}

} // namespace dconSoft
