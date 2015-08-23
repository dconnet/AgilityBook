#pragma once

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
 * Usage: 
 * Users of this can extend by associating a callback class.
 *
 * Revision History
 * 2015-08-22 Expose LOAD_BITMAP_PNG for use in OnCreateBitmap.
 * 2013-10-13 Changed ImageManager to an art provider.
 * 2012-12-29 Created.
 */

#include "ImageManager.h"

// Helper functions called from IImageManagerCallback interfaces

namespace ImageHelper
{
	extern wxIcon CreateIconFromBitmap(const wxBitmap& bitmap);

	extern bool DoCreateBitmap(
			const wxArtID& id,
			const wxArtClient& client,
			const wxSize& size,
			wxBitmap& outBmp);

	extern bool DoCreateIconBundle(
			const wxArtID& id,
			const wxArtClient& client,
			wxIconBundle& outIcon);

#if defined(__WINDOWS__)
	extern void LoadLocalBitmap(
			wchar_t const* const pImageName,
			wxBitmap& outBmp);
#endif
};


#define ImageMgrAppBundle			wxART_MAKE_ART_ID(ImageMgrAppBundle)
#define ImageMgrApp					wxART_MAKE_ART_ID(ImageMgrApp)
#define ImageMgrApp48				wxART_MAKE_ART_ID(ImageMgrApp48)
#define ImageMgrApp256				wxART_MAKE_ART_ID(ImageMgrApp256)

#define ImageMgrBlank				wxART_MAKE_ART_ID(ImageMgrBlank)
#define ImageMgrChecked				wxART_MAKE_ART_ID(ImageMgrChecked)
#define ImageMgrUnChecked			wxART_MAKE_ART_ID(ImageMgrUnChecked)
#define ImageMgrCheck				wxART_MAKE_ART_ID(ImageMgrCheck)
#define ImageMgrQuestion			wxART_MAKE_ART_ID(ImageMgrQuestion)

#define ImageMgrHeaderDown			wxART_MAKE_ART_ID(ImageMgrHeaderDown)
#define ImageMgrHeaderUp			wxART_MAKE_ART_ID(ImageMgrHeaderUp)

#define ImageMgrRuns 				wxART_MAKE_ART_ID(ImageMgrRuns)
#define ImageMgrPoints				wxART_MAKE_ART_ID(ImageMgrPoints)
#define ImageMgrCalendar			wxART_MAKE_ART_ID(ImageMgrCalendar)
#define ImageMgrTraining			wxART_MAKE_ART_ID(ImageMgrTraining)
#define ImageMgrDog					wxART_MAKE_ART_ID(ImageMgrDog)
#define ImageMgrTrial				wxART_MAKE_ART_ID(ImageMgrTrial)
#define ImageMgrVenueAAC			wxART_MAKE_ART_ID(ImageMgrVenueAAC)
#define ImageMgrVenueAKC			wxART_MAKE_ART_ID(ImageMgrVenueAKC)
#define ImageMgrVenueASCA			wxART_MAKE_ART_ID(ImageMgrVenueASCA)
#define ImageMgrVenueCKC			wxART_MAKE_ART_ID(ImageMgrVenueCKC)
#define ImageMgrVenueCPE			wxART_MAKE_ART_ID(ImageMgrVenueCPE)
#define ImageMgrVenueDOCNA			wxART_MAKE_ART_ID(ImageMgrVenueDOCNA)
#define ImageMgrVenueFCI			wxART_MAKE_ART_ID(ImageMgrVenueFCI)
#define ImageMgrVenueNADAC			wxART_MAKE_ART_ID(ImageMgrVenueNADAC)
#define ImageMgrVenueSCC			wxART_MAKE_ART_ID(ImageMgrVenueSCC)
#define ImageMgrVenueSweepstakes	wxART_MAKE_ART_ID(ImageMgrVenueSweepstakes)
#define ImageMgrVenueTDAA			wxART_MAKE_ART_ID(ImageMgrVenueTDAA)
#define ImageMgrVenueUKC			wxART_MAKE_ART_ID(ImageMgrVenueUKC)
#define ImageMgrVenueUSDAA			wxART_MAKE_ART_ID(ImageMgrVenueUSDAA)
#define ImageMgrCRCD				wxART_MAKE_ART_ID(ImageMgrCRCD)
#define ImageMgrAccomConfirm		wxART_MAKE_ART_ID(ImageMgrAccomConfirm)
#define ImageMgrAccomNone			wxART_MAKE_ART_ID(ImageMgrAccomNone)
#define ImageMgrAccomTodo			wxART_MAKE_ART_ID(ImageMgrAccomTodo)
#define ImageMgrCalEntered			wxART_MAKE_ART_ID(ImageMgrCalEntered)
#define ImageMgrCalEnteredTentative	wxART_MAKE_ART_ID(ImageMgrCalEnteredTentative)
#define ImageMgrCalPending			wxART_MAKE_ART_ID(ImageMgrCalPending)
#define ImageMgrCalPendingTentative	wxART_MAKE_ART_ID(ImageMgrCalPendingTentative)
#define ImageMgrCalPlanTentative	wxART_MAKE_ART_ID(ImageMgrCalPlanTentative)

