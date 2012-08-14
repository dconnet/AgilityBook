#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgSelectDog class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARBTypes.h"
#include <vector>
class CAgilityBookDoc;

class CDlgSelectDog : public wxDialog
{
public:
	CDlgSelectDog(
			CAgilityBookDoc* pDoc,
			std::vector<ARBDogPtr>& outDogs,
			wxWindow* pParent = NULL);

	int ShowModal();

private:
	CAgilityBookDoc* m_pDoc;
	std::vector<ARBDogPtr>& m_outDogs;
	std::vector<ARBDogPtr> m_Dogs;
	wxCheckListBox* m_checkList;

	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};
