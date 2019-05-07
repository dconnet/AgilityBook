/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgSelectDog class
 * @author David Connet
 *
 * Revision History
 * 2014-12-31 Changed pixels to dialog units.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-10 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-02-18 Remember selected dogs.
 */

#include "stdafx.h"
#include "DlgSelectDog.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "RegItems.h"

#include "ARB/ARBDog.h"
#include "ARB/ARBTypes2.h"
#include "ARBCommon/StringUtil.h"
#include <algorithm>
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


wxBEGIN_EVENT_TABLE(CDlgSelectDog, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgSelectDog::OnOk)
wxEND_EVENT_TABLE()


CDlgSelectDog::CDlgSelectDog(
		CAgilityBookDoc* pDoc,
		std::vector<ARBDogPtr>& outDogs,
		wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_outDogs(outDogs)
	, m_Dogs()
	, m_checkList(nullptr)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_DOG_PICKER"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	m_Dogs.insert(m_Dogs.end(), pDoc->Book().GetDogs().begin(), pDoc->Book().GetDogs().end());

	std::set<std::wstring> selection;
	long nDogs = wxConfig::Get()->Read(CFG_SELECTION_NDOGS, 0L);
	for (long iDog = 1; iDog <= nDogs; ++iDog)
	{
		std::wstring dog = StringUtil::stringW(wxConfig::Get()->Read(CFG_SELECTION_DOG(iDog), wxString()));
		if (!dog.empty())
			selection.insert(dog);
	}

	// Controls (these are done first to control tab order)

	wxArrayString checkListChoices;
	for (ARBDogList::const_iterator iter = m_Dogs.begin(); iter != m_Dogs.end(); ++iter)
	{
		checkListChoices.Add(StringUtil::stringWX((*iter)->GetCallName()));
	}
	m_checkList = new wxCheckListBox(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		checkListChoices,
		wxLB_EXTENDED|wxLB_NEEDED_SB);
	for (int index = 0; index < static_cast<int>(m_Dogs.size()); ++index)
	{
		if (selection.end() != std::find(selection.begin(), selection.end(), m_Dogs[index]->GetCallName()))
			m_checkList->Check(index, true);
	}
	m_checkList->SetHelpText(_("HIDC_PICK_NAME"));
	m_checkList->SetToolTip(_("HIDC_PICK_NAME"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_checkList, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_checkList->SetFocus();
}


int CDlgSelectDog::ShowModal()
{
	m_outDogs.clear();
	if (1 == m_pDoc->Book().GetDogs().size())
	{
		m_outDogs.push_back(*(m_pDoc->Book().GetDogs().begin()));
		return wxID_OK;
	}
	else
		return wxDialog::ShowModal();
}


void CDlgSelectDog::OnOk(wxCommandEvent& evt)
{
	// Erase existing.
	long nDogs = wxConfig::Get()->Read(CFG_SELECTION_NDOGS, 0L);
	for (int iDog = 1; iDog <= nDogs; ++iDog)
	{
		wxConfig::Get()->DeleteEntry(CFG_SELECTION_DOG(iDog));
	}
	// Now commit the selection.
	nDogs = 0;
	m_outDogs.clear();
	for (int index = 0; index < static_cast<int>(m_checkList->GetCount()); ++index)
	{
		if (m_checkList->IsChecked(index))
		{
			m_outDogs.push_back(m_Dogs[index]);
			++nDogs;
			wxConfig::Get()->Write(CFG_SELECTION_DOG(nDogs), m_Dogs[index]->GetCallName().c_str());
		}
	}
	wxConfig::Get()->Write(CFG_SELECTION_NDOGS, nDogs);
	EndDialog(wxID_OK);
}
