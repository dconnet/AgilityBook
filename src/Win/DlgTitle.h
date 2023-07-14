#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgTitle class
 * @author David Connet
 *
 * Revision History
 * 2011-08-17 Add support to get newly created title.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include "LibARBWin/Logger.h"
class wxDateEvent;
class wxDatePickerCtrl;


namespace dconSoft
{
namespace ARBWin
{
class CRichEditCtrl2;
} // namespace ARBWin
class CVenueComboBox;


class CDlgTitle : public wxDialog
{
public:
	CDlgTitle(
		ARB::ARBConfig const& config,
		ARB::ARBDogTitleList& titles,
		ARB::ARBDogTitlePtr const& inTitle,
		wxWindow* pParent = nullptr);

	// Returns existing title or the newly created one.
	ARB::ARBDogTitlePtr GetNewTitle() const
	{
		return m_newTitle;
	}

private:
	ARB::ARBConfigTitlePtr GetTitleData(int index) const;
	ARBCommon::ARBDate GetDate();
	void FillTitles(bool bIniting = false);
	void FillTitleInfo();
	short GetInstance(ARB::ARBConfigTitlePtr const& inTitle, std::vector<short>* outMissing = nullptr) const;

	ARB::ARBDogTitleList& m_Titles;
	ARB::ARBDogTitlePtr m_pTitle;
	ARB::ARBDogTitlePtr m_newTitle;
	wxDatePickerCtrl* m_ctrlDate;
	wxCheckBox* m_ctrlReceived;
	CVenueComboBox* m_ctrlVenues;
	wxCheckBox* m_ctrlHide;
	wxComboBox* m_ctrlTitles;
	ARBWin::CRichEditCtrl2* m_ctrlDesc;
	wxString m_Venue;
	bool m_bEarned;
	bool m_bHidden;
	bool m_bReceived;
	ARBWin::CStackLogger m_trace;

	void OnTitleDateChanged(wxDateEvent& evt);
	void OnClickedEarned(wxCommandEvent& evt);
	void OnSelchangeVenues(wxCommandEvent& evt);
	void OnSelchangeTitles(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
