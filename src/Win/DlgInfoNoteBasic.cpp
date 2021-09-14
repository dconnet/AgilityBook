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
 * Revision History
 * 2021-09-08 Created
 */

#include "stdafx.h"
#include "DlgInfoNote.h"

#include "DlgName.h"
#include "ImageHelper.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class InfoNoteBasic : public InfoNotePanel
{
public:
	InfoNoteBasic(std::set<std::wstring> const& namesInUse, std::wstring const& inSelect, CDlgInfoNote* parent);

	wxWindow* GetInitialFocus() override;
	void LoadData() override;
	bool OnOk() override;

private:
	void UpdateImage(int index);
	void UpdateControls();

	wxBitmapComboBox* m_ctrlNames;
	wxButton* m_ctrlDelete;
	wxCheckBox* m_ctrlVisible;
	CSpellCheckCtrl* m_ctrlNotes;
	wxBitmap m_None;
	wxBitmap m_Note;
	wxBitmap m_Added;
	wxBitmap m_NoteAdded;

	void OnNewItem(wxCommandEvent& evt);
	void OnDeleteItem(wxCommandEvent& evt);
	void OnClickedJudgeVisible(wxCommandEvent& evt);
	void OnEnChangeComments(wxCommandEvent& evt);
};

InfoNotePanel* InfoNotePanel::CreateBasic(
	std::set<std::wstring> const& namesInUse,
	std::wstring const& inSelect,
	CDlgInfoNote* parent)
{
	return new InfoNoteBasic(namesInUse, inSelect, parent);
}

/////////////////////////////////////////////////////////////////////////////

InfoNoteBasic::InfoNoteBasic(
	std::set<std::wstring> const& namesInUse,
	std::wstring const& inSelect,
	CDlgInfoNote* parent)
	: InfoNotePanel(namesInUse, inSelect, parent)
	, m_ctrlNames(nullptr)
	, m_ctrlDelete(nullptr)
	, m_ctrlVisible(nullptr)
	, m_ctrlNotes(nullptr)
	, m_None(CImageManager::Get()->GetBitmap(ImageMgrBlank))
	, m_Note(CImageManager::Get()->GetBitmap(ImageMgrInfoNote))
	, m_Added(CImageManager::Get()->GetBitmap(ImageMgrInfoNoteAdded))
	, m_NoteAdded(CImageManager::Get()->GetBitmap(ImageMgrInfoNoteNoteAdded))
{
	m_ctrlNames = new wxBitmapComboBox(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY | wxCB_SORT);
	m_ctrlNames->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, [this](wxCommandEvent& evt) { UpdateControls(); });
	m_ctrlNames->SetHelpText(_("HIDC_INFONOTE"));
	m_ctrlNames->SetToolTip(_("HIDC_INFONOTE"));

	wxButton* ctrlNew
		= new wxButton(this, wxID_ANY, _("IDC_INFONOTE_NEW"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	ctrlNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &InfoNoteBasic::OnNewItem, this);
	ctrlNew->SetHelpText(_("HIDC_INFONOTE_NEW"));
	ctrlNew->SetToolTip(_("HIDC_INFONOTE_NEW"));

	m_ctrlDelete
		= new wxButton(this, wxID_ANY, _("IDC_INFONOTE_DELETE"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &InfoNoteBasic::OnDeleteItem, this);
	m_ctrlDelete->SetHelpText(_("HIDC_INFONOTE_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_INFONOTE_DELETE"));

	m_ctrlVisible = new wxCheckBox(this, wxID_ANY, _("IDC_INFONOTE_VISIBLE"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlVisible->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &InfoNoteBasic::OnClickedJudgeVisible, this);
	m_ctrlVisible->SetHelpText(_("HIDC_INFONOTE_VISIBLE"));
	m_ctrlVisible->SetToolTip(_("HIDC_INFONOTE_VISIBLE"));

	m_ctrlNotes = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(180, 60)),
		wxTE_MULTILINE);
	m_ctrlNotes->Bind(wxEVT_COMMAND_TEXT_UPDATED, &InfoNoteBasic::OnEnChangeComments, this);
	m_ctrlNotes->SetHelpText(_("HIDC_INFONOTE_COMMENTS"));
	m_ctrlNotes->SetToolTip(_("HIDC_INFONOTE_COMMENTS"));

	LoadData();

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerJudges = new wxBoxSizer(wxHORIZONTAL);
	sizerJudges->Add(m_ctrlNames, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(parent, 5));
	sizerJudges->Add(ctrlNew, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(parent, 5));
	sizerJudges->Add(m_ctrlDelete, 0, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerJudges, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(parent, 5));
	bSizer->Add(m_ctrlVisible, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(parent, 5));
	bSizer->Add(m_ctrlNotes, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(parent, 5));

	SetSizer(bSizer);
}


wxWindow* InfoNoteBasic::GetInitialFocus()
{
	return m_ctrlNames;
}


