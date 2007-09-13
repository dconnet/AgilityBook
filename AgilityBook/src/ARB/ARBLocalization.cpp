/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
 * @brief Localization
 * @author David Connet
 *
 * If ARB is ported to a different platform, this class will need to be
 * changed. It makes use of CStrings and resources.
 *
 * Revision History
 * @li 2007-09-12 DRC Moved hardcoded strings out of ARBAgilityRecordBook.h
 */

#include "StdAfx.h"
#include "ARBLocalization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

tstring ErrorInvalidDocStructure(TCHAR const* const inMsg)
{
	otstringstream str;
	str << INVALID_DOC_STRUCTURE;
	if (inMsg)
		str << ": " << inMsg << std::endl;
	return str.str();
}


tstring ErrorMissingAttribute(
		TCHAR const* const inElement,
		TCHAR const* const inAttrib,
		TCHAR const* const inMsg)
{
	otstringstream str;
	str << INVALID_FILE_FORMAT
		<< inElement
		<< INVALID_FILE_MISSING_ATTRIB
		<< inAttrib
		<< "'.";
	if (inMsg)
		str << " " << inMsg << std::endl;
	return str.str();
}


tstring ErrorInvalidAttributeValue(
		TCHAR const* const inElement,
		TCHAR const* const inAttrib,
		TCHAR const* const inMsg)
{
	otstringstream str;
	str << INVALID_FILE_FORMAT
		<< inElement
		<< INVALID_FILE_BAD_ATTRIB
		<< inAttrib
		<< "'.";
	if (inMsg)
		str << " " << inMsg << std::endl;
	return str.str();
}

/////////////////////////////////////////////////////////////////////////////

tstring UPDATE_FORMAT_FAULTS(int nNew, int nSkipped)
{
	otstringstream buffer;
	buffer << _T("Faults: ") << nNew << _T(" added, ") << nSkipped << _T(" identical");
	return buffer.str();
}


tstring UPDATE_FORMAT_OTHERPTS(int nNew, int nUpdated, int nSkipped)
{
	otstringstream buffer;
	buffer << _T("Other Points: ") << nNew << _T(" added, ") << nUpdated << _T(" updated, ") << nSkipped << _T(" identical");
	return buffer.str();
}


tstring UPDATE_FORMAT_VENUES(int nNew, int nUpdated, int nSkipped)
{
	otstringstream buffer;
	buffer << _T("Venues: ") << nNew << _T(" added, ") << nUpdated << _T(" updated, ") << nSkipped << _T(" identical");
	return buffer.str();
}


tstring UPDATE_FORMAT_DIVISIONS(int nAdded, int nChanged, int nSkipped)
{
	otstringstream buffer;
	buffer << _T("Divisions: ") << nAdded << _T(" added, ") << nChanged << _T(" updated, ") << nSkipped << _T(" identical");
	return buffer.str();
}


tstring UPDATE_FORMAT_EVENTS(int nAdded, int nChanged, int nSkipped)
{
	otstringstream buffer;
	buffer << _T("Events: ") << nAdded << _T(" added, ") << nChanged << _T(" updated, ") << nSkipped << _T(" identical");
	return buffer.str();
}


tstring UPDATE_FORMAT_MULTIQS(int nAdded, int nDeleted, int nSkipped)
{
	otstringstream buffer;
	buffer << _T("MultiQs: ") << nAdded << _T(" added, ") << nDeleted << _T(" deleted, ") << nSkipped << _T(" identical");
	return buffer.str();
}


tstring UPDATE_FORMAT_LEVELS(int nAdded, int nChanged, int nSkipped)
{
	otstringstream buffer;
	buffer << _T("Levels: ") << nAdded << _T(" added, ") << nChanged << _T(" updated, ") << nSkipped << _T(" identical");
	return buffer.str();
}


tstring UPDATE_FORMAT_TITLES(int nAdded, int nChanged, int nSkipped)
{
	otstringstream buffer;
	buffer << _T("Titles: ") << nAdded << _T(" added, ") << nChanged << _T(" updated, ") << nSkipped << _T(" identical");
	return buffer.str();
}


tstring UPDATE_FORMAT_SUBLEVELS(int nAdded)
{
	otstringstream buffer;
	buffer << nAdded << _T(" new sub-levels");
	return buffer.str();
}


tstring UPDATE_FORMAT_RULES(int nAdded, int nDeleted, int nChanged, int nSkipped)
{
	otstringstream buffer;
	buffer << _T(" rules: ") << nAdded << _T(" added, ") << nDeleted << _T(" deleted, ") << nChanged << _T(" updated, ") << nSkipped << _T(" identical");
	return buffer.str();
}


tstring TITLE_POINTS_NAME_FORMAT(double points, double faults)
{
	otstringstream buffer;
	buffer << points << _T(" points with ") << faults << _T(" faults");
	return buffer.str();
}


tstring LIFETIME_POINTS_NAME_FORMAT(double points, double faults)
{
	otstringstream buffer;
	buffer << points << _T(" lifetime points with ") << faults << _T(" faults");
	return buffer.str();
}


tstring PLACEMENT_POINTS_NAME_FORMAT(double points, short place)
{
	otstringstream buffer;
	buffer << points << _T(" points with place of ") << place;
	return buffer.str();
}


tstring WARN_DELETED_RUNS(int nRuns, tstring const& inRunsMsg)
{
	otstringstream buffer;
	buffer << _T("WARNING: ") << nRuns << _T(" run(s) deleted due to configuration changes.\n") << inRunsMsg;
	return buffer.str();
}


tstring UPDATE_TABLE_RUNS(int nRuns)
{
	otstringstream buffer;
	buffer << _T("Table setting updated in ") << nRuns << _T(" runs.");
	return buffer.str();
}
