/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
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
 * @li 2009-12-18 DRC File version 12.11
 *                    Added 'O' to 'Entry'.
 * @li 2009-07-26 DRC File version 12.10
 *                    Added 'DNR' to 'Placement' and 'ReferenceRun'
 * @li 2008-02-12 DRC File version 12.9
 *                    Fix metadata bug in dognotes.
 * @li 2008-08-05 DRC File version 12.8
 *                    Added 'style' to Title, 'Style' to 'Titles'
 * @li 2008-01-01 DRC File version 12.7. Added 'Visible' to InfoItem,
 *                    'obstacles' to 'Scoring'
 * @li 2007-07-03 DRC File version 12.6. Added 'show' to 'Title',
 *                    'timestamp' to 'AgilityBook'
 * @li 2007-04-22 DRC Added 'Accom', 'Confirm' to 'Calendar', 'icon' to 'Venue'
 * @li 2007-02-27 DRC File version 12.5. Added 'defValue' to 'OtherPts'.
 * @li 2007-01-03 DRC File version 12.4. Added 'DateDraw' to Calendar.
 *                    Changed 'CRCDMeta' to 'CRCDMeta2'.
 * @li 2006-11-03 DRC File version 12.3. Added 'SpdMult'.
 * @li 2006-07-02 DRC File version 12.2. Added 'subtractTF'.
 * @li 2006-02-16 DRC File version 12.1. Added 'RenameEvent'.
 *                    Allow fractional faults in title points.
 *                    Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC File version 12.0. Moved 'Titles' to 'Venue'.
 * @li 2005-10-14 DRC File version 11.1. Added 'Prefix' to 'Titles'.
 * @li 2005-07-15 DRC File version 11.0. Changed how Double-Qs are done.
 * @li 2005-04-26 DRC File version 10.2. Added 'ver' to 'AgilityBook'
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2004-11-15 DRC File version 10.1. Added 'underTF'/'overTF' to 'Scoring'.
 * @li 2004-10-06 DRC File version 10. Added 'Height' to 'ReferenceRun'
 *                    GetAllHeights now accumulates these too. Changed
 *                    lifetime points.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-06-29 DRC File version 9.0. Added 'SubName' to 'Training'. Changed
 *                    structure of 'RegNum'.
 * @li 2004-03-26 DRC File version 8.6. Changed Table-in-YPS to hasTable.
 * @li 2004-03-13 DRC File version 8.5. Added 'isHidden' to 'Title'.
 * @li 2004-02-15 DRC Cleaned up newline in warning msg.
 * @li 2004-02-14 DRC File version 8.4. Added Table-in-YPS flag.
 * @li 2004-02-02 DRC File version 8.3. Added ARBDogExistingPoints items.
 * @li 2004-01-21 DRC File version 8.2. Added ConfigTitles LongName. Added
 *                    ARBConfigAction.
 * @li 2003-12-29 DRC GetAllClubs/Locations now includes calendar entries.
 * @li 2003-12-27 DRC File version 8.1. Changed ARBConfigScoring.
 * @li 2003-12-07 DRC File version 8.0. Added Info section.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 *                    Added warning check when minor versions are different.
 * @li 2003-10-31 DRC Added options to Save() to allow partial saves.
 * @li 2003-10-13 DRC File version 7. Added course faults to ByScore/etc.
 * @li 2003-09-21 DRC File version 6. Added training log.
 * @li 2003-07-24 DRC Removed built-in sort on dogs. Dogs are user-sorted now.
 * @li 2003-06-23 DRC File version 5. Added 'Note' and 'Verified' to trials.
 *                    Changed how title points are configured.
 * @li 2003-06-11 DRC File version 4. Added 'dropFractions' to config scoring.
 *                    Added 'Desc' to config venue and event.
 * @li 2003-04-21 DRC File version 3. Added notes to config events. Moved
 *                    faulttypes and otherpoints from venue to main config.
 *                    Added Receieved field to Titles.
 * @li 2003-01-23 DRC Bumped file version to 2: see ARBCalendar.cpp
 */

#include "stdafx.h"
#include "ARBAgilityRecordBook.h"

