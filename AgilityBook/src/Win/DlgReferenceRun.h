#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgReferenceRun class
 * @author David Connet
 *
 * Revision History
 * @li 2012-05-22 DRC Change KillFocus handler to text change handler.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARB_Q.h"
#include "ARB/ARBTypes.h"
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
			wxWindow* pParent = NULL);

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
