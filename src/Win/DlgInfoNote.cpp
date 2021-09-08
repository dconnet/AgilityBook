/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Dialog for adding notes to ARBInfo items.
 * @author David Connet
 *
 * Remember, when adding an entry, it is only saved if there is a comment.
 *
 * Revision History
 * 2014-12-31 Changed pixels to dialog units.
 * 2012-05-22 Change KillFocus handler to text change handler.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-12 Fix killfocus handling.
 * 2009-02-10 Ported to wxWidgets.
 * 2008-02-01 Add ability to see what was last selected.
 * 2008-01-01 Added visible flag.
 * 2007-12-03 SelectString was still used in OnNew.
 *            Fix a drawing problem in the drop list.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-13 SelectString doesn't work in ownerdraw combo. Changed
 *            ctor interface for more versatility.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-24-05 If there were 0 items in the list, it crashed.
 *            Allow saving an entry with no comment.
 * 2004-12-11 Added indicators if item is added and/or has comments.
 *            Merged in club/location support (was in separate files
 *            that were added 11/18/04.
 * 2003-12-07 Created
 */

#include "stdafx.h"
#include "DlgInfoNote.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"

#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

InfoNotePanel::InfoNotePanel(
	std::set<std::wstring> const& namesInUse,
	std::wstring const& inSelect,
	CDlgInfoNote* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0)
	, m_parent(parent)
	, m_NamesInUse(namesInUse)
	, m_Select(StringUtil::stringWX(inSelect))
{
}

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoNote dialog

CDlgInfoNote::CDlgInfoNote(CAgilityBookDoc* pDoc, ARBInfoType inType, std::wstring const& inSelect, wxWindow* parent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_type(inType)
	, m_NamesInUse()
	, m_InfoOrig(m_pDoc->Book().GetInfo().GetInfo(inType))
	, m_Info(m_pDoc->Book().GetInfo().GetInfo(inType).GetItemName())
	, m_Names()
	, m_nAdded(0)
	, m_CurSel()
	, m_panelBasic(nullptr)
	, m_panelAlternate(nullptr)
{
	std::wstring caption = L"?";
	std::set<std::wstring> names;
	switch (m_type)
	{
	case ARBInfoType::Club:
		m_pDoc->Book().GetAllClubNames(m_NamesInUse, false, false);
		m_pDoc->Book().GetAllClubNames(names, true, false);
		caption = _("IDS_INFO_CLUB");
		break;
	case ARBInfoType::Judge:
		m_pDoc->Book().GetAllJudges(m_NamesInUse, false, false);
		m_pDoc->Book().GetAllJudges(names, true, false);
		caption = _("IDS_INFO_JUDGE");
		break;
	case ARBInfoType::Location:
		m_pDoc->Book().GetAllTrialLocations(m_NamesInUse, false, false);
		m_pDoc->Book().GetAllTrialLocations(names, true, false);
		caption = _("IDS_INFO_LOCATION");
		break;
	}
	m_InfoOrig.Clone(m_Info);

	m_Names.reserve(names.size());
	for (std::set<std::wstring>::iterator iter = names.begin(); iter != names.end(); ++iter)
	{
		NameInfo data(*iter);
		ARBInfoItemPtr item;
		if (m_Info.FindItem(data.m_name, &item))
		{
			if (!item->GetComment().empty() || !item->IsVisible())
				data.m_hasData = true;
		}
		if (m_NamesInUse.end() != std::find(m_NamesInUse.begin(), m_NamesInUse.end(), data.m_name))
			data.m_usage = NameInfo::Usage::InUse;
		else
			++m_nAdded;
		m_Names.push_back(data);
	}

	if (!parent)
		parent = wxGetApp().GetTopWindow();
	wxDialog::Create(
		parent,
		wxID_ANY,
		caption.c_str(),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_panelBasic = InfoNotePanel::CreateBasic(m_NamesInUse, inSelect, this);
	m_panelAlternate = InfoNotePanel::CreateAlternate(m_NamesInUse, inSelect, this);

	wxWindow* ctrlFocus = nullptr;
	wxString textAlternate;
	bool showBasic = true; // TODO: remember last shown
	if (showBasic)
	{
		m_panelAlternate->Show(false);
		textAlternate = _("IDC_INFONOTE_ALTERNATE_EXPAND");
		ctrlFocus = m_panelBasic->GetInitialFocus();
	}
	else
	{
		m_panelBasic->Show(false);
		textAlternate = _("IDC_INFONOTE_ALTERNATE_COLLAPSE");
		ctrlFocus = m_panelAlternate->GetInitialFocus();
	}

	wxButton* ctrlAlternate = new wxButton(this, wxID_ANY, textAlternate, wxDefaultPosition, wxDefaultSize);
	ctrlAlternate->SetHelpText(_("HIDC_INFONOTE_ALTERNATE"));
	ctrlAlternate->SetToolTip(_("HIDC_INFONOTE_ALTERNATE"));
	ctrlAlternate->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this, ctrlAlternate](wxCommandEvent&) {
		bool showBasic = !m_panelBasic->IsShown();
		auto panelActive = showBasic ? m_panelBasic : m_panelAlternate;
		auto panelHidden = !showBasic ? m_panelBasic : m_panelAlternate;
		panelActive->Show(true);
		panelHidden->Show(false);
		panelActive->SetSelection(panelHidden->GetSelection());
		panelActive->LoadData();
		ctrlAlternate->SetLabel(showBasic ? _("IDC_INFONOTE_ALTERNATE_EXPAND") : _("IDC_INFONOTE_ALTERNATE_COLLAPSE"));
		panelActive->GetInitialFocus()->SetFocus();
		// Reset min size constraints or dialog won't scale down.
		m_minWidth = -1;
		m_maxWidth = -1;
		m_minHeight = -1;
		m_maxHeight = -1;
		// Resize dialog
		GetSizer()->Fit(this);
		SetSizeHints(GetSize(), wxDefaultSize);
		CenterOnParent();
	});

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	bSizer->Add(m_panelBasic, 1, wxEXPAND);
	bSizer->Add(m_panelAlternate, 1, wxEXPAND);

	wxSizer* sdbSizer = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
	sdbSizer->Insert(0, ctrlAlternate);
	sdbSizer = CreateSeparatedSizer(sdbSizer);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgInfoNote::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgInfoNote, ctrlFocus)
}