#include "ARBConfig.h"
#include "ARBDog.h"
#include "ARBLocalization.h"
#include "ARBTypes.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

ARBVersion const& ARBAgilityRecordBook::GetCurrentDocVersion()
{
	// Note, when bumping to the next version - DO NOT bump to a 7.x.
	// V0.9.3.7 can read 7.x files, but will not issue the warning about
	// possible data loss.
	static ARBVersion const curVersion(12, 11);
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
		ElementNodePtr inTree,
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

	// The version of the document must be something we understand.
	ARBVersion version;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_BOOK_VERSION, version))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_BOOK, ATTRIB_BOOK_VERSION));
		return false;
	}
	if (version < ARBVersion(1,0) || version > GetCurrentDocVersion())
	{
		if (version.Major() == GetCurrentDocVersion().Major())
		{
			if (!ioCallback.OnError(Localization()->WarningNewerDoc()))
				return false;
		}
		else
		{
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_BOOK, ATTRIB_BOOK_VERSION, Localization()->UnknownVersion()));
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
		int i;
		int nConfig = -1;
		// Find a configuration.
		for (i = 0; i < inTree->GetElementCount(); ++i)
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
			for (i = 0; i < inTree->GetElementCount(); ++i)
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

#ifndef _WIN32
#ifdef UNICODE
#define _tcsftime	wcsftime
#else
#define _tcsftime	strftime
#endif
#endif

static wxString GetTimeStamp()
{
	time_t t;
	time(&t);
	struct tm* pTime = NULL;
#if defined(ARB_HAS_SECURE_LOCALTIME)
	struct tm l;
	if (0 == _localtime64_s(&l, &t))
		pTime = &l;
#else
	pTime = localtime(&t);
#endif
	wxChar szBuffer[128]; // as defined by VC9 ATL::maxTimeBufferSize
	if (!pTime || !_tcsftime(szBuffer, 128, wxT("%Y-%m-%d %H:%M:%S"), pTime))
	{
		szBuffer[0] = '\0';
	}
	return wxString(szBuffer);
}


bool ARBAgilityRecordBook::Save(ElementNodePtr outTree,
		wxString const& inPgmVer,
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
	outTree->AddAttrib(ATTRIB_BOOK_TIMESTAMP, GetTimeStamp());
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
		if (!m_Dogs.Save(outTree))
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
}


void ARBAgilityRecordBook::Default(IARBConfigHandler* inHandler)
{
	clear();
	m_Config.Default(inHandler);
}


