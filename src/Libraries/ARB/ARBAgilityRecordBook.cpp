/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The main data class.
 * @author David Connet
 *
 * This class is separated from the Win32 CDocument class to help anyone who
 * tries to port this to a different platform or put a different GUI on it.
 *
 * When changing the file version, make sure to update 'curVersion' below,
 * src/Win/res/DefaultConfig.xml and src/Win/res/AgilityRecordBook.dtd.
 *
 * Revision History
 * 2020-09-15 File version 15.3
 * 2020-01-17 File version 15.2
 * 2020-01-16 Fix runs when table/subnames are changed in config.
 * 2019-09-17 File version 15.1
 * 2019-08-31 File version 15.0
 * 2019-08-18 File version 14.6
 * 2018-12-16 Convert to fmt.
 * 2017-12-31 File version 14.5
 * 2016-01-06 File version 14.4
 * 2015-11-27 Fix Qs on runs when titling points are removed from config.
 * 2015-02-13 File version 14.3
 * 2014-08-17 Fix crash when saving a new file.
 * 2014-06-09 Add access to write-only data for file-properties purpose.
 * 2014-05-18 File version 14.3 (but version bump didn't happen)
 * 2013-10-23 Change arch signature to allow deprecating platforms.
 * 2013-09-06 File version 14.2
 *            Added 'ShortName' to 'Division', 'Level', 'SubLevel', 'Event'.
 * 2013-05-24 File version 14.1
 *            Added 'Date' to 'Trial'.
 * 2013-01-13 File version 14.0
 *            Added new recurring title suffix style.
 * 2012-07-30 File version 13.2
 *            Added new title style (eTitleNone).
 *            Added 'titlePts' to 'Placement'.
 * 2012-07-08 Fix title instance when a title is changed to recurring.
 * 2011-07-31 File version 13.1
 *            Added 'platform','os' to 'AgilityBook', modified
 *            TitlePoints (see ARBConfigTitlePoints.cpp).
 * 2011-06-09 File version 12.11
 *            Added 'Config' to 'Action'.
 * 2009-12-18 File version 12.11
 *            Added 'O' to 'Entry'.
 * 2009-07-26 File version 12.10
 *            Added 'DNR' to 'Placement' and 'ReferenceRun'
 * 2008-02-12 File version 12.9
 *            Fix metadata bug in dognotes.
 * 2008-08-05 File version 12.8
 *            Added 'style' to Title, 'Style' to 'Titles'
 * 2008-01-01 File version 12.7. Added 'Visible' to InfoItem,
 *            'obstacles' to 'Scoring'
 * 2007-07-03 File version 12.6. Added 'show' to 'Title',
 *            'timestamp' to 'AgilityBook'
 * 2007-04-22 Added 'Accom', 'Confirm' to 'Calendar', 'icon' to 'Venue'
 * 2007-02-27 File version 12.5. Added 'defValue' to 'OtherPts'.
 * 2007-01-03 File version 12.4. Added 'DateDraw' to Calendar.
 *            Changed 'CRCDMeta' to 'CRCDMeta2'.
 * 2006-11-03 File version 12.3. Added 'SpdMult'.
 * 2006-07-02 File version 12.2. Added 'subtractTF'.
 * 2006-02-16 File version 12.1. Added 'RenameEvent'.
 *            Allow fractional faults in title points.
 *            Cleaned up memory usage with smart pointers.
 * 2005-12-14 File version 12.0. Moved 'Titles' to 'Venue'.
 * 2005-10-14 File version 11.1. Added 'Prefix' to 'Titles'.
 * 2005-07-15 File version 11.0. Changed how Double-Qs are done.
 * 2005-04-26 File version 10.2. Added 'ver' to 'AgilityBook'
 * 2005-01-02 Added subnames to events.
 * 2004-11-15 File version 10.1. Added 'underTF'/'overTF' to 'Scoring'.
 * 2004-10-06 File version 10. Added 'Height' to 'ReferenceRun'
 *            GetAllHeights now accumulates these too. Changed
 *            lifetime points.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-06-29 File version 9.0. Added 'SubName' to 'Training'. Changed
 *            structure of 'RegNum'.
 * 2004-03-26 File version 8.6. Changed Table-in-YPS to hasTable.
 * 2004-03-13 File version 8.5. Added 'isHidden' to 'Title'.
 * 2004-02-15 Cleaned up newline in warning msg.
 * 2004-02-14 File version 8.4. Added Table-in-YPS flag.
 * 2004-02-02 File version 8.3. Added ARBDogExistingPoints items.
 * 2004-01-21 File version 8.2. Added ConfigTitles LongName. Added
 *            ARBConfigAction.
 * 2003-12-29 GetAllClubs/Locations now includes calendar entries.
 * 2003-12-27 File version 8.1. Changed ARBConfigScoring.
 * 2003-12-07 File version 8.0. Added Info section.
 * 2003-11-26 Changed version number to a complex value.
 *            Added warning check when minor versions are different.
 * 2003-10-31 Added options to Save() to allow partial saves.
 * 2003-10-13 File version 7. Added course faults to ByScore/etc.
 * 2003-09-21 File version 6. Added training log.
 * 2003-07-24 Removed built-in sort on dogs. Dogs are user-sorted now.
 * 2003-06-23 File version 5. Added 'Note' and 'Verified' to trials.
 *            Changed how title points are configured.
 * 2003-06-11 File version 4. Added 'dropFractions' to config scoring.
 *            Added 'Desc' to config venue and event.
 * 2003-04-21 File version 3. Added notes to config events. Moved
 *            faulttypes and otherpoints from venue to main config.
 *            Added Receieved field to Titles.
 * 2003-01-23 Bumped file version to 2: see ARBCalendar.cpp
 */

#include "stdafx.h"
#include "ARB/ARBAgilityRecordBook.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBDog.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/ARBMisc.h"
#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"

#if defined(__WXWINDOWS__)
#include <wx/utils.h>
#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif
#endif


namespace dconSoft
{
using namespace ARBCommon;
namespace ARB
{

/////////////////////////////////////////////////////////////////////////////

ARBVersion const& ARBAgilityRecordBook::GetCurrentDocVersion()
{
	static ARBVersion const curVersion(15, 3);
	return curVersion;
}

/////////////////////////////////////////////////////////////////////////////

ARBAgilityRecordBook::ARBAgilityRecordBook()
{
}


ARBAgilityRecordBook::~ARBAgilityRecordBook()
{
	clear();
}


// In general, we're very strict about our data. If anything is bad, we abort.
// This could be relaxed in many areas and either ignore it or set it to some
// default. The only reason we should end up with bad data is because someone
// was directly editing the file. Also the code is a little more forgiving
// than the DTD, we have some integrity checks that should never trigger if
// we actually include the DTD in the file.
// @todo: Relax strictness when reading data and handle errors better.
//  - note, we actually have relaxed some things...
bool ARBAgilityRecordBook::Load(
	ARBCommon::ElementNodePtr const& inTree,
	bool inCalendar,
	bool inTraining,
	bool inConfig,
	bool inInfo,
	bool inDogs,
	ARBErrorCallback& ioCallback)
{
	// Get the records ready.
	clear();

	// Make sure the input looks okay.
	// The root must be TREE_BOOK.
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_BOOK)
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidDocStructure(Localization()->InvalidRoot()));
		return false;
	}

	m_FileInfo.insert(m_FileInfo.end(), static_cast<size_t>(ARBFileInfo::Max), std::wstring());
	inTree->GetAttrib(ATTRIB_BOOK_VERSION, m_FileInfo[static_cast<size_t>(ARBFileInfo::Book)]);
	inTree->GetAttrib(ATTRIB_BOOK_PGM_VERSION, m_FileInfo[static_cast<size_t>(ARBFileInfo::Version)]);
	inTree->GetAttrib(ATTRIB_BOOK_PGM_PLATFORM, m_FileInfo[static_cast<size_t>(ARBFileInfo::Platform)]);
	inTree->GetAttrib(ATTRIB_BOOK_PGM_OS, m_FileInfo[static_cast<size_t>(ARBFileInfo::OS)]);
	inTree->GetAttrib(ATTRIB_BOOK_TIMESTAMP, m_FileInfo[static_cast<size_t>(ARBFileInfo::TimeStamp)]);

	// The version of the document must be something we understand.
	ARBVersion version;
	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_BOOK_VERSION, version))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_BOOK, ATTRIB_BOOK_VERSION));
		return false;
	}
	if (version < ARBVersion(1, 0) || version > GetCurrentDocVersion())
	{
		if (version.Major() == GetCurrentDocVersion().Major())
		{
			if (!ioCallback.OnError(Localization()->WarningNewerDoc()))
				return false;
		}
		else
		{
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(
				TREE_BOOK,
				ATTRIB_BOOK_VERSION,
				Localization()->UnknownVersion().c_str()));
			return false;
		}
	}

	// Something was loaded.
	bool bLoaded = false;

	// Load the calendar information.
	if (inCalendar)
	{
		bLoaded = true;
		for (int i = 0; i < inTree->GetElementCount(); ++i)
		{
			ElementNodePtr element = inTree->GetElementNode(i);
			if (!element)
				continue;
			if (element->GetName() == TREE_CALENDAR)
			{
				// Ignore any errors...
				m_Calendar.Load(element, version, ioCallback);
			}
		}
		m_Calendar.sort();
	}

	// Load the training information.
	if (inTraining)
	{
		bLoaded = true;
		for (int i = 0; i < inTree->GetElementCount(); ++i)
		{
			ElementNodePtr element = inTree->GetElementNode(i);
			if (!element)
				continue;
			if (element->GetName() == TREE_TRAINING)
			{
				// Ignore any errors...
				m_Training.Load(element, version, ioCallback);
			}
		}
		m_Training.sort();
	}

	// We have to load the configuration before any dog records.
	if (inConfig)
	{
		bLoaded = true;
		int nConfig = -1;
		// Find a configuration.
		for (int i = 0; i < inTree->GetElementCount(); ++i)
		{
			ElementNodePtr element = inTree->GetElementNode(i);
			if (!element)
				continue;
			if (element->GetName() == TREE_CONFIG)
			{
				// Make sure there's only one.
				if (-1 != nConfig)
				{
					ioCallback.LogMessage(Localization()->ErrorInvalidDocStructure(Localization()->InvalidConfig()));
					return false;
				}
				nConfig = i;
			}
		}
		// Oops. No config.
		if (0 > nConfig)
		{
			ioCallback.LogMessage(Localization()->ErrorInvalidDocStructure(Localization()->MissingConfig()));
			return false;
		}
		// Load the config.
		else if (!m_Config.Load(inTree->GetElementNode(nConfig), version, ioCallback))
		{
			// Error message was printed within.
			return false;
		}

		// Now, we can load the dog's information.
		if (inDogs)
		{
			// Now load the rest
			for (int i = 0; i < inTree->GetElementCount(); ++i)
			{
				ElementNodePtr element = inTree->GetElementNode(i);
				if (!element)
					continue;
				if (element->GetName() == TREE_DOG)
				{
					// If this fails, keep going.
					// We'll try to load whatever we can.
					m_Dogs.Load(m_Config, element, version, ioCallback);
				}
			}
		}
	}

	// Load the generic information.
	if (inInfo)
	{
		bLoaded = true;
		int i = inTree->FindElement(TREE_INFO);
		if (0 <= i)
		{
			// Ignore any errors...
			m_Info.Load(inTree->GetElementNode(i), version, ioCallback);
		}
	}

	return bLoaded;
}


