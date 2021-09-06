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
 * Note, this class also deals with adding notes on Clubs and Locations,
 * in addition to judges. It probably should be renamed...
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
#include "DlgName.h"
#include "ImageHelper.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Widgets.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoNote dialog

wxBEGIN_EVENT_TABLE(CDlgInfoNote, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgInfoNote::OnOk)
wxEND_EVENT_TABLE()


CDlgInfoNote::CDlgInfoNote(CAgilityBookDoc* pDoc, ARBInfoType inType, std::wstring const& inSelect, wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_Type(inType)
	, m_Select(StringUtil::stringWX(inSelect))
	, m_NamesInUse()
	, m_InfoOrig(m_pDoc->Book().GetInfo().GetInfo(m_Type))
	, m_Info(m_pDoc->Book().GetInfo().GetInfo(m_Type).GetItemName())
	, m_Names()
	, m_nAdded(0)
	, m_CurSel()
	, m_ctrlNames(nullptr)
	, m_ctrlDelete(nullptr)
	, m_ctrlVisible(nullptr)
	, m_ctrlNotes(nullptr)
	, m_None(CImageManager::Get()->GetBitmap(ImageMgrBlank))
	, m_Note(CImageManager::Get()->GetBitmap(ImageMgrInfoNote))
	, m_Added(CImageManager::Get()->GetBitmap(ImageMgrInfoNoteAdded))
	, m_NoteAdded(CImageManager::Get()->GetBitmap(ImageMgrInfoNoteNoteAdded))
{
	std::wstring caption = L"?";
	std::set<std::wstring> names;
	switch (m_Type)
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
		if (m_Info.FindItem(data.m_Name, &item))
		{
			if (!item->GetComment().empty() || !item->IsVisible())
				data.m_bHasData = true;
		}
		if (m_NamesInUse.end() != std::find(m_NamesInUse.begin(), m_NamesInUse.end(), data.m_Name))
			data.m_eInUse = NameInfoUsage::InUse;
		else
			++m_nAdded;
		m_Names.push_back(data);
	}

	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	wxDialog::Create(
		pParent,
		wxID_ANY,
		caption.c_str(),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	auto panelBasic = CreateBasic();

	auto panelAdv = CreateAdvanced();
	panelAdv->Show(false);

	wxButton* ctrlAdvanced = new wxButton(this, wxID_ANY, _("IDC_INFONOTE_ADVAMCED"), wxDefaultPosition, wxDefaultSize);
	ctrlAdvanced->SetHelpText(_("HIDC_INFONOTE_ADVAMCED"));
	ctrlAdvanced->SetToolTip(_("HIDC_INFONOTE_ADVAMCED"));
	ctrlAdvanced->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this, panelBasic, panelAdv, ctrlAdvanced](wxCommandEvent&) {
		bool basicShown = panelBasic->IsShown();
		panelBasic->Show(!basicShown);
		panelAdv->Show(basicShown);
		auto btnText = basicShown ? _("IDC_INFONOTE_BASIC") : _("IDC_INFONOTE_ADVAMCED");
		ctrlAdvanced->SetLabel(btnText);
		ctrlAdvanced->SetFocus();
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

	bSizer->Add(panelBasic, 1, wxEXPAND);
	bSizer->Add(panelAdv, 1, wxEXPAND);

	wxSizer* sdbSizer = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
	sdbSizer->Insert(0, ctrlAdvanced);
	sdbSizer = CreateSeparatedSizer(sdbSizer);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgInfoNote, m_ctrlNames)
}


DEFINE_ON_INIT(CDlgInfoNote)


