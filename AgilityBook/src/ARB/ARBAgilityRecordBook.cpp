/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 *
 * @brief The main data class.
 * @author David Connet
 *
 * This class is separated from the Win32 CDocument class to help anyone who
 * tries to port this to a different platform or put a different GUI on it.
 *
 * Revision History
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

#include "StdAfx.h"
#include "ARBAgilityRecordBook.h"

#include "ARBConfig.h"
#include "ARBDog.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

short ARBAgilityRecordBook::GetCurrentDocVersion()
{
	static const short curVersion = 6;
	return curVersion;
}

/////////////////////////////////////////////////////////////////////////////
// These error routines centralize platform dependent code.

void ErrorInvalidDocStructure(const char* const inMsg)
{
	std::string str(INVALID_DOC_STRUCTURE);
	if (inMsg)
	{
		str += ": ";
		str += inMsg;
	}
#ifdef WIN32
	AfxMessageBox(str.c_str());
#else
	cerr << str.c_str() << endl;
#endif
}

void ErrorMissingAttribute(
	const char* const inElement,
	const char* const inAttrib,
	const char* const inMsg)
{
	std::string str(INVALID_FILE_FORMAT);
	str += inElement;
	str += INVALID_FILE_MISSING_ATTRIB;
	str += inAttrib;
	str += "'.";
	if (inMsg)
	{
		str += "\n";
		str += inMsg;
	}
#ifdef WIN32
	AfxMessageBox(str.c_str());
#else
	cerr << str.c_str() << endl;
#endif
}

void ErrorInvalidAttributeValue(
	const char* const inElement,
	const char* const inAttrib,
	const char* const inMsg)
{
	std::string str(INVALID_FILE_FORMAT);
	str += inElement;
	str += INVALID_FILE_BAD_ATTRIB;
	str += inAttrib;
	str += "'.";
	if (inMsg)
	{
		str += "\n";
		str += inMsg;
	}
#ifdef WIN32
	AfxMessageBox(str.c_str());
#else
	cerr << str.c_str() << endl;
#endif
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
bool ARBAgilityRecordBook::Load(
	const CElement& inTree,
	bool inCalendar,
	bool inTraining,
	bool inConfig,
	bool inDogs)
{
	// Get the records ready.
	clear();

	// Make sure the input looks okay.
	// The root must be TREE_BOOK.
	if (inTree.GetName() != TREE_BOOK)
	{
		ErrorInvalidDocStructure(INVALID_ROOT);
		return false;
	}
	// The version of the document must be something we understand.
	short version;
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_BOOK_VERSION, version))
	{
		ErrorMissingAttribute(TREE_BOOK, ATTRIB_BOOK_VERSION);
		return false;
	}
	if (1 > version || version > GetCurrentDocVersion())
	{
		ErrorInvalidAttributeValue(TREE_BOOK, ATTRIB_BOOK_VERSION, UNKNOWN_VERSION);
		return false;
	}

	// Something was loaded.
	bool bLoaded = false;

	// Load the calendar information.
	if (inCalendar)
	{
		bLoaded = true;
		for (int i = 0; i < inTree.GetElementCount(); ++i)
		{
			const CElement& element = inTree.GetElement(i);
			if (element.GetName() == TREE_CALENDAR)
			{
				// Ignore any errors...
				m_Calendar.Load(element, version);
			}
		}
		m_Calendar.sort();
	}

	if (inTraining)
	{
		bLoaded = true;
		for (int i = 0; i < inTree.GetElementCount(); ++i)
		{
			const CElement& element = inTree.GetElement(i);
			if (element.GetName() == TREE_TRAINING)
			{
				// Ignore any errors...
				m_Training.Load(element, version);
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
		// Find a configuration and make sure there's only one.
		for (i = 0; i < inTree.GetElementCount(); ++i)
		{
			if (inTree.GetElement(i).GetName() == TREE_CONFIG)
			{
				if (-1 != nConfig)
				{
					ErrorInvalidDocStructure(INVALID_CONFIG);
					return false;
				}
				nConfig = i;
			}
		}
		// Oops. No config.
		if (0 > nConfig)
		{
			ErrorInvalidDocStructure(MISSING_CONFIG);
			return false;
		}
		// Load the config.
		else if (!m_Config.Load(inTree.GetElement(nConfig), version))
		{
			// Error message was printed within.
			return false;
		}

		// Now, we can load the dog's information.
		if (inDogs)
		{
			// Now load the rest
			for (i = 0; i < inTree.GetElementCount(); ++i)
			{
				const CElement& element = inTree.GetElement(i);
				if (element.GetName() == TREE_DOG)
				{
					// If this fails, keep going.
					// We'll try to load whatever we can.
					m_Dogs.Load(m_Config, element, version);
				}
			}
		}
	}
	return bLoaded;
}

bool ARBAgilityRecordBook::Save(CElement& outTree) const
{
	outTree.clear();
	outTree.SetName(TREE_BOOK);
	outTree.AddAttrib(ATTRIB_BOOK_VERSION, GetCurrentDocVersion());
	if (!m_Calendar.Save(outTree))
		return false;
	if (!m_Training.Save(outTree))
		return false;
	if (!m_Config.Save(outTree))
		return false;
	if (!m_Dogs.Save(outTree))
		return false;
	return true;
}

void ARBAgilityRecordBook::clear()
{
	m_Calendar.clear();
	m_Training.clear();
	m_Config.clear();
	m_Dogs.clear();
}

void ARBAgilityRecordBook::Default()
{
	clear();
	m_Config.Default();
}

size_t ARBAgilityRecordBook::GetAllClubNames(std::set<std::string>& outClubs) const
{
	outClubs.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		const ARBDog* pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			const ARBDogTrial* pTrial = (*iterTrial);
			for (ARBDogClubList::const_iterator iterClub = pTrial->GetClubs().begin();
				iterClub != pTrial->GetClubs().end();
				++iterClub)
			{
				const ARBDogClub* pClub = (*iterClub);
				if (0 < pClub->GetName().length())
					outClubs.insert(pClub->GetName());
			}
		}
	}
	return outClubs.size();
}