static std::wstring GetTimeStamp()
{
	time_t t;
	time(&t);
	struct tm* pTime = nullptr;
#if defined(ARB_HAS_SECURE_LOCALTIME)
	struct tm l;
	if (0 == _localtime64_s(&l, &t))
		pTime = &l;
#else
	pTime = localtime(&t);
#endif
	wchar_t szBuffer[128] = {0}; // as defined by VC9 ATL::maxTimeBufferSize
	if (!pTime || !wcsftime(szBuffer, 128, L"%Y-%m-%d %H:%M:%S", pTime))
	{
		szBuffer[0] = '\0';
	}
	return std::wstring(szBuffer);
}


bool ARBAgilityRecordBook::Save(
	ElementNodePtr const& outTree,
	std::wstring const& inPgmVer,
	bool inCalendar,
	bool inTraining,
	bool inConfig,
	bool inInfo,
	bool inDogs) const
{
	assert(outTree);
	if (!outTree)
		return false;

	outTree->clear();
	outTree->SetName(TREE_BOOK);
	outTree->AddAttrib(ATTRIB_BOOK_VERSION, GetCurrentDocVersion());
	outTree->AddAttrib(ATTRIB_BOOK_PGM_VERSION, inPgmVer);
	outTree->AddAttrib(ATTRIB_BOOK_PGM_PLATFORM, GetARBArch());
#if defined(__WXWINDOWS__)
	outTree->AddAttrib(ATTRIB_BOOK_PGM_OS, StringUtil::stringW(::wxGetOsDescription()));
#else
	outTree->AddAttrib(ATTRIB_BOOK_PGM_OS, L"TODO");
#endif
	outTree->AddAttrib(ATTRIB_BOOK_TIMESTAMP, GetTimeStamp());

	// Refresh cached "write-only" file info
	if (m_FileInfo.empty()) // A new file didn't initialize this
		m_FileInfo.insert(m_FileInfo.end(), static_cast<size_t>(ARBFileInfo::Max), std::wstring());
	outTree->GetAttrib(ATTRIB_BOOK_VERSION, m_FileInfo[static_cast<size_t>(ARBFileInfo::Book)]);
	outTree->GetAttrib(ATTRIB_BOOK_PGM_VERSION, m_FileInfo[static_cast<size_t>(ARBFileInfo::Version)]);
	outTree->GetAttrib(ATTRIB_BOOK_PGM_PLATFORM, m_FileInfo[static_cast<size_t>(ARBFileInfo::Platform)]);
	outTree->GetAttrib(ATTRIB_BOOK_PGM_OS, m_FileInfo[static_cast<size_t>(ARBFileInfo::OS)]);
	outTree->GetAttrib(ATTRIB_BOOK_TIMESTAMP, m_FileInfo[static_cast<size_t>(ARBFileInfo::TimeStamp)]);

	if (inCalendar)
	{
		if (!m_Calendar.Save(outTree))
			return false;
	}
	if (inTraining)
	{
		if (!m_Training.Save(outTree))
			return false;
	}
	if (inConfig || inDogs)
	{
		if (!m_Config.Save(outTree))
			return false;
	}
	if (inInfo)
	{
		if (!m_Info.Save(outTree))
			return false;
	}
	if (inDogs)
	{
		if (!m_Dogs.Save(outTree, m_Config))
			return false;
	}
	return true;
}