bool ARBAgilityRecordBook::Update(
		int indent,
		ARBConfig const& inConfigNew,
		wxString& ioInfo,
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

	// In configuration 24, we added 'Team'. Originally, 'Pairs/Tournament' was
	// used to track Team Qs. Starting in v24, we're using it specifically for
	// the Relay run. To track Team Qs, use the new 'Team' event. So
	// automatically migrate pre-v24 Pair runs to 'Team'.
	bool bFixUSDAAPairs = false;
	if (curConfigVersion <= 23 && inConfigNew.GetVersion() >= 24)
	{
		ARBConfigVenuePtr venue, venueNew;
		if (m_Config.GetVenues().FindVenue(wxT("USDAA"), &venue) && inConfigNew.GetVenues().FindVenue(wxT("USDAA"), &venueNew))
		{
			ARBConfigEventPtr event, eventNew;
			if (venue->GetEvents().FindEvent(wxT("Pairs"), &event) && venueNew->GetEvents().FindEvent(wxT("Team"), &eventNew))
			{
				ARBDate d;
				if (event->VerifyEvent(WILDCARD_DIVISION, wxT("Nationals"), d)
				|| event->VerifyEvent(WILDCARD_DIVISION, wxT("Tournament"), d)
				|| eventNew->VerifyEvent(WILDCARD_DIVISION, wxT("Nationals"), d)
				|| eventNew->VerifyEvent(WILDCARD_DIVISION, wxT("Tournament"), d))
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
	for (ARBConfigVenueList::iterator iterVenue = m_Config.GetVenues().begin();
		iterVenue != m_Config.GetVenues().end();
		++iterVenue)
	{
		// This actually just synchronizes multiQs.
		m_Dogs.DeleteMultiQs(m_Config, (*iterVenue)->GetName());
	}
	wxString msgPairsRuns, msgDelRuns;
	int nUpdatedPairsRuns = 0;
	int nDeletedRuns = 0;
	for (ARBDogList::iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		ARBDogPtr pDog = *iterDog;
		// Fix titles to deal with showing '1' on a recurring title.
		for (ARBDogTitleList::iterator iterTitle = pDog->GetTitles().begin();
			iterTitle != pDog->GetTitles().end();
			++iterTitle)
		{
			ARBDogTitlePtr pTitle = *iterTitle;
			ARBConfigTitlePtr pConfigTitle;
			if (m_Config.GetVenues().FindTitle(pTitle->GetVenue(), pTitle->GetRawName(), &pConfigTitle))
				pTitle->SetName(pTitle->GetRawName(), pTitle->GetInstance(), pConfigTitle->GetMultiple() == 1, pConfigTitle->GetMultipleStyle());
		}
		for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			ARBDogTrialPtr pTrial = *iterTrial;
			if (!pTrial->GetClubs().GetPrimaryClub())
				continue;
			wxString venue = pTrial->GetClubs().GetPrimaryClubVenue();
			for (ARBDogRunList::iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
				)
			{
				ARBDogRunPtr pRun = *iterRun;
				if (bFixUSDAAPairs && venue == wxT("USDAA") && pRun->GetEvent() == wxT("Pairs")
				&& (pRun->GetLevel() == wxT("Tournament") || pRun->GetLevel() == wxT("Nationals")))
				{
					// Move pairs run to new team
					pRun->SetEvent(wxT("Team"));
					msgPairsRuns << wxT("   ")
						<< pRun->GetDate().GetString(ARBDate::eISO)
						<< wxT(" ")
						<< venue
						<< wxT(" ")
						<< pRun->GetEvent()
						<< wxT(" ")
						<< pRun->GetDivision()
						<< wxT("/")
						<< pRun->GetLevel()
						<< wxT("\n");
					++nUpdatedPairsRuns;
				}
				ARBConfigScoringPtr pScoring;
				if (m_Config.GetVenues().FindEvent(
					venue,
					pRun->GetEvent(),
					pRun->GetDivision(),
					pRun->GetLevel(),
					pRun->GetDate(),
					NULL,
					&pScoring))
				{
					if (ARBDogRunScoring::TranslateConfigScoring(pScoring->GetScoringStyle())
						!= pRun->GetScoring().GetType())
					{
						pRun->GetScoring().SetType(ARBDogRunScoring::TranslateConfigScoring(pScoring->GetScoringStyle()), pScoring->DropFractions());
					}
					++iterRun;
				}
				else
				{
					msgDelRuns << wxT("   ")
						<< pRun->GetDate().GetString(ARBDate::eISO)
						<< wxT(" ")
						<< venue
						<< wxT(" ")
						<< pRun->GetEvent()
						<< wxT(" ")
						<< pRun->GetDivision()
						<< wxT("/")
						<< pRun->GetLevel()
						<< wxT("\n");
					++nDeletedRuns;
					iterRun = pTrial->GetRuns().erase(iterRun);
				}
			}
		}
	}
	if (0 < nUpdatedPairsRuns)
	{
		nChanges += nUpdatedPairsRuns;
		wxString msg = Localization()->UpdateTeamRuns(nUpdatedPairsRuns, msgPairsRuns);
		ioInfo << wxT("\n") << msg << wxT("\n");
	}
	if (0 < nDeletedRuns)
	{
		nChanges += nDeletedRuns;
		wxString msg = Localization()->WarnDeletedRuns(nDeletedRuns, msgDelRuns);
		ioCallBack.PostDelete(msg);
		ioInfo << wxT("\n") << msg << wxT("\n");
	}

	// This fixup is only done when upgrading from Config version 2 to 3.
	// Later note: I don't know why it was restricted to 2->3. If we're going
	// 2->4, we still need to do the conversion.
	if (ioCallBack.CanContinue()
	&& curConfigVersion <= 2 && inConfigNew.GetVersion() >= 3)
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
				for (ARBDogList::iterator iterDog = m_Dogs.begin();
					iterDog != m_Dogs.end();
					++iterDog)
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
								if (pRun->GetEvent() == pEvent->GetName()
								&& pRun->GetScoring().TableNeedsConverting())
								{
									// Specifically set whether the run has a
									// table based on the configuration. This
									// makes sure that old runs marked as having
									// a table get properly cleaned up.
									if (pRun->GetScoring().HasTable() != pEvent->HasTable())
									{
										++nUpdated;
										pRun->GetScoring().SetHasTable(pEvent->HasTable());
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
			ioInfo << Localization()->UpdateTableRuns(nUpdated)
				<< wxT("\n");
		}
	}

	if (0 < nChanges || bChanges)
	{
		bChanges = true;
		m_Dogs.SetMultiQs(m_Config);
	}

	return bChanges;
}


size_t ARBAgilityRecordBook::GetAllClubNames(
		std::set<wxString>& outClubs,
		bool bInfo,
		bool bVisibleOnly) const
{
	outClubs.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
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
	for (ARBCalendarList::const_iterator iterCal = m_Calendar.begin();
		iterCal != m_Calendar.end();
		++iterCal)
	{
		ARBCalendarPtr pCal = *iterCal;
		if (0 < pCal->GetClub().length())
			outClubs.insert(pCal->GetClub());
	}
	if (bInfo)
		m_Info.GetInfo(ARBInfo::eClubInfo).GetAllItems(outClubs, bVisibleOnly);
	return outClubs.size();
}


size_t ARBAgilityRecordBook::GetAllTrialLocations(
		std::set<wxString>& outLocations,
		bool bInfo,
		bool bVisibleOnly) const
{
	outLocations.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
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
	for (ARBCalendarList::const_iterator iterCal = m_Calendar.begin();
		iterCal != m_Calendar.end();
		++iterCal)
	{
		ARBCalendarPtr pCal = *iterCal;
		if (0 < pCal->GetLocation().length())
			outLocations.insert(pCal->GetLocation());
	}
	if (bInfo)
		m_Info.GetInfo(ARBInfo::eLocationInfo).GetAllItems(outLocations, bVisibleOnly);
	return outLocations.size();
}


size_t ARBAgilityRecordBook::GetAllEventSubNames(
		wxString const& inVenue,
		ARBConfigEventPtr inEvent,
		std::set<wxString>& outNames) const
{
	outNames.clear();
	if (!inEvent || !inEvent->HasSubNames())
		return 0;
	inEvent->GetSubNames(outNames);
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
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


size_t ARBAgilityRecordBook::GetAllHeights(std::set<wxString>& outHeights) const
{
	outHeights.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
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


size_t ARBAgilityRecordBook::GetAllCallNames(std::set<wxString>& outNames) const
{
	outNames.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		outNames.insert(pDog->GetCallName());
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
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


size_t ARBAgilityRecordBook::GetAllBreeds(std::set<wxString>& outBreeds) const
{
	outBreeds.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		if (0 < pDog->GetBreed().length())
			outBreeds.insert(pDog->GetBreed());
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
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


size_t ARBAgilityRecordBook::GetAllJudges(
		std::set<wxString>& outJudges,
		bool bInfo,
		bool bVisibleOnly) const
{
	outJudges.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
				++iterRun)
			{
				ARBDogRunPtr pRun = (*iterRun);
				if (0 < pRun->GetJudge().length())
					outJudges.insert(pRun->GetJudge());
			}
		}
	}
	if (bInfo)
		m_Info.GetInfo(ARBInfo::eJudgeInfo).GetAllItems(outJudges, bVisibleOnly);
	return outJudges.size();
}


size_t ARBAgilityRecordBook::GetAllHandlers(std::set<wxString>& outHandlers) const
{
	outHandlers.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
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


void ARBAgilityRecordBook::GetAllPartners(
		std::set<wxString>& outPartners,
		std::set<wxString>& outDogs) const
{
	outPartners.clear();
	outDogs.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
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


size_t ARBAgilityRecordBook::GetAllFaultTypes(std::set<wxString>& outFaults) const
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
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		ARBDogPtr pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
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
