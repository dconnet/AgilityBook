#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Base class for dog panels
 * @author David Connet
 */

#include "ARB/ARBTypes2.h"
#include "LibARBWin/ReportListHeader.h"
#include <wx/panel.h>
class CAgilityBookDoc;
class CReportListCtrl;


class CDlgDogPanelBase : public wxPanel
{
public:
	CDlgDogPanelBase(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* parent);

	virtual wxWindow* GetInitialControl() = 0;
	virtual bool Save(unsigned int& hint) = 0;

protected:
	CAgilityBookDoc* m_pDoc;
	ARBDogPtr m_pDog;
};


class CDlgDogPanelReportBase : public CDlgDogPanelBase
{
public:
	CDlgDogPanelReportBase(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* parent);
	wxWindow* GetInitialControl() override;
	bool Save(unsigned int& hint) override;

protected:
	void DeleteListItem(long index);

	virtual void DoCreate(bool isSingleSel, bool hasImageList);
	virtual void DoInitList() = 0;
	virtual void UpdateControls();

	virtual void DoNew() = 0;
	virtual void DoEdit() = 0;
	virtual void DoDelete() = 0;

	CReportListCtrl* m_ctrlList;
	CReportListHeader m_reportColumn;
	wxButton* m_btnNew;
	wxButton* m_btnEdit;
	wxButton* m_btnDelete;
	wxBoxSizer* m_sizerButtons;

	void OnItemActivated(wxListEvent& evt);
	void OnKeyDown(wxKeyEvent& evt);
	void OnNew(wxCommandEvent& evt);
	void OnEdit(wxCommandEvent& evt);
	void OnDelete(wxCommandEvent& evt);
};