void ARBAgilityRecordBook::clear()
{
	m_Calendar.clear();
	m_Training.clear();
	m_Config.clear();
	m_Info.clear();
	m_Dogs.clear();
	m_FileInfo.clear();
}


void ARBAgilityRecordBook::Default(IARBConfigHandler const* inHandler)
{
	clear();
	m_Config.Default(inHandler);
}


bool ARBAgilityRecordBook::Update(
	int indent,
	ARBConfig const& inConfigNew,
	fmt::wmemory_buffer& ioInfo,
	IConfigActionCallback& ioCallBack)
{
	int curConfigVersion = m_Config.GetVersion();
	int nChanges = 0;
	if (0 < inConfigNew.GetActions().size())
		nChanges += inConfigNew.GetActions().Apply(m_Config, &m_Dogs, ioInfo, ioCallBack);

	bool bChanges = false;
	if (ioCallBack.CanContinue())
	{
		bChanges = m_Config.Update(indent, inConfigNew, ioInfo);
	}
	// m_Config is now updated.

	// In configuration 24, we added 'Team'. Originally, 'Pairs/Tournament' was
	// used to track Team Qs. Starting in v24, we're using it specifically for
	// the Relay run. To track Team Qs, use the new 'Team' event. So
	// automatically migrate pre-v24 Pair runs to 'Team'.
	bool bFixUSDAAPairs = false;
	if (curConfigVersion <= 23 && inConfigNew.GetVersion() >= 24)
	{
		ARBConfigVenuePtr venue, venueNew;
		if (m_Config.GetVenues().FindVenue(L"USDAA", &venue) && inConfigNew.GetVenues().FindVenue(L"USDAA", &venueNew))
		{
			ARBConfigEventPtr event, eventNew;
			if (venue->GetEvents().FindEvent(L"Pairs", &event) && venueNew->GetEvents().FindEvent(L"Team", &eventNew))
			{
				ARBDate d;
				if (event->VerifyEvent(WILDCARD_DIVISION, L"Nationals", d)
					|| event->VerifyEvent(WILDCARD_DIVISION, L"Tournament", d)
					|| eventNew->VerifyEvent(WILDCARD_DIVISION, L"Nationals", d)
					|| eventNew->VerifyEvent(WILDCARD_DIVISION, L"Tournament", d))
				{
					// Ok, the configuration passes...
					bFixUSDAAPairs = true;
				}
			}
		}
	}

	// Fix existing runs. Note, we need to do this regardless of whether the
	// user cancelled the update since some changes may have occurred that will
	// cause additional runs to be lost.
	for (ARBConfigVenueList::iterator iterVenue = m_Config.GetVenues().begin(); iterVenue != m_Config.GetVenues().end();
		 ++iterVenue)
	{
		// This actually just synchronizes multiQs.
		m_Dogs.DeleteMultiQs(m_Config, (*iterVenue)->GetName());
	}
	fmt::wmemory_buffer msgPairsRuns, msgDelRuns, msgTable, msgSubname;
	int nUpdatedPairsRuns = 0;
	int nDeletedRuns = 0;
	int nUpdatedTable = 0;
	int nUpdatedSubname = 0;
	for (ARBDogList::iterator iterDog = m_Dogs.begin(); iterDog != m_Dogs.end(); ++iterDog)
	{
		ARBDogPtr pDog = *iterDog;
		// Fix titles to deal with showing '1' on a recurring title.
		for (ARBDogTitleList::iterator iterTitle = pDog->GetTitles().begin(); iterTitle != pDog->GetTitles().end();
			 ++iterTitle)
		{
			ARBDogTitlePtr pTitle = *iterTitle;
			ARBConfigTitlePtr pConfigTitle;
			if (m_Config.GetVenues().FindTitle(pTitle->GetVenue(), pTitle->GetRawName(), &pConfigTitle))
			{
				short inst = pTitle->GetInstance();
				if (0 == inst && 1 < pConfigTitle->GetMultipleStartAt())
					inst = 1;
				pTitle->SetName(pTitle->GetRawName(), inst, pConfigTitle);
			}
		}
		for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin(); iterTrial != pDog->GetTrials().end();
			 ++iterTrial)
		{
			ARBDogTrialPtr pTrial = *iterTrial;
			for (ARBDogRunList::iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end();)
			{
				ARBDogRunPtr pRun = *iterRun;
				std::wstring venue = pRun->GetClub()->GetVenue();
				if (bFixUSDAAPairs && venue == L"USDAA" && pRun->GetEvent() == L"Pairs"
					&& (pRun->GetLevel() == L"Tournament" || pRun->GetLevel() == L"Nationals"))
				{
					// Move pairs run to new team
					pRun->SetEvent(L"Team");
					fmt::format_to(
						std::back_inserter(msgPairsRuns),
						L"   {} {} {} {}/{}\n",
						pRun->GetDate().GetString(ARBDateFormat::ISO),
						venue,
						pRun->GetEvent(),
						pRun->GetDivision(),
						pRun->GetLevel());
					++nUpdatedPairsRuns;
				}
				ARBConfigScoringPtr pScoring;
				if (m_Config.GetVenues().FindEvent(
						venue,
						pRun->GetEvent(),
						pRun->GetDivision(),
						pRun->GetLevel(),
						pRun->GetDate(),
						nullptr,
						&pScoring))
				{
					if (ARBDogRunScoring::TranslateConfigScoring(pScoring->GetScoringStyle())
						!= pRun->GetScoring().GetType())
					{
						pRun->GetScoring().SetType(
							ARBDogRunScoring::TranslateConfigScoring(pScoring->GetScoringStyle()),
							pScoring->DropFractions());
					}
					if (0 == pScoring->GetLifetimePoints().size() + pScoring->GetTitlePoints().size())
					{
						ARB_Q q = pRun->GetQ();
						if (!q.AllowForNonTitling())
						{
							// Titling points were removed. Reset the Q/NQ to NA.
							pRun->SetQ(Q::NA);
						}
					}
					if (!pScoring->HasTable() && pRun->GetScoring().HasTable())
					{
						pRun->GetScoring().SetHasTable(false);
						fmt::format_to(
							std::back_inserter(msgTable),
							L"   {} {} {} {}/{}\n",
							pRun->GetDate().GetString(ARBDateFormat::ISO),
							venue,
							pRun->GetEvent(),
							pRun->GetDivision(),
							pRun->GetLevel());
						++nUpdatedTable;
					}
					if (!pScoring->HasSubNames() && !pRun->GetSubName().empty())
					{
						pRun->SetSubName(std::wstring());
						fmt::format_to(
							std::back_inserter(msgSubname),
							L"   {} {} {} {}/{}\n",
							pRun->GetDate().GetString(ARBDateFormat::ISO),
							venue,
							pRun->GetEvent(),
							pRun->GetDivision(),
							pRun->GetLevel());
						++nUpdatedSubname;
					}
					++iterRun;
				}
				else
				{
					fmt::format_to(
						std::back_inserter(msgDelRuns),
						L"   {} {} {} {}/{}\n",
						pRun->GetDate().GetString(ARBDateFormat::ISO),
						venue,
						pRun->GetEvent(),
						pRun->GetDivision(),
						pRun->GetLevel());
					++nDeletedRuns;
					iterRun = pTrial->GetRuns().erase(iterRun);
				}
			}
		}
	}
	if (0 < nUpdatedPairsRuns)
	{
		nChanges += nUpdatedPairsRuns;
		std::wstring msg = Localization()->UpdateTeamRuns(nUpdatedPairsRuns, fmt::to_string(msgPairsRuns));
		fmt::format_to(std::back_inserter(ioInfo), L"\n{}\n", msg);
	}
	if (0 < nDeletedRuns)
	{
		nChanges += nDeletedRuns;
		std::wstring msg = Localization()->WarnDeletedRuns(nDeletedRuns, fmt::to_string(msgDelRuns));
		ioCallBack.PostDelete(msg);
		fmt::format_to(std::back_inserter(ioInfo), L"\n{}\n", msg);
	}
	if (0 < nUpdatedTable)
	{
		nChanges += nUpdatedTable;
		std::wstring msg = Localization()->UpdateTableRuns(nUpdatedTable, fmt::to_string(msgTable));
		fmt::format_to(std::back_inserter(ioInfo), L"\n{}\n", msg);
	}
	if (0 < nUpdatedSubname)
	{
		nChanges += nUpdatedSubname;
		std::wstring msg = Localization()->UpdateSubnameRuns(nUpdatedSubname, fmt::to_string(msgSubname));
		fmt::format_to(std::back_inserter(ioInfo), L"\n{}\n", msg);
	}

	// This fixup is only done when upgrading from Config version 2 to 3.
	// Later note: I don't know why it was restricted to 2->3. If we're going
	// 2->4, we still need to do the conversion.
	if (ioCallBack.CanContinue() && curConfigVersion <= 2 && inConfigNew.GetVersion() >= 3)
	{
		int nUpdated = 0;
		for (ARBConfigVenueList::iterator iterVenue = m_Config.GetVenues().begin();
			 iterVenue != m_Config.GetVenues().end();
			 ++iterVenue)
		{
			ARBConfigVenuePtr pVenue = *iterVenue;
			for (ARBConfigEventList::iterator iterEvent = pVenue->GetEvents().begin();
				 iterEvent != pVenue->GetEvents().end();
				 ++iterEvent)
			{
				ARBConfigEventPtr pEvent = *iterEvent;
				// For every event that has a table listed, fix all those runs.
				// - Note, this must actually be done for ALL runs in order to
				// clear the table flag on non-table events.
				for (ARBDogList::iterator iterDog = m_Dogs.begin(); iterDog != m_Dogs.end(); ++iterDog)
				{
					ARBDogPtr pDog = *iterDog;
					for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin();
						 iterTrial != pDog->GetTrials().end();
						 ++iterTrial)
					{
						ARBDogTrialPtr pTrial = *iterTrial;
						if (pTrial->GetClubs().FindVenue(pVenue->GetName()))
						{
							for (ARBDogRunList::iterator iterRun = pTrial->GetRuns().begin();
								 iterRun != pTrial->GetRuns().end();
								 ++iterRun)
							{
								ARBDogRunPtr pRun = *iterRun;
								if (pRun->GetEvent() == pEvent->GetName() && pRun->GetScoring().TableNeedsConverting())
								{
									std::wstring level(pRun->GetLevel());
									ARBConfigDivisionPtr div;
									if (pVenue->GetDivisions().FindDivision(pRun->GetDivision(), &div))
									{
										ARBConfigLevelPtr pLevel;
										if (div->GetLevels().FindSubLevel(pRun->GetLevel(), &pLevel))
											level = pLevel->GetName();
									}
									// Specifically set whether the run has a
									// table based on the configuration. This
									// makes sure that old runs marked as having
									// a table get properly cleaned up.
									ARBConfigScoringPtr scoring;
									if (pEvent->GetScorings()
											.FindEvent(pRun->GetDivision(), level, pRun->GetDate(), &scoring))
									{
										if (pRun->GetScoring().HasTable() != scoring->HasTable())
										{
											++nUpdated;
											pRun->GetScoring().SetHasTable(scoring->HasTable());
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if (0 < nUpdated)
		{
			nChanges += nUpdated;
			fmt::format_to(std::back_inserter(ioInfo), L"{}\n", Localization()->UpdateTableRuns(nUpdated));
		}
	}

	if (0 < nChanges || bChanges)
	{
		bChanges = true;
		m_Dogs.SetMultiQs(m_Config);
	}

	return bChanges;
}


size_t ARBAgilityRecordBook::GetAllClubNames(std::set<std::wstring>& outClubs, bool bInfo, bool bVisibleOnly) const
{
	outClubs.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin(); iterDog != m_Dogs.end(); ++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			 iterTrial != pDog->GetTrials().end();
			 ++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogClubList::const_iterator iterClub = pTrial->GetClubs().begin();
				 iterClub != pTrial->GetClubs().end();
				 ++iterClub)
			{
				ARBDogClubPtr pClub = (*iterClub);
				if (0 < pClub->GetName().length())
					outClubs.insert(pClub->GetName());
			}
		}
	}
	for (ARBCalendarList::const_iterator iterCal = m_Calendar.begin(); iterCal != m_Calendar.end(); ++iterCal)
	{
		ARBCalendarPtr pCal = *iterCal;
		if (0 < pCal->GetClub().length())
			outClubs.insert(pCal->GetClub());
	}
	if (bInfo)
		m_Info.GetInfo(ARBInfoType::Club).GetAllItems(outClubs, bVisibleOnly);
	return outClubs.size();
}


size_t ARBAgilityRecordBook::GetAllTrialLocations(std::set<std::wstring>& outLocations, bool bInfo, bool bVisibleOnly)
	const
{
	outLocations.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin(); iterDog != m_Dogs.end(); ++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			 iterTrial != pDog->GetTrials().end();
			 ++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			if (0 < pTrial->GetLocation().length())
				outLocations.insert(pTrial->GetLocation());
		}
	}
	for (ARBCalendarList::const_iterator iterCal = m_Calendar.begin(); iterCal != m_Calendar.end(); ++iterCal)
	{
		ARBCalendarPtr pCal = *iterCal;
		if (0 < pCal->GetLocation().length())
			outLocations.insert(pCal->GetLocation());
	}
	if (bInfo)
		m_Info.GetInfo(ARBInfoType::Location).GetAllItems(outLocations, bVisibleOnly);
	return outLocations.size();
}


size_t ARBAgilityRecordBook::GetAllEventSubNames(
	std::wstring const& inVenue,
	ARBConfigEventPtr const& inEvent,
	std::set<std::wstring>& outNames) const
{
	outNames.clear();
	if (!inEvent || 0 == inEvent->GetScorings().size())
		return 0;
	for (auto iter = inEvent->GetScorings().begin(); iter != inEvent->GetScorings().end(); ++iter)
	{
		if ((*iter)->HasSubNames())
			(*iter)->GetSubNames(outNames, false);
	}
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin(); iterDog != m_Dogs.end(); ++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			 iterTrial != pDog->GetTrials().end();
			 ++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			if (pTrial->HasVenue(inVenue))
			{
				for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
					 iterRun != pTrial->GetRuns().end();
					 ++iterRun)
				{
					ARBDogRunPtr pRun = (*iterRun);
					if (pRun->GetEvent() == inEvent->GetName())
					{
						if (0 < pRun->GetSubName().length())
							outNames.insert(pRun->GetSubName());
					}
				}
			}
		}
	}
	return outNames.size();
}


