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
 * @brief interface of the CDlgReferenceRun class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include <set>
#include "ARBTypes.h"
class CAgilityBookDoc;


class CDlgReferenceRun : public wxDialog
{
public:
	CDlgReferenceRun(
			CAgilityBookDoc* pDoc,
			ARBDogRunPtr inRun,
			std::set<wxString> const& inHeights,
			std::set<wxString> const& inNames,
			std::set<wxString> const& inBreeds,
			ARBDogReferenceRunPtr ref,
			wxWindow* pParent = NULL);

private:
	CAgilityBookDoc* m_pDoc;
	ARBDogRunPtr m_Run;
	ARBDogReferenceRunPtr m_Ref;
	short m_Place;
	ARB_Q m_Q;
	double m_Time;
	wxStaticText* m_ctrlYPS;
	wxString m_Points;
	wxString m_Height;
	wxString m_Name;
	wxString m_Breed;
	wxString m_Notes;

	DECLARE_EVENT_TABLE()
	void OnKillfocusRefRunTime(wxFocusEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
