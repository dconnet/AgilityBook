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
#include <vector>
class CAgilityBookDoc;
class CTreeCtrl;
class wxTreeEvent;


class CDlgReorder : public wxDialog
{
public:
	CDlgReorder(
			CAgilityBookDoc* pDoc,
			ARBDogList* dogs,
			wxWindow* pParent = NULL);
	CDlgReorder(
			CAgilityBookDoc* pDoc,
			ARBDogTrialPtr trial,
			ARBDogRunPtr run,
			wxWindow* pParent = NULL);

private:
	CAgilityBookDoc* m_pDoc;
	ARBDogList* m_Dogs;
	ARBDogTrialPtr m_Trial;
	ARBDogRunPtr m_Run;
	wxListBox* m_ctrlList;
	CTreeCtrl* m_ctrlTree;
	wxButton* m_ctrlUp;
	wxButton* m_ctrlDown;

protected:
	void Init(wxWindow* pParent);
	void UpdateControls();

	DECLARE_EVENT_TABLE()
	void OnListSelected(wxCommandEvent& evt);
	void OnTreeSelected(wxTreeEvent& evt);
	void OnMoveUp(wxCommandEvent& evt);
	void OnMoveDown(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
