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
			//	virtual std::wstring GetGenericName() const;
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
			//	int NumOtherPointsInUse(std::wstring const& inOther) const;
		}
	}


	SECTION("RenameOtherPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int RenameOtherPoints(
			//			std::wstring const& inOldName,
			//			std::wstring const& inNewName);
		}
	}


	SECTION("DeleteOtherPoints")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
			//	int DeleteOtherPoints(std::wstring const& inName);
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
			std::wstring venue(L"NADAC");
			std::wstring division(L"Std Skilled");
			std::wstring level(L"Intro");

			ARBConfigVenuePtr pVenue;
			REQUIRE(config.GetVenues().FindVenue(venue, &pVenue));
			REQUIRE(pVenue);
			REQUIRE(1 == pVenue->GetLifetimeNames().size());
			ARBConfigLifetimeNamePtr pLifetimeName = pVenue->GetLifetimeNames()[0];

			ARBDogClubList clubs;
			clubs.AddClub(L"Club Name", venue);

			const struct RealUserData
			{
				std::wstring event;
				std::wstring courseFaults;
				std::wstring sct;
				std::wstring time;
				std::wstring yards;
				std::wstring bonus;
				std::wstring place;
				std::wstring q;
				std::wstring scorepts;
				std::wstring titlepts;
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
				fmt::wmemory_buffer errMsg;
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
