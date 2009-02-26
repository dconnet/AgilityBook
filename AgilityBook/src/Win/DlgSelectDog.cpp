/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief implementation of the CDlgSelectDog class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-02-18 DRC Remember selected dogs.
 */

#include "stdafx.h"
#include "DlgSelectDog.h"

#include "AgilityBook.h"
#include "ARBDog.h"
#include "ARBTypes.h"
#include "AgilityBookDoc.h"
#include <wx/config.h>


BEGIN_EVENT_TABLE(CDlgSelectDog, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgSelectDog::OnOk)
END_EVENT_TABLE()


CDlgSelectDog::CDlgSelectDog(
		CAgilityBookDoc* pDoc,
		std::vector<ARBDogPtr>& outDogs,
		wxWindow* pParent)
	: wxDialog(pParent, wxID_ANY, _("IDD_DOG_PICKER"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
	, m_pDoc(pDoc)
	, m_outDogs(outDogs)
	, m_Dogs()
	, m_checkList(NULL)
{
	m_Dogs.insert(m_Dogs.end(), pDoc->Book().GetDogs().begin(), pDoc->Book().GetDogs().end());

	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);

	std::set<wxString> selection;
	long nDogs = wxConfig::Get()->Read(wxT("Selection/nDogs"), 0L);
	for (long iDog = 1; iDog <= nDogs; ++iDog)
	{
		otstringstream item;
		item << _T("Selection/Dog") << iDog;
		wxString dog = wxConfig::Get()->Read(item.str().c_str(), wxT(""));
		if (!dog.empty())
			selection.insert(dog);
	}

	// Controls (these are done first to control tab order)

	wxArrayString checkListChoices;
	for (ARBDogList::const_iterator iter = m_Dogs.begin(); iter != m_Dogs.end(); ++iter)
	{
		checkListChoices.Add((*iter)->GetCallName().c_str());
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

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_checkList, 1, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

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
	long nDogs = wxConfig::Get()->Read(wxT("Selection/nDogs"), 0L);
	for (int iDog = 1; iDog <= nDogs; ++iDog)
	{
		otstringstream item;
		item << wxT("Selection/Dog") << iDog;
		wxConfig::Get()->DeleteEntry(item.str().c_str());
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
			otstringstream item;
			item << wxT("Selection/Dog") << nDogs;
			wxConfig::Get()->Write(item.str().c_str(), m_Dogs[index]->GetCallName().c_str());
		}
	}
	wxConfig::Get()->Write(wxT("Selection/nDogs"), nDogs);
	EndDialog(wxID_OK);
}