wxPanel* CDlgInfoNote::CreateBasic()
{
	auto panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	m_ctrlNames = new wxBitmapComboBox(
		panel,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY | wxCB_SORT);
	m_ctrlNames->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, [this](wxCommandEvent& evt) { UpdateData(); });
	m_ctrlNames->SetHelpText(_("HIDC_INFONOTE"));
	m_ctrlNames->SetToolTip(_("HIDC_INFONOTE"));

	wxButton* ctrlNew
		= new wxButton(panel, wxID_ANY, _("IDC_INFONOTE_NEW"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	ctrlNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgInfoNote::OnNewItem, this);
	ctrlNew->SetHelpText(_("HIDC_INFONOTE_NEW"));
	ctrlNew->SetToolTip(_("HIDC_INFONOTE_NEW"));

	m_ctrlDelete
		= new wxButton(panel, wxID_ANY, _("IDC_INFONOTE_DELETE"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgInfoNote::OnDeleteItem, this);
	m_ctrlDelete->SetHelpText(_("HIDC_INFONOTE_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_INFONOTE_DELETE"));

	m_ctrlVisible = new wxCheckBox(panel, wxID_ANY, _("IDC_INFONOTE_VISIBLE"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlVisible->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgInfoNote::OnClickedJudgeVisible, this);
	m_ctrlVisible->SetHelpText(_("HIDC_INFONOTE_VISIBLE"));
	m_ctrlVisible->SetToolTip(_("HIDC_INFONOTE_VISIBLE"));

	m_ctrlNotes = new CSpellCheckCtrl(
		panel,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(180, 60)),
		wxTE_MULTILINE);
	m_ctrlNotes->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CDlgInfoNote::OnEnChangeComments, this);
	m_ctrlNotes->SetHelpText(_("HIDC_INFONOTE_COMMENTS"));
	m_ctrlNotes->SetToolTip(_("HIDC_INFONOTE_COMMENTS"));

	bool bSet = false;
	for (size_t idx = 0; idx < m_Names.size(); ++idx)
	{
		// Combo box is ownerdraw.
		int index = m_ctrlNames->Append(StringUtil::stringWX(m_Names[idx].m_Name), m_None);
		m_ctrlNames->SetClientData(index, reinterpret_cast<void*>(idx));
		UpdateImage(index);
		if (!bSet && 0 < m_Select.length())
		{
			if (0 == m_Names[idx].m_Name.find(m_Select))
			{
				m_ctrlNames->SetSelection(index);
				bSet = true;
			}
		}
	}
	if (0 == m_Select.length() || !bSet)
		m_ctrlNames->SetSelection(0);
	UpdateData();

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerJudges = new wxBoxSizer(wxHORIZONTAL);
	sizerJudges->Add(m_ctrlNames, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerJudges->Add(ctrlNew, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerJudges->Add(m_ctrlDelete, 0, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerJudges, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(m_ctrlVisible, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(m_ctrlNotes, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	panel->SetSizer(bSizer);

	return panel;
}


wxPanel* CDlgInfoNote::CreateAdvanced()
{
	auto panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);

	auto test = new wxButton(panel, wxID_ANY, L"Testing", wxDefaultPosition, wxSize(500, 500));

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(test, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	panel->SetSizer(bSizer);

	return panel;
}


std::wstring CDlgInfoNote::CurrentSelection() const
{
	return m_CurSel;
}


void CDlgInfoNote::UpdateImage(int index)
{
	size_t idx = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));

	wxBitmap bmp = m_None;
	if (0 < m_nAdded && NameInfoUsage::NotInUse == m_Names[idx].m_eInUse)
	{
		if (m_Names[idx].m_bHasData)
			bmp = m_NoteAdded;
		else
			bmp = m_Added;
	}
	else
	{
		if (m_Names[idx].m_bHasData)
			bmp = m_Note;
	}
	m_ctrlNames->SetItemBitmap(index, bmp);
}


void CDlgInfoNote::UpdateData()
{
	bool bEnable = false;
	std::wstring data;
	bool checked = true;
	int index = m_ctrlNames->GetSelection();
	if (wxNOT_FOUND != index)
	{
		size_t idx = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));
		ARBInfoItemPtr item;
		if (m_Info.FindItem(m_Names[idx].m_Name, &item))
		{
			checked = item->IsVisible();
			data = item->GetComment();
		}
		if (m_NamesInUse.end() == m_NamesInUse.find(m_Names[idx].m_Name))
			bEnable = true;
	}
	m_ctrlVisible->SetValue(checked);
	m_ctrlNotes->SetValue(StringUtil::stringWX(data));
	m_ctrlDelete->Enable(bEnable);
}


void CDlgInfoNote::OnClickedJudgeVisible(wxCommandEvent& evt)
{
	int index = m_ctrlNames->GetSelection();
	if (wxNOT_FOUND != index)
	{
		size_t idx = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));
		ARBInfoItemPtr item;
		if (!m_Info.FindItem(m_Names[idx].m_Name, &item))
			m_Info.AddItem(m_Names[idx].m_Name, &item);
		if (!item)
			return;
		item->SetIsVisible(m_ctrlVisible->IsChecked());
		m_Names[idx].m_bHasData = (!item->GetComment().empty() || !item->IsVisible());
		UpdateImage(index);
	}
}


