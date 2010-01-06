#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgReorder class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARBBase.h"
#include <vector>


class CDlgReorder : public wxDialog
{
public:
	CDlgReorder(
			std::vector<ARBBasePtr>& items,
			wxWindow* pParent = NULL);

private:
	std::vector<ARBBasePtr>& m_Items;
	wxListBox* m_ctrlList;
	wxButton* m_ctrlUp;
	wxButton* m_ctrlDown;

protected:
	void LoadData();
	void UpdateControls();

	void OnItemSelected(wxCommandEvent& evt);
	void OnMoveUp(wxCommandEvent& evt);
	void OnMoveDown(wxCommandEvent& evt);
};
