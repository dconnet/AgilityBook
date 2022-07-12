#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgSelectDog class
 * @author David Connet
 *
 * Revision History
 * 2021-03-09 Only show living dogs,
 * 2009-02-10 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARBTypes2.h"
#include <vector>


namespace dconSoft
{
class CAgilityBookDoc;


class CDlgSelectDog : public wxDialog
{
public:
	CDlgSelectDog(
		CAgilityBookDoc* pDoc,
		std::vector<ARB::ARBDogPtr>& outDogs,
		wxWindow* pParent = nullptr,
		bool onlyLiving = true);

	int ShowModal() override;

private:
	CAgilityBookDoc* m_pDoc;
	std::vector<ARB::ARBDogPtr>& m_outDogs;
	std::vector<ARB::ARBDogPtr> m_Dogs;
	wxCheckListBox* m_checkList;

	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
