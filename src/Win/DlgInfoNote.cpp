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
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/Widgets.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class InfoNotePanel : public wxPanel
{
public:
	InfoNotePanel(std::set<std::wstring> const& namesInUse, std::wstring const& inSelect, CDlgInfoNote* parent)
		: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0)
		, m_parent(parent)
		, m_NamesInUse(namesInUse)
		, m_Select(StringUtil::stringWX(inSelect))
	{
	}

	virtual void LoadData() = 0;
	virtual bool OnOk() = 0;

protected:
	CDlgInfoNote* m_parent;
	std::set<std::wstring> const& m_NamesInUse;
	wxString m_Select;
};


class InfoNoteBasic : public InfoNotePanel
{
public:
	InfoNoteBasic(std::set<std::wstring> const& namesInUse, std::wstring const& inSelect, CDlgInfoNote* parent);
	void LoadData() override;
	bool OnOk() override;

private:
	void UpdateImage(int index);
	void UpdateData();

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


class InfoNoteAdvanced : public InfoNotePanel
{
public:
	InfoNoteAdvanced(std::set<std::wstring> const& namesInUse, std::wstring const& inSelect, CDlgInfoNote* parent);
	void LoadData() override;
	bool OnOk() override
	{
		return true;
	}

private:
};

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
	m_ctrlNames->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, [this](wxCommandEvent& evt) { UpdateData(); });
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
	UpdateData();
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


void InfoNoteBasic::UpdateData()
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
	CDlgName dlg(StringUtil::stringW(m_Select), m_parent->GetCaption(), this);
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
		UpdateData();
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
			UpdateData();
		}
		else
			wxBell();
	}
}

/////////////////////////////////////////////////////////////////////////////

InfoNoteAdvanced::InfoNoteAdvanced(
	std::set<std::wstring> const& namesInUse,
	std::wstring const& inSelect,
	CDlgInfoNote* parent)
	: InfoNotePanel(namesInUse, inSelect, parent)
{
	auto test = new wxButton(this, wxID_ANY, L"Testing", wxDefaultPosition, wxSize(500, 500));

	LoadData();

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(test, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(parent, 5));

	SetSizer(bSizer);
}


void InfoNoteAdvanced::LoadData()
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

	m_panelBasic = new InfoNoteBasic(m_NamesInUse, inSelect, this);

	m_panelAdv = new InfoNoteAdvanced(m_NamesInUse, inSelect, this);
	m_panelAdv->Show(false);

	wxButton* ctrlAdvanced = new wxButton(this, wxID_ANY, _("IDC_INFONOTE_ADVAMCED"), wxDefaultPosition, wxDefaultSize);
	ctrlAdvanced->SetHelpText(_("HIDC_INFONOTE_ADVAMCED"));
	ctrlAdvanced->SetToolTip(_("HIDC_INFONOTE_ADVAMCED"));
	ctrlAdvanced->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this, ctrlAdvanced](wxCommandEvent&) {
		bool basicShown = m_panelBasic->IsShown();
		auto panelActive = !basicShown ? m_panelBasic : m_panelAdv;
		auto panelHidden = basicShown ? m_panelBasic : m_panelAdv;
		panelActive->Show(true);
		panelHidden->Show(false);
		panelActive->LoadData();
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

	bSizer->Add(m_panelBasic, 1, wxEXPAND);
	bSizer->Add(m_panelAdv, 1, wxEXPAND);

	wxSizer* sdbSizer = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
	sdbSizer->Insert(0, ctrlAdvanced);
	sdbSizer = CreateSeparatedSizer(sdbSizer);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgInfoNote::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	auto focus = FindWindowInSizer(sdbSizer, wxID_CANCEL);
	IMPLEMENT_ON_INIT(CDlgInfoNote, focus)
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

	auto ok = m_panelBasic->IsShown() ? m_panelBasic->OnOk() : m_panelAdv->OnOk();
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
