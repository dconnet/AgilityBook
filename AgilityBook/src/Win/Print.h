#pragma once

/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
 * @brief Printing
 * @author David Connet
 *
 * Revision History
 * @li 2009-04-19 DRC Moved from Globals.h
 */

#include "ARBTypes.h"
#include "PointsData.h"
#include <vector>
#include <wx/html/htmprint.h>
#include <wx/print.h>


class CPrintPreview : public wxPrintPreview
{
public:
	CPrintPreview(
			wxPrintout* printout,
			wxPrintout* printoutForPrinting,
			wxPrintDialogData* data = NULL);
	// Override so we can close the frame after printing. That's how MFC did
	// it (note: MFC closed as soon as you clicked print - we wait for success
	// so we're not fully implmementing the old mfc behavior)
	bool Print(bool interactive);
};


class CHtmlEasyPrinting : public wxHtmlEasyPrinting
{
public:
	CHtmlEasyPrinting(wxWindow* parent);
	virtual bool DoPreview(
			wxHtmlPrintout* printout1,
			wxHtmlPrintout* printout2);
	virtual bool DoPrint(wxHtmlPrintout* printout);
};


/**
 * Print blank pages to enter runs on.
 * @param inConfig Configuration
 * @param inDog Dog's runs to print.
 * @param inRuns Runs to print, if empty, print blank pages.
 * @return Printed?
 * @note In Print.cpp
 */
extern bool PrintRuns(
		ARBConfig const* inConfig,
		ARBDogPtr inDog,
		std::vector<RunInfo> const& inRuns);
