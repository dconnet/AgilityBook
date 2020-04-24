#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Printing
 * @author David Connet
 *
 * Revision History
 * 2009-04-19 Moved from Globals.h
 */

#include "ARBCommon/ARBTypes.h"
#include "PointsData.h"
#include <wx/html/htmprint.h>
#include <wx/print.h>
#include <vector>


class CPrintPreview : public wxPrintPreview
{
public:
	CPrintPreview(wxPrintout* printout, wxPrintout* printoutForPrinting, wxPrintDialogData* data = nullptr);
	// Override so we can close the frame after printing. That's how MFC did
	// it (note: MFC closed as soon as you clicked print - we wait for success
	// so we're not fully implmementing the old mfc behavior)
	bool Print(bool interactive) override;
};


class CHtmlEasyPrinting : public wxHtmlEasyPrinting
{
public:
	CHtmlEasyPrinting(wxWindow* parent);
	bool DoPreview(wxHtmlPrintout* printout1, wxHtmlPrintout* printout2) override;
	bool DoPrint(wxHtmlPrintout* printout) override;
};


/**
 * Print blank pages to enter runs on.
 * @param inConfig Configuration
 * @param inRuns Runs to print, if empty, print blank pages.
 * @return Printed?
 * @note In Print.cpp
 */
extern bool PrintRuns(ARBConfig const* inConfig, std::vector<RunInfo> const& inRuns);
