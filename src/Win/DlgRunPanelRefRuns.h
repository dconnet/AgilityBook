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
 * 2015-11-01 Added YPS.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-07-10 Add button to add yourself to ref-runs.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2003-10-13 Make ref run dlg default to perfect score.
 */

#include "DlgRunPanelBase.h"

#include "ARB/ARBTypes2.h"
#include "LibARBWin/ReportListHeader.h"
#include <wx/listctrl.h>


namespace dconSoft
{
namespace ARBWin
{
class CReportListCtrl;
} // namespace ARBWin
class CAgilityBookDoc;

class CDlgDogRefRunData;
typedef std::shared_ptr<CDlgDogRefRunData> CDlgDogRefRunDataPtr;


class CDlgRunPanelRefRuns : public CDlgRunPanelBase
{
public:
	CDlgRunPanelRefRuns(
		CDlgRun* pDlg,
		CAgilityBookDoc* pDoc,
		ARB::ARBDogPtr const& inDog,
		ARB::ARBDogTrialPtr const& inTrial,
		ARB::ARBDogRunPtr const& inRun,
		wxWindow* parent);

	wxWindow* GetInitialControl() override;
	void OnActivation() override;
	bool Validate() override;
	bool Save() override;

private:
	CDlgDogRefRunDataPtr GetReferenceData(long index) const;
	bool IsRefRunMe();
	void CreateRefRunMe();
	void UpdateRefRunButtons();
	void ListRefRuns();
	void GetAllHeights(std::set<std::wstring>& outNames);
	void GetAllCallNames(std::set<std::wstring>& outNames);
	void GetAllBreeds(std::set<std::wstring>& outNames);
	void EditRefRun();

	ARB::ARBDogReferenceRunPtr m_pRefRunMe;

	ARBWin::CReportListCtrl* m_ctrlRefRuns;
	ARBWin::CReportListHeader m_reportColumn;
	wxButton* m_ctrlRefAddMe;
	wxButton* m_ctrlRefEdit;
	wxButton* m_ctrlRefDelete;

	void OnRefRunItemSelected(wxListEvent& evt);
	void OnRefRunItemActivated(wxListEvent& evt);
	void OnRefRunKeyDown(wxKeyEvent& evt);
	void OnRefRunNew(wxCommandEvent& evt);
	void OnRefRunAddMe(wxCommandEvent& evt);
	void OnRefRunEdit(wxCommandEvent& evt);
	void OnRefRunDelete(wxCommandEvent& evt);
};

} // namespace dconSoft
