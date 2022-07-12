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

#include "ARB/ARBTypes2.h"
class wxCheckBox;


namespace dconSoft
{
class CAgilityBookDoc;
class CMetaDataDisplay;

class CDlgRunPanelCRCD : public CDlgRunPanelBase
{
public:
	CDlgRunPanelCRCD(
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
	void UpdateCRCDButtons();

	CMetaDataDisplay* m_CRCDDisplay;
	wxButton* m_ctrlCourse;
	wxButton* m_ctrlCRCDView;
	wxButton* m_ctrlCRCDCopy;
	wxCheckBox* m_ctrlIncImage;

	void OnCRCDEdit(wxCommandEvent& evt);
	void OnCRCDView(wxCommandEvent& evt);
	void OnCRCDCopy(wxCommandEvent& evt);
	void OnCRCDImage(wxCommandEvent& evt);
};

} // namespace dconSoft
