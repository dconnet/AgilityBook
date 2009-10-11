#pragma once

/*
 * Copyright (c) 2004-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgConfigTitle class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-01-05 DRC Created.
 */

#include "ARBTypes.h"
class CTextCtrl;
class wxDatePickerCtrl;


class CDlgConfigTitle : public wxDialog
{
public:
	CDlgConfigTitle(
			ARBConfigTitlePtr inTitle,
			wxWindow* pParent = NULL);

private:
	ARBConfigTitlePtr m_Title;
	wxString m_Name;
	bool m_Prefix;
	wxString m_LongName;
	wxString m_Desc;
	bool m_AllowMany;
	short m_Multiple;
	bool m_DateFrom;
	bool m_DateTo;
	CTextCtrl* m_ctrlMultiple;
	wxDatePickerCtrl* m_ctrlDateFrom;
	wxDatePickerCtrl* m_ctrlDateTo;
	wxComboBox* m_ctrlStyle;

protected:
	void UpdateButtons();

	DECLARE_EVENT_TABLE()
	void OnAllowMultiple(wxCommandEvent& evt);
	void OnCheck(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
