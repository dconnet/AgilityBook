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

#include "ARB/ARBTypes2.h"
#include "ARB/ARB_Q.h"
#include "LibARBWin/Logger.h"
#include <set>


namespace dconSoft
{
namespace ARBWin
{
class CTextCtrl;
} // namespace ARBWin
class CAgilityBookDoc;


class CDlgReferenceRun : public wxDialog
{
public:
	CDlgReferenceRun(
		CAgilityBookDoc* pDoc,
		ARB::ARBDogRunPtr const& inRun,
		std::set<wxString> const& inHeights,
		std::set<wxString> const& inNames,
		std::set<wxString> const& inBreeds,
		ARB::ARBDogReferenceRunPtr const& inRef,
		wxWindow* pParent = nullptr);

private:
	CAgilityBookDoc* m_pDoc;
	ARB::ARBDogRunPtr m_Run;
	ARB::ARBDogReferenceRunPtr m_Ref;
	short m_Place;
	ARB::ARB_Q m_Q;
	double m_Time;
	ARBWin::CTextCtrl* m_ctrlTime;
	wxStaticText* m_ctrlYPS;
	wxString m_Points;
	wxString m_Height;
	wxString m_Name;
	wxString m_Breed;
	wxString m_Notes;
	ARBWin::CStackLogger m_trace;

	void OnEnChangeRefRunTime(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