// Hidden: !
// Titled: ribbon
// Have: check
#define ImageMgrTitle					wxART_MAKE_ART_ID(ImageMgrTitle)
#define ImageMgrTitleHidden				wxART_MAKE_ART_ID(ImageMgrTitleHidden)
#define ImageMgrTitleTitledHidden		wxART_MAKE_ART_ID(ImageMgrTitleTitledHidden)
#define ImageMgrTitleTitledHiddenHave	wxART_MAKE_ART_ID(ImageMgrTitleTitledHiddenHave)
#define ImageMgrTitleTitled				wxART_MAKE_ART_ID(ImageMgrTitleTitled)
#define ImageMgrTitleTitledHave			wxART_MAKE_ART_ID(ImageMgrTitleTitledHave)

// Added: +
// Note: Yellow-sticky
#define ImageMgrNoteButton			wxART_MAKE_ART_ID(ImageMgrNoteButton)
#define ImageMgrInfoNote			wxART_MAKE_ART_ID(ImageMgrInfoNote)
#define ImageMgrInfoNoteAdded		wxART_MAKE_ART_ID(ImageMgrInfoNoteAdded)
#define ImageMgrInfoNoteNoteAdded	wxART_MAKE_ART_ID(ImageMgrInfoNoteNoteAdded)

#define ImageMgrAbout				wxART_MAKE_ART_ID(ImageMgrAbout)
#define ImageMgrCopy				wxART_MAKE_ART_ID(ImageMgrCopy)
#define ImageMgrCut					wxART_MAKE_ART_ID(ImageMgrCut)
#define ImageMgrNew					wxART_MAKE_ART_ID(ImageMgrNew)
#define ImageMgrOpen				wxART_MAKE_ART_ID(ImageMgrOpen)
#define ImageMgrPaste				wxART_MAKE_ART_ID(ImageMgrPaste)
#define ImageMgrPreview				wxART_MAKE_ART_ID(ImageMgrPreview)
#define ImageMgrPrint				wxART_MAKE_ART_ID(ImageMgrPrint)
#define ImageMgrSave				wxART_MAKE_ART_ID(ImageMgrSave)


#if defined(__WINDOWS__)
#define LOAD_BITMAP_PNG(name, outBmp)	ImageHelper::LoadLocalBitmap(L#name, outBmp)
#define LOAD_BUNDLE_PNG(name, outIcon) \
	{ \
		wxBitmap bmp; \
		ImageHelper::LoadLocalBitmap(L#name, bmp); \
		outIcon.AddIcon(ImageHelper::CreateIconFromBitmap(bmp)); \
	}

#else
#if !defined(__WXOSX__)
// OSX auto-loads @2 images.
#pragma PRAGMA_FIXME(This is not likely to load the correct image);
#endif
#define LOAD_BITMAP_PNG(name, outBmp)	outBmp = wxBITMAP_PNG(name)
#define LOAD_BUNDLE_PNG(name, outIcon) \
	{ \
		wxBitmap bmp = wxBITMAP_PNG(name); \
		outIcon.AddIcon(ImageHelper::CreateIconFromBitmap(bmp)); \
	}
#endif
