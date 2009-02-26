#pragma once

/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgTitle class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARBDate.h"
#include "ARBTypes.h"
class CRichEditCtrl2;
class CVenueComboBox;
class wxDateEvent;
class wxDatePickerCtrl;


class CDlgTitle : public wxDialog
{
public:
	CDlgTitle(
			ARBConfig const& config,
			ARBDogTitleList& titles,
			ARBDogTitlePtr pTitle,
			wxWindow* pParent = NULL);

private:
	ARBConfigTitlePtr GetTitleData(int index) const;
	ARBDate GetDate();
	void FillTitles(bool bIniting = false);
	void FillTitleInfo();

	ARBDogTitleList& m_Titles;
	ARBDogTitlePtr m_pTitle;
	wxDatePickerCtrl* m_ctrlDate;
	wxCheckBox* m_ctrlReceived;
	CVenueComboBox* m_ctrlVenues;
	wxComboBox* m_ctrlTitles;
	CRichEditCtrl2* m_ctrlDesc;
	wxString m_Venue;
	bool m_bEarned;
	bool m_bHidden;
	bool m_bReceived;

	DECLARE_EVENT_TABLE()
	void OnTitleDateChanged(wxDateEvent& evt);
	void OnClickedEarned(wxCommandEvent& evt);
	void OnSelchangeVenues(wxCommandEvent& evt);
	void OnSelchangeTitles(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