size_t ARBAgilityRecordBook::GetAllHeights(std::set<std::wstring>& outHeights) const
{
	outHeights.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin(); iterDog != m_Dogs.end(); ++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			 iterTrial != pDog->GetTrials().end();
			 ++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end();
				 ++iterRun)
			{
				ARBDogRunPtr pRun = (*iterRun);
				if (0 < pRun->GetHeight().length())
					outHeights.insert(pRun->GetHeight());
				for (ARBDogReferenceRunList::const_iterator iterRef = pRun->GetReferenceRuns().begin();
					 iterRef != pRun->GetReferenceRuns().end();
					 ++iterRef)
				{
					ARBDogReferenceRunPtr pRef = *iterRef;
					if (0 < pRef->GetHeight().length())
						outHeights.insert(pRef->GetHeight());
				}
			}
		}
	}
	return outHeights.size();
}


size_t ARBAgilityRecordBook::GetAllCallNames(std::set<std::wstring>& outNames) const
{
	outNames.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin(); iterDog != m_Dogs.end(); ++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		outNames.insert(pDog->GetCallName());
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			 iterTrial != pDog->GetTrials().end();
			 ++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end();
				 ++iterRun)
			{
				ARBDogRunPtr pRun = (*iterRun);
				for (ARBDogReferenceRunList::const_iterator iterRef = pRun->GetReferenceRuns().begin();
					 iterRef != pRun->GetReferenceRuns().end();
					 ++iterRef)
				{
					ARBDogReferenceRunPtr pRef = *iterRef;
					if (0 < pRef->GetName().length())
						outNames.insert(pRef->GetName());
				}
			}
		}
	}
	return outNames.size();
}


