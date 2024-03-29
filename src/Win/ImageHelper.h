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

#include "LibARBWin/ImageHelperBase.h"


namespace dconSoft
{


#define ImageMgrRuns                wxART_MAKE_ART_ID(ImageMgrRuns)
#define ImageMgrPoints              wxART_MAKE_ART_ID(ImageMgrPoints)
#define ImageMgrCalendar            wxART_MAKE_ART_ID(ImageMgrCalendar)
#define ImageMgrTraining            wxART_MAKE_ART_ID(ImageMgrTraining)
#define ImageMgrDog                 wxART_MAKE_ART_ID(ImageMgrDog)
#define ImageMgrTrial               wxART_MAKE_ART_ID(ImageMgrTrial)
#define ImageMgrVenueAAC            wxART_MAKE_ART_ID(ImageMgrVenueAAC)
#define ImageMgrVenueAKC            wxART_MAKE_ART_ID(ImageMgrVenueAKC)
#define ImageMgrVenueASCA           wxART_MAKE_ART_ID(ImageMgrVenueASCA)
#define ImageMgrVenueBHA            wxART_MAKE_ART_ID(ImageMgrVenueBHA)
#define ImageMgrVenueCKC            wxART_MAKE_ART_ID(ImageMgrVenueCKC)
#define ImageMgrVenueCKCSC          wxART_MAKE_ART_ID(ImageMgrVenueCKCSC)
#define ImageMgrVenueCPE            wxART_MAKE_ART_ID(ImageMgrVenueCPE)
#define ImageMgrVenueDOCNA          wxART_MAKE_ART_ID(ImageMgrVenueDOCNA)
#define ImageMgrVenueFCI            wxART_MAKE_ART_ID(ImageMgrVenueFCI)
#define ImageMgrVenueNADAC          wxART_MAKE_ART_ID(ImageMgrVenueNADAC)
#define ImageMgrVenueSCC            wxART_MAKE_ART_ID(ImageMgrVenueSCC)
#define ImageMgrVenueSweepstakes    wxART_MAKE_ART_ID(ImageMgrVenueSweepstakes)
#define ImageMgrVenueTDAA           wxART_MAKE_ART_ID(ImageMgrVenueTDAA)
#define ImageMgrVenueUKC            wxART_MAKE_ART_ID(ImageMgrVenueUKC)
#define ImageMgrVenueUKI            wxART_MAKE_ART_ID(ImageMgrVenueUKI)
#define ImageMgrVenueUSDAA          wxART_MAKE_ART_ID(ImageMgrVenueUSDAA)
#define ImageMgrVenueVALOR          wxART_MAKE_ART_ID(ImageMgrVenueVALOR)
#define ImageMgrCRCD                wxART_MAKE_ART_ID(ImageMgrCRCD)
#define ImageMgrAccomConfirm        wxART_MAKE_ART_ID(ImageMgrAccomConfirm)
#define ImageMgrAccomNone           wxART_MAKE_ART_ID(ImageMgrAccomNone)
#define ImageMgrAccomTodo           wxART_MAKE_ART_ID(ImageMgrAccomTodo)
#define ImageMgrCalEntered          wxART_MAKE_ART_ID(ImageMgrCalEntered)
#define ImageMgrCalEnteredTentative wxART_MAKE_ART_ID(ImageMgrCalEnteredTentative)
#define ImageMgrCalPending          wxART_MAKE_ART_ID(ImageMgrCalPending)
#define ImageMgrCalPendingTentative wxART_MAKE_ART_ID(ImageMgrCalPendingTentative)
#define ImageMgrCalPlanTentative    wxART_MAKE_ART_ID(ImageMgrCalPlanTentative)

// Hidden: !
// Titled: ribbon
// Have: check
#define ImageMgrTitle                 wxART_MAKE_ART_ID(ImageMgrTitle)
#define ImageMgrTitleHidden           wxART_MAKE_ART_ID(ImageMgrTitleHidden)
#define ImageMgrTitleTitledHidden     wxART_MAKE_ART_ID(ImageMgrTitleTitledHidden)
#define ImageMgrTitleTitledHiddenHave wxART_MAKE_ART_ID(ImageMgrTitleTitledHiddenHave)
#define ImageMgrTitleTitled           wxART_MAKE_ART_ID(ImageMgrTitleTitled)
#define ImageMgrTitleTitledHave       wxART_MAKE_ART_ID(ImageMgrTitleTitledHave)

// Added: +
// Note: Yellow-sticky
#define ImageMgrNoteButton        wxART_MAKE_ART_ID(ImageMgrNoteButton)
#define ImageMgrInfoNote          wxART_MAKE_ART_ID(ImageMgrInfoNote)
#define ImageMgrInfoNoteAdded     wxART_MAKE_ART_ID(ImageMgrInfoNoteAdded)
#define ImageMgrInfoNoteNoteAdded wxART_MAKE_ART_ID(ImageMgrInfoNoteNoteAdded)

#define ImageMgrAbout    wxART_MAKE_ART_ID(ImageMgrAbout)
#define ImageMgrCopy     wxART_MAKE_ART_ID(ImageMgrCopy)
#define ImageMgrCut      wxART_MAKE_ART_ID(ImageMgrCut)
#define ImageMgrDelete   wxART_MAKE_ART_ID(ImageMgrDelete)
#define ImageMgrFind     wxART_MAKE_ART_ID(ImageMgrFind)
#define ImageMgrNew      wxART_MAKE_ART_ID(ImageMgrNew)
#define ImageMgrOpen     wxART_MAKE_ART_ID(ImageMgrOpen)
#define ImageMgrPaste    wxART_MAKE_ART_ID(ImageMgrPaste)
#define ImageMgrPreview  wxART_MAKE_ART_ID(ImageMgrPreview)
#define ImageMgrPrint    wxART_MAKE_ART_ID(ImageMgrPrint)
#define ImageMgrSave     wxART_MAKE_ART_ID(ImageMgrSave)
#define ImageMgrSettings wxART_MAKE_ART_ID(ImageMgrSettings)

} // namespace dconSoft