DEFINE_ON_INIT(CDlgInfoNote)


wxString CDlgInfoNote::GetCaption() const
{
	wxString caption;
	switch (m_type)
	{
	case ARBInfoType::Club:
		caption = _("IDS_COL_CLUB");
		break;
	case ARBInfoType::Judge:
		caption = _("IDS_COL_JUDGE");
		break;
	case ARBInfoType::Location:
		caption = _("IDS_COL_LOCATION");
		break;
	}
	return caption;
}


std::wstring CDlgInfoNote::CurrentSelection() const
{
	return m_CurSel;
}


// Note: Only called during OnOk.
void CDlgInfoNote::SetCurrentSelection(size_t current)
{
	m_CurSel = m_Names[current].m_name;
}


size_t CDlgInfoNote::AddName(std::wstring const& name, UpdateStatus& status)
{
	size_t idxName;
	auto iter = std::find(m_Names.begin(), m_Names.end(), name);
	if (iter != m_Names.end())
	{
		// Ok, it exists, but it may have been deleted.
		idxName = iter - m_Names.begin();
		// Added items cannot be in-use. So we're re-adding.
		if (NameInfo::Usage::Deleted == m_Names[idxName].m_usage)
		{
			m_Names[idxName].m_usage = NameInfo::Usage::NotInUse;
			++m_nAdded;
			status = UpdateStatus::ReAdded;
			m_Info.AddItem(m_Names[idxName].m_name);
		}
		else
			status = UpdateStatus::Exists;
	}
	else
	{
		// Brand new name!
		status = UpdateStatus::Added;
		NameInfo data(name);
		m_Names.push_back(data);
		idxName = m_Names.size() - 1;
		++m_nAdded;
		m_Info.AddItem(m_Names[idxName].m_name);
	}
	return idxName;
}


bool CDlgInfoNote::DeleteName(size_t idxName)
{
	if (m_NamesInUse.end() == m_NamesInUse.find(m_Names[idxName].m_name))
	{
		ARBInfoItemPtr item;
		if (m_Info.FindItem(m_Names[idxName].m_name, &item))
		{
			m_Info.DeleteItem(item);
		}
		--m_nAdded;
		// Do NOT remove the name from m_Names. It will mess up
		// the indices of all the other items. But mark it
		// so we know if it's being re-added later.
		m_Names[idxName].m_usage = NameInfo::Usage::Deleted;
		return true;
	}
	else
		return false;
}


void CDlgInfoNote::SetNameVisible(size_t idxName, bool visible)
{
	ARBInfoItemPtr item;
	if (!m_Info.FindItem(m_Names[idxName].m_name, &item))
		m_Info.AddItem(m_Names[idxName].m_name, &item);
	if (!item)
		return;
	item->SetIsVisible(visible);
	m_Names[idxName].m_hasData = (!item->GetComment().empty() || !item->IsVisible());
}


void CDlgInfoNote::SetNameComment(size_t idxName, std::wstring const& data)
{
	ARBInfoItemPtr item;
	if (!m_Info.FindItem(m_Names[idxName].m_name, &item))
		m_Info.AddItem(m_Names[idxName].m_name, &item);
	if (!item)
		return;
	item->SetComment(data);
	m_Names[idxName].m_hasData = (!item->GetComment().empty() || !item->IsVisible());
}


ARBInfoItemPtr CDlgInfoNote::FindName(std::wstring const& name) const
{
	ARBInfoItemPtr item;
	m_Info.FindItem(name, &item);
	return item;
}


void CDlgInfoNote::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	auto ok = m_panelBasic->IsShown() ? m_panelBasic->OnOk() : m_panelAlternate->OnOk();
	if (!ok)
		return;

	m_Info.CondenseContent(m_NamesInUse);
	if (m_Info != m_InfoOrig)
	{
		m_pDoc->Book().GetInfo().GetInfo(m_type) = m_Info;
		m_pDoc->Modify(true);
	}
	EndDialog(wxID_OK);
}
