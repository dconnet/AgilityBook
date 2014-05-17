#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgReferenceRun class
 * @author David Connet
 *
 * Revision History
 * 2012-05-22 Change KillFocus handler to text change handler.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARB_Q.h"
#include "ARB/ARBTypes2.h"
#include <set>
class CAgilityBookDoc;
class CTextCtrl;


class CDlgReferenceRun : public wxDialog
{
public:
	CDlgReferenceRun(
			CAgilityBookDoc* pDoc,
			ARBDogRunPtr inRun,
			std::set<std::wstring> const& inHeights,
			std::set<std::wstring> const& inNames,
			std::set<std::wstring> const& inBreeds,
			ARBDogReferenceRunPtr ref,
			wxWindow* pParent = nullptr);

private:
	CAgilityBookDoc* m_pDoc;
	ARBDogRunPtr m_Run;
	ARBDogReferenceRunPtr m_Ref;
	short m_Place;
	ARB_Q m_Q;
	double m_Time;
	CTextCtrl* m_ctrlTime;
	wxStaticText* m_ctrlYPS;
	wxString m_Points;
	wxString m_Height;
	wxString m_Name;
	wxString m_Breed;
	wxString m_Notes;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnEnChangeRefRunTime(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
