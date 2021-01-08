#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * 2021-01-07 Separated into multiple files.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-06-02 Moved ShellExecute code to AgilityBook.cpp, added icons.
 * 2004-03-30 Created
 */

#include "DlgRunPanelBase.h"

#include "ARB/ARBTypes2.h"
#include <wx/listctrl.h>
class CAgilityBookDoc;
class CListCtrl;

class CDlgRunPanelLinks : public CDlgRunPanelBase
{
	friend class CLinkDropTarget;

public:
	CDlgRunPanelLinks(
		CDlgRun* pDlg,
		CAgilityBookDoc* pDoc,
		ARBDogPtr const& inDog,
		ARBDogTrialPtr const& inTrial,
		ARBDogRunPtr const& inRun,
		wxWindow* parent);

	wxWindow* GetInitialControl() override;
	void OnActivation() override
	{
	}
	bool Validate() override;
	bool Save() override;

private:
	void UpdateLinksButtons();
	void ListLinkFiles(wchar_t const* pItem);
	int GetImageIndex(std::wstring const& inLink);
	void EditLink();

	CListCtrl* m_ctrlLinks;
	wxImageList m_ImageList;
	int m_imgEmpty;
	int m_imgOk;
	int m_imgMissing;
	wxButton* m_ctrlLinkEdit;
	wxButton* m_ctrlLinkDelete;
	wxButton* m_ctrlLinkOpen;

	void OnLinksItemSelected(wxListEvent& evt);
	void OnLinksItemActivated(wxListEvent& evt);
	void OnLinksKeyDown(wxKeyEvent& evt);
	void OnLinksNew(wxCommandEvent& evt);
	void OnLinksEdit(wxCommandEvent& evt);
	void OnLinksDelete(wxCommandEvent& evt);
	void OnLinksOpen(wxCommandEvent& evt);
};
