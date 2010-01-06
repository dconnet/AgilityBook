#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgReferenceRun class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include <set>
#include "ARBTypes.h"
class CAgilityBookDoc;


class CDlgReferenceRun : public wxDialog
{
public:
	CDlgReferenceRun(
			CAgilityBookDoc* pDoc,
			ARBDogRunPtr inRun,
			std::set<wxString> const& inHeights,
			std::set<wxString> const& inNames,
			std::set<wxString> const& inBreeds,
			ARBDogReferenceRunPtr ref,
			wxWindow* pParent = NULL);

private:
	CAgilityBookDoc* m_pDoc;
	ARBDogRunPtr m_Run;
	ARBDogReferenceRunPtr m_Ref;
	short m_Place;
	ARB_Q m_Q;
	double m_Time;
	wxStaticText* m_ctrlYPS;
	wxString m_Points;
	wxString m_Height;
	wxString m_Name;
	wxString m_Breed;
	wxString m_Notes;

	DECLARE_EVENT_TABLE()
	void OnKillfocusRefRunTime(wxFocusEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