void InfoNoteBasic::LoadData()
{
	m_ctrlNames->Clear();
	bool bSet = false;
	for (size_t idxName = 0; idxName < m_parent->GetNames().size(); ++idxName)
	{
		// Combo box is ownerdraw.
		int index = m_ctrlNames->Append(StringUtil::stringWX(m_parent->GetNames()[idxName].m_name), m_None);
		m_ctrlNames->SetClientData(index, reinterpret_cast<void*>(idxName));
		UpdateImage(index);
		if (!bSet && 0 < m_Select.length())
		{
			if (0 == m_parent->GetNames()[idxName].m_name.find(m_Select))
			{
				m_ctrlNames->SetSelection(index);
				bSet = true;
			}
		}
	}
	if (0 == m_Select.length() || !bSet)
		m_ctrlNames->SetSelection(0);
	UpdateControls();
}


bool InfoNoteBasic::OnOk()
{
	int index = m_ctrlNames->GetSelection();
	if (0 <= index)
	{
		size_t idxName = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));
		m_parent->SetCurrentSelection(idxName);
	}
	return true;
}


void InfoNoteBasic::UpdateImage(int index)
{
	size_t idxName = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));

	wxBitmap bmp = m_None;
	if (0 < m_parent->GetAddedCount() && NameInfo::Usage::NotInUse == m_parent->GetNames()[idxName].m_usage)
	{
		if (m_parent->GetNames()[idxName].m_hasData)
			bmp = m_NoteAdded;
		else
			bmp = m_Added;
	}
	else
	{
		if (m_parent->GetNames()[idxName].m_hasData)
			bmp = m_Note;
	}
	m_ctrlNames->SetItemBitmap(index, bmp);
}


void InfoNoteBasic::UpdateControls()
{
	bool bEnable = false;
	std::wstring data;
	bool checked = true;
	int index = m_ctrlNames->GetSelection();
	if (wxNOT_FOUND != index)
	{
		size_t idxName = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));
		std::wstring name = m_parent->GetNames()[idxName].m_name;
		m_Select = name;
		ARBInfoItemPtr item = m_parent->FindName(name);
		if (item)
		{
			checked = item->IsVisible();
			data = item->GetComment();
		}
		if (m_NamesInUse.end() == m_NamesInUse.find(name))
			bEnable = true;
	}
	m_ctrlVisible->SetValue(checked);
	m_ctrlNotes->SetValue(StringUtil::stringWX(data));
	m_ctrlDelete->Enable(bEnable);
}


void InfoNoteBasic::OnClickedJudgeVisible(wxCommandEvent& evt)
{
	int index = m_ctrlNames->GetSelection();
	if (wxNOT_FOUND != index)
	{
		size_t idxName = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));
		m_parent->SetNameVisible(idxName, m_ctrlVisible->IsChecked());
		UpdateImage(index);
	}
}


void InfoNoteBasic::OnEnChangeComments(wxCommandEvent& evt)
{
	int index = m_ctrlNames->GetSelection();
	if (wxNOT_FOUND != index)
	{
		size_t idxName = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));
		std::wstring data = StringUtil::TrimRight(StringUtil::stringW(m_ctrlNotes->GetValue()));
		m_parent->SetNameComment(idxName, data);
		UpdateImage(index);
	}
	evt.Skip();
}


void InfoNoteBasic::OnNewItem(wxCommandEvent& evt)
{
	CDlgName dlg(L"", m_parent->GetCaption(), this);
	if (wxID_OK == dlg.ShowModal())
	{
		int index = -1;
		m_Select.clear();
		std::wstring name = dlg.Name();
		CDlgInfoNote::UpdateStatus status;
		size_t idxName = m_parent->AddName(name, status);
		switch (status)
		{
		case CDlgInfoNote::UpdateStatus::Exists:
			index = static_cast<int>(idxName);
			break;
		case CDlgInfoNote::UpdateStatus::Added:
			index = m_ctrlNames->Append(StringUtil::stringWX(m_parent->GetNames()[idxName].m_name));
			m_ctrlNames->SetClientData(index, reinterpret_cast<void*>(idxName));
			m_ctrlVisible->SetValue(true);
			m_ctrlNotes->SetValue(L"");
			break;
		case CDlgInfoNote::UpdateStatus::ReAdded:
			index = m_ctrlNames->Append(StringUtil::stringWX(m_parent->GetNames()[idxName].m_name));
			m_ctrlNames->SetClientData(index, reinterpret_cast<void*>(idxName));
			m_ctrlVisible->SetValue(true);
			m_ctrlNotes->SetValue(L"");
			break;
		}
		UpdateImage(index);
		m_ctrlNames->SetSelection(index);
		UpdateControls();
	}
}


void InfoNoteBasic::OnDeleteItem(wxCommandEvent& evt)
{
	int index = m_ctrlNames->GetSelection();
	if (wxNOT_FOUND != index)
	{
		size_t idxName = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));
		if (m_parent->DeleteName(idxName))
		{
			m_ctrlNames->Delete(index);
			if (index == static_cast<int>(m_ctrlNames->GetCount()))
				--index;
			if (0 <= index)
				m_ctrlNames->SetSelection(index);
			UpdateControls();
		}
		else
			wxBell();
	}
}