void CDlgInfoNote::OnEnChangeComments(wxCommandEvent& evt)
{
	int index = m_ctrlNames->GetSelection();
	if (wxNOT_FOUND != index)
	{
		size_t idx = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));
		std::wstring data = StringUtil::TrimRight(StringUtil::stringW(m_ctrlNotes->GetValue()));
		ARBInfoItemPtr item;
		if (!m_Info.FindItem(m_Names[idx].m_Name, &item))
			m_Info.AddItem(m_Names[idx].m_Name, &item);
		if (!item)
			return;
		item->SetComment(data);
		m_Names[idx].m_bHasData = (!item->GetComment().empty() || !item->IsVisible());
		UpdateImage(index);
	}
	evt.Skip();
}


void CDlgInfoNote::OnNewItem(wxCommandEvent& evt)
{
	wxString caption;
	switch (m_Type)
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
	CDlgName dlg(StringUtil::stringW(m_Select), caption, this);
	if (wxID_OK == dlg.ShowModal())
	{
		int index = -1;
		m_Select.clear();
		std::wstring name = dlg.Name();
		// First, check if the item exists.
		std::vector<NameInfo>::iterator iter = std::find(m_Names.begin(), m_Names.end(), name);
		if (iter != m_Names.end())
		{
			// Ok, it exists, but it may have been deleted.
			size_t idx = iter - m_Names.begin();
			// Added items cannot be in-use. So we're re-adding.
			if (NameInfoUsage::Deleted == m_Names[idx].m_eInUse)
			{
				m_Names[idx].m_eInUse = NameInfoUsage::NotInUse;
				++m_nAdded;
				index = m_ctrlNames->Append(StringUtil::stringWX(m_Names[idx].m_Name));
				m_ctrlNames->SetClientData(index, reinterpret_cast<void*>(idx));
				m_ctrlVisible->SetValue(true);
				m_ctrlNotes->SetValue(L"");
				m_Info.AddItem(m_Names[idx].m_Name);
			}
			else
				index = static_cast<int>(idx);
		}
		else
		{
			// Brand new name!
			NameInfo data(name);
			m_Names.push_back(data);
			size_t idx = m_Names.size() - 1;
			++m_nAdded;
			index = m_ctrlNames->Append(StringUtil::stringWX(m_Names[idx].m_Name));
			m_ctrlNames->SetClientData(index, reinterpret_cast<void*>(idx));
			m_ctrlVisible->SetValue(true);
			m_ctrlNotes->SetValue(L"");
			m_Info.AddItem(m_Names[idx].m_Name);
		}
		UpdateImage(index);
		m_ctrlNames->SetSelection(index);
		UpdateData();
	}
}


void CDlgInfoNote::OnDeleteItem(wxCommandEvent& evt)
{
	int index = m_ctrlNames->GetSelection();
	if (wxNOT_FOUND != index)
	{
		size_t idx = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));
		if (m_NamesInUse.end() == m_NamesInUse.find(m_Names[idx].m_Name))
		{
			m_ctrlNames->Delete(index);
			ARBInfoItemPtr item;
			if (m_Info.FindItem(m_Names[idx].m_Name, &item))
			{
				m_Info.DeleteItem(item);
			}
			if (index == static_cast<int>(m_ctrlNames->GetCount()))
				--index;
			if (0 <= index)
				m_ctrlNames->SetSelection(index);
			--m_nAdded;
			// Do NOT remove the name from m_Names. It will mess up
			// the indices of all the other items. But mark it
			// so we know if it's being re-added later.
			m_Names[idx].m_eInUse = NameInfoUsage::Deleted;
			UpdateData();
		}
		else
			wxBell();
	}
}


void CDlgInfoNote::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	int index = m_ctrlNames->GetSelection();
	if (0 <= index)
	{
		size_t idx = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));
		m_CurSel = m_Names[idx].m_Name;
	}
	m_Info.CondenseContent(m_NamesInUse);
	if (m_Info != m_InfoOrig)
	{
		m_pDoc->Book().GetInfo().GetInfo(m_Type) = m_Info;
		m_pDoc->Modify(true);
	}
	EndDialog(wxID_OK);
}