size_t ARBAgilityRecordBook::GetAllBreeds(std::set<std::wstring>& outBreeds) const
{
	outBreeds.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin(); iterDog != m_Dogs.end(); ++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		if (0 < pDog->GetBreed().length())
			outBreeds.insert(pDog->GetBreed());
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			 iterTrial != pDog->GetTrials().end();
			 ++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end();
				 ++iterRun)
			{
				ARBDogRunPtr pRun = (*iterRun);
				for (ARBDogReferenceRunList::const_iterator iterRef = pRun->GetReferenceRuns().begin();
					 iterRef != pRun->GetReferenceRuns().end();
					 ++iterRef)
				{
					ARBDogReferenceRunPtr pRef = *iterRef;
					if (0 < pRef->GetBreed().length())
						outBreeds.insert(pRef->GetBreed());
				}
			}
		}
	}
	return outBreeds.size();
}


size_t ARBAgilityRecordBook::GetAllJudges(std::set<std::wstring>& outJudges, bool bInfo, bool bVisibleOnly) const
{
	outJudges.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin(); iterDog != m_Dogs.end(); ++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			 iterTrial != pDog->GetTrials().end();
			 ++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end();
				 ++iterRun)
			{
				ARBDogRunPtr pRun = (*iterRun);
				if (0 < pRun->GetJudge().length())
					outJudges.insert(pRun->GetJudge());
			}
		}
	}
	if (bInfo)
		m_Info.GetInfo(ARBInfoType::Judge).GetAllItems(outJudges, bVisibleOnly);
	return outJudges.size();
}


