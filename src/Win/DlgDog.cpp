/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * 2021-01-07 Separated panes from DlgDog.cpp
 * 2018-12-16 Convert to fmt.
 * 2014-12-31 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-03-14 Merged DlgRun* into here.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-17 Points view didn't update 'View Hidden' after canceling.
 * 2004-02-03 Broke dialog up into pages.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-08-18 Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "DlgDog.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgDogPanelPoints.h"
#include "DlgDogPanelProp.h"
#include "DlgDogPanelRegNums.h"
#include "DlgDogPanelTitles.h"
#include "FilterOptions.h"

#include <wx/notebook.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

wxBEGIN_EVENT_TABLE(CDlgDog, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgDog::OnOk)
wxEND_EVENT_TABLE()


CDlgDog::CDlgDog(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* pParent, int iSelectPage)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_pDog(inDog)
	, m_viewHidden(CAgilityBookOptions::GetViewHiddenTitles())
	, m_panelProps(nullptr)
	, m_panelTitles(nullptr)
	, m_panelRegNums(nullptr)
	, m_panelPoints(nullptr)
{
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDS_COL_DOG"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxNotebook* notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	notebook->Bind(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, &CDlgDog::OnPageChanging, this);

	// Properties

	m_panelProps = new CDlgDogPanelProp(m_pDoc, m_pDog, notebook);
	notebook->AddPage(m_panelProps, _("IDD_DOG_PROP"), true);

	// Titles

	m_panelTitles = new CDlgDogPanelTitles(m_pDoc, m_pDog, notebook);
	notebook->AddPage(m_panelTitles, _("IDD_DOG_TITLES"), false);

	// RegNums

	m_panelRegNums = new CDlgDogPanelRegNums(m_pDoc, m_pDog, notebook);
	notebook->AddPage(m_panelRegNums, _("IDD_DOG_NUMBERS"), false);

	// Points

	m_panelPoints = new CDlgDogPanelPoints(m_pDoc, m_pDog, notebook);
	notebook->AddPage(m_panelPoints, _("IDD_DOG_POINTS"), false);

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(notebook, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	std::vector<wxWindow*> ctrls;
	ctrls.push_back(m_panelProps->GetInitialControl());
	ctrls.push_back(m_panelTitles->GetInitialControl());
	ctrls.push_back(m_panelRegNums->GetInitialControl());
	ctrls.push_back(m_panelPoints->GetInitialControl());
	IMPLEMENT_ON_INIT(CDlgDog, ctrls[iSelectPage])

	notebook->ChangeSelection(iSelectPage);
}


void CDlgDog::OnPageChanging(wxBookCtrlEvent& evt)
{
	if (wxNOT_FOUND != evt.GetOldSelection())
	{
		if (!Validate() || !TransferDataFromWindow())
		{
			evt.Veto();
			return;
		}
	}
	evt.Skip();
}


void CDlgDog::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	bool refreshTitles = false;
	bool bModified = false;
	unsigned int hint = 0;

	bModified = m_panelProps->Save(hint);
	if (m_panelTitles->Save(hint))
	{
		bModified = true;
		refreshTitles = true;
	}
	bModified |= m_panelRegNums->Save(hint);
	bModified |= m_panelPoints->Save(hint);

	if (hint)
		bModified = true;

	if (refreshTitles || m_viewHidden != CAgilityBookOptions::GetViewHiddenTitles())
	{
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		for (ARBDogList::iterator iterDogs = m_pDoc->Book().GetDogs().begin();
			 iterDogs != m_pDoc->Book().GetDogs().end();
			 ++iterDogs)
			for (ARBDogTitleList::iterator iterTitle = (*iterDogs)->GetTitles().begin();
				 iterTitle != (*iterDogs)->GetTitles().end();
				 ++iterTitle)
			{
				if (m_pDoc->ResetVisibility(venues, *iterTitle))
					hint |= UPDATE_POINTS_VIEW;
			}
	}

	if (bModified)
		m_pDoc->Modify(true);

	if (hint)
	{
		CUpdateHint updateHint(hint);
		m_pDoc->UpdateAllViews(nullptr, &updateHint);
	}

	EndDialog(wxID_OK);
}

} // namespace dconSoft