size_t ARBAgilityRecordBook::GetAllTrialLocations(std::set<std::string>& outLocations) const
{
	outLocations.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		const ARBDog* pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			const ARBDogTrial* pTrial = (*iterTrial);
			if (0 < pTrial->GetLocation().length())
				outLocations.insert(pTrial->GetLocation());
		}
	}
	return outLocations.size();
}

size_t ARBAgilityRecordBook::GetAllHeights(std::set<std::string>& outHeights) const
{
	outHeights.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		const ARBDog* pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			const ARBDogTrial* pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
				++iterRun)
			{
				const ARBDogRun* pRun = (*iterRun);
				if (0 < pRun->GetHeight().length())
					outHeights.insert(pRun->GetHeight());
			}
		}
	}
	return outHeights.size();
}

size_t ARBAgilityRecordBook::GetAllJudges(std::set<std::string>& outJudges) const
{
	outJudges.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		const ARBDog* pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			const ARBDogTrial* pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
				++iterRun)
			{
				const ARBDogRun* pRun = (*iterRun);
				if (0 < pRun->GetJudge().length())
					outJudges.insert(pRun->GetJudge());
			}
		}
	}
	return outJudges.size();
}

size_t ARBAgilityRecordBook::GetAllHandlers(std::set<std::string>& outHandlers) const
{
	outHandlers.clear();
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		const ARBDog* pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			const ARBDogTrial* pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
				++iterRun)
			{
				const ARBDogRun* pRun = (*iterRun);
				if (0 < pRun->GetHandler().length())
					outHandlers.insert(pRun->GetHandler());
			}
		}
	}
	return outHandlers.size();
}

size_t ARBAgilityRecordBook::GetAllFaultTypes(std::set<std::string>& outFaults) const
{
	outFaults.clear();
	for (ARBConfigFaultList::const_iterator iterFault = m_Config.GetFaults().begin();
		iterFault != m_Config.GetFaults().end();
		++iterFault)
	{
		const ARBConfigFault* pFault = (*iterFault);
		if (0 < pFault->GetName().length())
			outFaults.insert(pFault->GetName());
	}
	for (ARBDogList::const_iterator iterDog = m_Dogs.begin();
		iterDog != m_Dogs.end();
		++iterDog)
	{
		const ARBDog* pDog = (*iterDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			const ARBDogTrial* pTrial = (*iterTrial);
			for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
				iterRun != pTrial->GetRuns().end();
				++iterRun)
			{
				const ARBDogRun* pRun = (*iterRun);
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