size_t ARBAgilityRecordBook::GetAllHandlers(std::set<std::wstring>& outHandlers) const
{
	outHandlers.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin(); iterDog != m_Dogs.end(); ++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			 iterTrial != pDog->GetTrials().end();
			 ++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end();
				 ++iterRun)
			{
				ARBDogRunPtr pRun = (*iterRun);
				if (0 < pRun->GetHandler().length())
					outHandlers.insert(pRun->GetHandler());
			}
		}
	}
	return outHandlers.size();
}


void ARBAgilityRecordBook::GetAllPartners(std::set<std::wstring>& outPartners, std::set<std::wstring>& outDogs) const
{
	outPartners.clear();
	outDogs.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin(); iterDog != m_Dogs.end(); ++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			 iterTrial != pDog->GetTrials().end();
			 ++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end();
				 ++iterRun)
			{
				ARBDogRunPtr pRun = (*iterRun);
				for (ARBDogRunPartnerList::const_iterator iterPartner = pRun->GetPartners().begin();
					 iterPartner != pRun->GetPartners().end();
					 ++iterPartner)
				{
					ARBDogRunPartnerPtr pPartner = *iterPartner;
					if (0 < pPartner->GetHandler().length())
						outPartners.insert(pPartner->GetHandler());
					if (0 < pPartner->GetDog().length())
						outDogs.insert(pPartner->GetDog());
				}
			}
		}
	}
}


