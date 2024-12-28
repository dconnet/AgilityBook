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
 */

#include "DlgRunPanelBase.h"


namespace dconSoft
{
class CAgilityBookDoc;


class CDlgRunPanelComments : public CDlgRunPanelBase
{
public:
	CDlgRunPanelComments(
		CDlgRun* pDlg,
		CAgilityBookDoc* pDoc,
		ARB::ARBDogPtr const& inDog,
		ARB::ARBDogTrialPtr const& inTrial,
		ARB::ARBDogRunPtr const& inRun,
		wxWindow* parent);

	wxWindow* GetInitialControl() override;
	void OnActivation() override
	{
	}
	bool Validate() override;
	bool Save() override;

private:
	void SetFaultsText();
	void GetAllCallNames(std::set<wxString>& outNames);

	wxString m_Comments;
	wxListBox* m_ctrlFaultsList;

	void OnCommentsFaults(wxCommandEvent& evt);
};

} // namespace dconSoft
