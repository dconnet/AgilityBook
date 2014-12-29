/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Image Manager
 * @author David Connet
 *
 * Revision History
 * 2013-10-13 Changed ImageManager to an art provider.
 * 2012-12-29 Created (separated from IconList).
 */

#include "stdafx.h"
#include "ImageHelper.h"

#include "DPI.h"
#include <wx/artprov.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace ImageHelper
{

wxIcon CreateIconFromBitmap(const wxBitmap& bitmap)
{
	wxIcon icon;
	icon.CopyFromBitmap(bitmap);
	return icon;
}


#if defined(__WINDOWS__)
static void LoadLocalBitmap(wchar_t const* const pImageName, wxBitmap& outBmp)
{
	wxLogNull suppress;
	unsigned int scale = DPI::GetScale();
	if (scale > 100)
	{
		wxString s(pImageName);
		s += L"_2";
		outBmp = wxBitmap(s, wxBITMAP_TYPE_PNG_RESOURCE);
		if (outBmp.IsOk())
			scale /= 2;
		else
			outBmp = wxBitmap(pImageName, wxBITMAP_TYPE_PNG_RESOURCE);
	}
	else
		outBmp = wxBitmap(pImageName, wxBITMAP_TYPE_PNG_RESOURCE);

	if (100 != scale && outBmp.IsOk())
	{
		wxImage image = outBmp.ConvertToImage();
		image.Rescale(outBmp.GetWidth() * scale / 100, outBmp.GetHeight() * scale / 100);
		outBmp = wxBitmap(image);
	}
}
#define LOAD_BITMAP_PNG(name, outBmp)	LoadLocalBitmap(L#name, outBmp)

#else
#if !defined(__WXOSX__)
// OSX auto-loads @2 images.
#pragma PRAGMA_FIXME(This is not likely to load the correct image);
#endif
#define LOAD_BITMAP_PNG(name, outBmp)	outBmp = wxBITMAP_PNG(name)
#endif


bool DoCreateBitmap(
		const wxArtID& id,
		const wxArtClient& client,
		const wxSize& size,
		wxBitmap& outBmp)
{
#ifdef _DEBUG
	bool bCheckBmp = true;
#endif

	if (id == ImageMgrApp)
	{
		if (client == wxART_MESSAGE_BOX)
			LOAD_BITMAP_PNG(AgilityBook32, outBmp);
		else
			LOAD_BITMAP_PNG(AgilityBook16, outBmp);
	}
	else if (id == ImageMgrApp48)
		LOAD_BITMAP_PNG(AgilityBook48, outBmp);
	else if (id == ImageMgrApp256)
		LOAD_BITMAP_PNG(AgilityBook256, outBmp);

	else if (id == ImageMgrRuns)
	{
		if (client == wxART_TOOLBAR)
			LOAD_BITMAP_PNG(toolbarRun, outBmp);
		else
			LOAD_BITMAP_PNG(run, outBmp);
	}

	else if (id == ImageMgrPoints)
		LOAD_BITMAP_PNG(points, outBmp);

	else if (id == ImageMgrCalendar)
	{
		if (client == wxART_TOOLBAR)
			LOAD_BITMAP_PNG(toolbarCalendar, outBmp);
		else
			LOAD_BITMAP_PNG(calendar, outBmp);
	}

	else if (id == ImageMgrTraining)
	{
		if (client == wxART_TOOLBAR)
			LOAD_BITMAP_PNG(toolbarTraining, outBmp);
		else
			LOAD_BITMAP_PNG(training, outBmp);
	}

	else if (id == ImageMgrDog)
	{
		if (client == wxART_TOOLBAR)
			LOAD_BITMAP_PNG(toolbarDog, outBmp);
		else
			LOAD_BITMAP_PNG(dog, outBmp);
	}

	else if (id == ImageMgrTrial)
	{
		if (client == wxART_TOOLBAR)
			LOAD_BITMAP_PNG(toolbarTrial, outBmp);
		else
			LOAD_BITMAP_PNG(trial, outBmp);
	}

	else if (id == ImageMgrVenueAAC)
		LOAD_BITMAP_PNG(venue_aac, outBmp);
	else if (id == ImageMgrVenueAKC)
		LOAD_BITMAP_PNG(venue_akc, outBmp);
	else if (id == ImageMgrVenueASCA)
		LOAD_BITMAP_PNG(venue_asca, outBmp);
	else if (id == ImageMgrVenueCKC)
		LOAD_BITMAP_PNG(venue_ckc, outBmp);
	else if (id == ImageMgrVenueCPE)
		LOAD_BITMAP_PNG(venue_cpe, outBmp);
	else if (id == ImageMgrVenueDOCNA)
		LOAD_BITMAP_PNG(venue_docna, outBmp);
	else if (id == ImageMgrVenueFCI)
		LOAD_BITMAP_PNG(venue_fci, outBmp);
	else if (id == ImageMgrVenueNADAC)
		LOAD_BITMAP_PNG(venue_nadac, outBmp);
	else if (id == ImageMgrVenueSCC)
		LOAD_BITMAP_PNG(venue_scc, outBmp);
	else if (id == ImageMgrVenueSweepstakes)
		LOAD_BITMAP_PNG(venue_sweep, outBmp);
	else if (id == ImageMgrVenueTDAA)
		LOAD_BITMAP_PNG(venue_tdaa, outBmp);
	else if (id == ImageMgrVenueUKC)
		LOAD_BITMAP_PNG(venue_ukc, outBmp);
	else if (id == ImageMgrVenueUSDAA)
		LOAD_BITMAP_PNG(venue_usdaa, outBmp);

	else if (id == ImageMgrCRCD)
		LOAD_BITMAP_PNG(crcd, outBmp);
	else if (id == ImageMgrBlank)
		LOAD_BITMAP_PNG(CalEmpty, outBmp);
	else if (id == ImageMgrChecked)
		LOAD_BITMAP_PNG(checked, outBmp);
	else if (id == ImageMgrUnChecked)
		LOAD_BITMAP_PNG(unchecked, outBmp);
	else if (id == ImageMgrAccomConfirm)
		LOAD_BITMAP_PNG(AccConfirm, outBmp);
	else if (id == ImageMgrAccomNone)
		LOAD_BITMAP_PNG(AccNone, outBmp);
	else if (id == ImageMgrAccomTodo)
		LOAD_BITMAP_PNG(AccTodo, outBmp);
	else if (id == ImageMgrCalEntered)
		LOAD_BITMAP_PNG(CalEntered, outBmp);
	else if (id == ImageMgrCalEnteredTentative)
		LOAD_BITMAP_PNG(CalEnteredTentative, outBmp);
	else if (id == ImageMgrCalPending)
		LOAD_BITMAP_PNG(CalPending, outBmp);
	else if (id == ImageMgrCalPendingTentative)
		LOAD_BITMAP_PNG(CalPendingTentative, outBmp);
	else if (id == ImageMgrCheck)
		LOAD_BITMAP_PNG(CalPlan, outBmp);
	else if (id == ImageMgrCalPlanTentative)
		LOAD_BITMAP_PNG(CalPlanTentative, outBmp);
	else if (id == ImageMgrQuestion)
		LOAD_BITMAP_PNG(CalTentative, outBmp);
	else if (id == ImageMgrHeaderDown)
		LOAD_BITMAP_PNG(HdrDown, outBmp);
	else if (id == ImageMgrHeaderUp)
		LOAD_BITMAP_PNG(HdrUp, outBmp);

	else if (id == ImageMgrTitle)
		LOAD_BITMAP_PNG(toolbarTitle, outBmp);
	else if (id == ImageMgrTitleHidden)
		LOAD_BITMAP_PNG(hidden, outBmp);
	else if (id == ImageMgrTitleTitledHidden)
		LOAD_BITMAP_PNG(title_hidden, outBmp);
	else if (id == ImageMgrTitleTitledHiddenHave)
		LOAD_BITMAP_PNG(title_hidden_have, outBmp);
	else if (id == ImageMgrTitleTitled)
		LOAD_BITMAP_PNG(title_visible, outBmp);
	else if (id == ImageMgrTitleTitledHave)
		LOAD_BITMAP_PNG(title_visible_have, outBmp);

	else if (id == ImageMgrNoteButton)
		LOAD_BITMAP_PNG(Note, outBmp);
	else if (id == ImageMgrInfoNote)
		LOAD_BITMAP_PNG(NoteNote, outBmp);
	else if (id == ImageMgrInfoNoteAdded)
		LOAD_BITMAP_PNG(NoteAdded, outBmp);
	else if (id == ImageMgrInfoNoteNoteAdded)
		LOAD_BITMAP_PNG(NoteNoteAdded, outBmp);
	
	else if (id == ImageMgrAbout)
		LOAD_BITMAP_PNG(toolbarAbout, outBmp);
	else if (id == ImageMgrCopy)
		LOAD_BITMAP_PNG(toolbarCopy, outBmp);
	else if (id == ImageMgrCut)
		LOAD_BITMAP_PNG(toolbarCut, outBmp);
	else if (id == ImageMgrNew)
		LOAD_BITMAP_PNG(toolbarNew, outBmp);
	else if (id == ImageMgrOpen)
		LOAD_BITMAP_PNG(toolbarOpen, outBmp);
	else if (id == ImageMgrPaste)
		LOAD_BITMAP_PNG(toolbarPaste, outBmp);
	else if (id == ImageMgrPreview)
		LOAD_BITMAP_PNG(toolbarPreview, outBmp);
	else if (id == ImageMgrPrint)
		LOAD_BITMAP_PNG(toolbarPrint, outBmp);
	else if (id == ImageMgrSave)
		LOAD_BITMAP_PNG(toolbarSave, outBmp);

	else
	{
#ifdef _DEBUG
		if (id == wxART_FILE_SAVE_AS
		|| id == wxART_INFORMATION
		|| id == wxART_WARNING
		|| id == wxART_ERROR)
			;
		else
			assert(0);
		bCheckBmp = false;
#endif
	}

#ifdef _DEBUG
	if (bCheckBmp)
	{
		assert(outBmp.IsOk());
	}
#endif
	return outBmp.IsOk();
}


bool DoCreateIconBundle(
		const wxArtID& id,
		const wxArtClient& client,
		wxIconBundle& outIcon)
{
	if (id == ImageMgrAppBundle)
	{
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp));
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp, wxART_MESSAGE_BOX));
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp48));
		outIcon.AddIcon(CImageManager::Get()->GetIcon(ImageMgrApp256));
		return true;
	}
	return false;
}

};
