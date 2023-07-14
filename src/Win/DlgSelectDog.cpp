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
 * 2021-03-09 Only show living dogs,
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
#include "LibARBWin/DlgPadding.h"
#include <wx/config.h>
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;


CDlgSelectDog::CDlgSelectDog(CAgilityBookDoc* pDoc, std::vector<ARBDogPtr>& outDogs, wxWindow* pParent, bool onlyLiving)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_outDogs(outDogs)
	, m_Dogs()
	, m_checkList(nullptr)
	, m_trace("CDlgSelectDog")
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDD_DOG_PICKER"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	if (onlyLiving)
	{
		std::copy_if(
			pDoc->Book().GetDogs().begin(),
			pDoc->Book().GetDogs().end(),
			std::back_inserter(m_Dogs),
			[](ARBDogPtr const& val) { return !val->GetDeceased().IsValid(); });
	}
	else
	{
		m_Dogs.insert(m_Dogs.end(), pDoc->Book().GetDogs().begin(), pDoc->Book().GetDogs().end());
	}

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
	m_checkList = new wxCheckListBox(
		this,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		checkListChoices,
		wxLB_EXTENDED | wxLB_NEEDED_SB);
	for (int index = 0; index < static_cast<int>(m_Dogs.size()); ++index)
	{
		if (selection.end() != std::find(selection.begin(), selection.end(), m_Dogs[index]->GetCallName()))
			m_checkList->Check(index, true);
	}
	m_checkList->SetHelpText(_("HIDC_PICK_NAME"));
	m_checkList->SetToolTip(_("HIDC_PICK_NAME"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_checkList, 1, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgSelectDog::OnOk, this, wxID_OK);

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
	if (1 == m_Dogs.size())
	{
		m_outDogs.push_back(*(m_Dogs.begin()));
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

} // namespace dconSoft
