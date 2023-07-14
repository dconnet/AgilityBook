#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgReorder class
 * @author David Connet
 *
 * Revision History
 * 2011-10-14 Changed how reorder dlg is used.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARBBase.h"
#include "LibARBWin/Logger.h"
#include <vector>
class wxTreeEvent;


namespace dconSoft
{
namespace ARBWin
{
class CTreeCtrl;
} // namespace ARBWin
class CAgilityBookDoc;


class CDlgReorder : public wxDialog
{
public:
	CDlgReorder(CAgilityBookDoc* pDoc, ARB::ARBDogList* dogs, wxWindow* pParent = nullptr);
	CDlgReorder(
		CAgilityBookDoc* pDoc,
		ARB::ARBDogTrialPtr const& inTrial,
		ARB::ARBDogRunPtr const& inRun,
		wxWindow* pParent = nullptr);

private:
	void InitDlg(wxWindow* pParent);
	void UpdateControls();

	CAgilityBookDoc* m_pDoc;
	ARB::ARBDogList* m_Dogs;
	ARB::ARBDogTrialPtr m_Trial;
	ARB::ARBDogRunPtr m_Run;
	wxListBox* m_ctrlList;
	ARBWin::CTreeCtrl* m_ctrlTree;
	wxButton* m_ctrlUp;
	wxButton* m_ctrlDown;
	ARBWin::CStackLogger m_trace;

	DECLARE_EVENT_TABLE()
	void OnListSelected(wxCommandEvent& evt);
	void OnTreeSelected(wxTreeEvent& evt);
	void OnMoveUp(wxCommandEvent& evt);
	void OnMoveDown(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