size_t ARBAgilityRecordBook::GetAllFaultTypes(std::set<std::wstring>& outFaults) const
{
	outFaults.clear();
	for (ARBConfigFaultList::const_iterator iterFault = m_Config.GetFaults().begin();
		 iterFault != m_Config.GetFaults().end();
		 ++iterFault)
	{
		ARBConfigFaultPtr pFault = (*iterFault);
		if (0 < pFault->GetName().length())
			outFaults.insert(pFault->GetName());
	}
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin(); iterDog != m_Dogs.end(); ++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			 iterTrial != pDog->GetTrials().end();
			 ++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end();
				 ++iterRun)
			{
				ARBDogRunPtr pRun = (*iterRun);
				for (ARBDogFaultList::const_iterator iterDogFault = pRun->GetFaults().begin();
					 iterDogFault != pRun->GetFaults().end();
					 ++iterDogFault)
				{
					if (0 < (*iterDogFault).length())
						outFaults.insert((*iterDogFault));
				}
			}
		}
	}
	return outFaults.size();
}


ARBCalendarList const& ARBAgilityRecordBook::GetCalendar() const
{
	return m_Calendar;
}


ARBCalendarList& ARBAgilityRecordBook::GetCalendar()
{
	return m_Calendar;
}


ARBTrainingList const& ARBAgilityRecordBook::GetTraining() const
{
	return m_Training;
}


ARBTrainingList& ARBAgilityRecordBook::GetTraining()
{
	return m_Training;
}


ARBConfig const& ARBAgilityRecordBook::GetConfig() const
{
	return m_Config;
}


ARBConfig& ARBAgilityRecordBook::GetConfig()
{
	return m_Config;
}


ARBInfo const& ARBAgilityRecordBook::GetInfo() const
{
	return m_Info;
}


ARBInfo& ARBAgilityRecordBook::GetInfo()
{
	return m_Info;
}


ARBDogList const& ARBAgilityRecordBook::GetDogs() const
{
	return m_Dogs;
}


ARBDogList& ARBAgilityRecordBook::GetDogs()
{
	return m_Dogs;
}

} // namespace ARB
} // namespace dconSoft
