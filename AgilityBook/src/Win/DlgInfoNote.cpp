/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Dialog for adding notes to ARBInfo items.
 * @author David Connet
 *
 * Note, this class also deals with adding notes on Clubs and Locations,
 * in addition to judges. It probably should be renamed...
 *
 * Remember, when adding an entry, it is only saved if there is a comment.
 *
 * Revision History
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-08-12 DRC Fix killfocus handling.
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2008-02-01 DRC Add ability to see what was last selected.
 * @li 2008-01-01 DRC Added visible flag.
 * @li 2007-12-03 DRC SelectString was still used in OnNew.
 *                    Fix a drawing problem in the drop list.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-13 DRC SelectString doesn't work in ownerdraw combo. Changed
 *                    ctor interface for more versatility.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-24-05 DRC If there were 0 items in the list, it crashed.
 *                    Allow saving an entry with no comment.
 * @li 2004-12-11 DRC Added indicators if item is added and/or has comments.
 *                    Merged in club/location support (was in separate files
 *                    that were added 11/18/04.
 * @li 2003-12-07 DRC Created
 */

#include "stdafx.h"
#include "DlgInfoNote.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "DlgName.h"
#include "Widgets.h"
#include <algorithm>
#include <wx/bmpcbox.h>

#include "res/CalEmpty.xpm"
#include "res/NoteNote.xpm"
#include "res/NoteAdded.xpm"
#include "res/NoteNoteAdded.xpm"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

CDlgInfoNote::NameInfo::NameInfo()
	: m_Name()
	, m_eInUse(eNotInUse)
	, m_bHasData(false)
{
}


CDlgInfoNote::NameInfo::NameInfo(wxString const& inName)
	: m_Name(inName)
	, m_eInUse(eNotInUse)
	, m_bHasData(false)
{
}


CDlgInfoNote::NameInfo::NameInfo(NameInfo const& rhs)
	: m_Name(rhs.m_Name)
	, m_eInUse(rhs.m_eInUse)
	, m_bHasData(rhs.m_bHasData)
{
}


bool CDlgInfoNote::NameInfo::operator==(NameInfo const& rhs)
{
	return m_Name == rhs.m_Name;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoNote dialog

BEGIN_EVENT_TABLE(CDlgInfoNote, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgInfoNote::OnOk)
END_EVENT_TABLE()


CDlgInfoNote::CDlgInfoNote(
		CAgilityBookDoc* pDoc,
		ARBInfo::eInfoType inType,
		wxString const& inSelect,
		wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_Type(inType)
	, m_Select(inSelect)
	, m_NamesInUse()
	, m_InfoOrig(m_pDoc->Book().GetInfo().GetInfo(m_Type))
	, m_Info(m_pDoc->Book().GetInfo().GetInfo(m_Type).GetItemName())
	, m_Names()
	, m_nAdded(0)
	, m_CurSel()
	, m_ctrlNames(NULL)
	, m_ctrlDelete(NULL)
	, m_ctrlVisible(NULL)
	, m_ctrlNotes(NULL)
	, m_None(CalEmpty_xpm)
	, m_Note(NoteNote_xpm)
	, m_Added(NoteAdded_xpm)
	, m_NoteAdded(NoteNoteAdded_xpm)
{
	wxString caption = _("IDD_JUDGE_INFO");
	std::set<wxString> names;
	switch (m_Type)
	{
	case ARBInfo::eClubInfo:
		m_pDoc->Book().GetAllClubNames(m_NamesInUse, false, false);
		m_pDoc->Book().GetAllClubNames(names, true, false);
		caption = _("IDS_INFO_CLUB");
		break;
	case ARBInfo::eJudgeInfo:
		m_pDoc->Book().GetAllJudges(m_NamesInUse, false, false);
		m_pDoc->Book().GetAllJudges(names, true, false);
		caption = _("IDS_INFO_JUDGE");
		break;
	case ARBInfo::eLocationInfo:
		m_pDoc->Book().GetAllTrialLocations(m_NamesInUse, false, false);
		m_pDoc->Book().GetAllTrialLocations(names, true, false);
		caption = _("IDS_INFO_LOCATION");
		break;
	}
	m_InfoOrig.Clone(m_Info);

	m_Names.reserve(names.size());
	for (std::set<wxString>::iterator iter = names.begin(); iter != names.end(); ++iter)
	{
		NameInfo data(*iter);
		ARBInfoItemPtr item;
		if (m_Info.FindItem(data.m_Name, &item))
		{
			if (!item->GetComment().empty() || !item->IsVisible())
				data.m_bHasData = true;
		}
		if (m_NamesInUse.end() != std::find(m_NamesInUse.begin(), m_NamesInUse.end(), data.m_Name))
			data.m_eInUse = NameInfo::eInUse;
		else
			++m_nAdded;
		m_Names.push_back(data);
	}

	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	wxDialog::Create(pParent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_ctrlNames = new wxBitmapComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL,
		wxCB_DROPDOWN|wxCB_READONLY|wxCB_SORT);
	BIND_OR_CONNECT_CTRL(m_ctrlNames, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgInfoNote::OnSelchangeName);
	m_ctrlNames->SetHelpText(_("HIDC_INFONOTE"));
	m_ctrlNames->SetToolTip(_("HIDC_INFONOTE"));

	wxButton* ctrlNew = new wxButton(this, wxID_ANY,
		_("IDC_INFONOTE_NEW"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	BIND_OR_CONNECT_CTRL(ctrlNew, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgInfoNote::OnNewItem);
	ctrlNew->SetHelpText(_("HIDC_INFONOTE_NEW"));
	ctrlNew->SetToolTip(_("HIDC_INFONOTE_NEW"));

	m_ctrlDelete = new wxButton(this, wxID_ANY,
		_("IDC_INFONOTE_DELETE"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	BIND_OR_CONNECT_CTRL(m_ctrlDelete, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgInfoNote::OnDeleteItem);
	m_ctrlDelete->SetHelpText(_("HIDC_INFONOTE_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_INFONOTE_DELETE"));

	m_ctrlVisible = new wxCheckBox(this, wxID_ANY,
		_("IDC_INFONOTE_VISIBLE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlVisible, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler, CDlgInfoNote::OnClickedJudgeVisible);
	m_ctrlVisible->SetHelpText(_("HIDC_INFONOTE_VISIBLE"));
	m_ctrlVisible->SetToolTip(_("HIDC_INFONOTE_VISIBLE"));

	m_ctrlNotes = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(300, 100), wxTE_MULTILINE);
	m_ctrlNotes->SetHelpText(_("HIDC_INFONOTE_COMMENTS"));
	m_ctrlNotes->SetToolTip(_("HIDC_INFONOTE_COMMENTS"));

	bool bSet = false;
	for (size_t idx = 0; idx < m_Names.size(); ++idx)
	{
		// Combo box is ownerdraw.
		int index = m_ctrlNames->Append(m_Names[idx].m_Name);
		m_ctrlNames->SetClientData(index, (void*)idx);
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

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerJudges = new wxBoxSizer(wxHORIZONTAL);
	sizerJudges->Add(m_ctrlNames, 1, wxALL, 5);
	sizerJudges->Add(ctrlNew, 0, wxALL, 5);
	sizerJudges->Add(m_ctrlDelete, 0, wxALL, 5);

	bSizer->Add(sizerJudges, 0, wxEXPAND, 5);
	bSizer->Add(m_ctrlVisible, 0, wxALL, 5);
	bSizer->Add(m_ctrlNotes, 1, wxALL|wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	// Bind killfocus handlers last
	BIND_OR_CONNECT_CTRL(m_ctrlNotes, wxEVT_KILL_FOCUS, wxFocusEventHandler, CDlgInfoNote::OnKillfocusComments);
}


void CDlgInfoNote::UpdateImage(int index)
{
	size_t idx = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));

	wxBitmap bmp = m_None;
	if (0 < m_nAdded && NameInfo::eNotInUse == m_Names[idx].m_eInUse)
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
	wxString data;
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
	m_ctrlNotes->SetValue(data);
	m_ctrlDelete->Enable(bEnable);
}


void CDlgInfoNote::OnSelchangeName(wxCommandEvent& evt)
{
	UpdateData();
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


void CDlgInfoNote::OnKillfocusComments(wxFocusEvent& evt)
{
	int index = m_ctrlNames->GetSelection();
	if (wxNOT_FOUND != index)
	{
		size_t idx = reinterpret_cast<size_t>(m_ctrlNames->GetClientData(index));
		wxString data = m_ctrlNotes->GetValue();
		data.Trim();
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
	case ARBInfo::eClubInfo:
		caption = _("IDS_COL_CLUB");
		break;
	case ARBInfo::eJudgeInfo:
		caption = _("IDS_COL_JUDGE");
		break;
	case ARBInfo::eLocationInfo:
		caption = _("IDS_COL_LOCATION");
		break;
	}
	CDlgName dlg(m_Select, caption, this);
	if (wxID_OK == dlg.ShowModal())
	{
		int index = -1;
		m_Select.erase();
		wxString name = dlg.GetName();
		// First, check if the item exists.
		std::vector<NameInfo>::iterator iter = std::find(m_Names.begin(), m_Names.end(), name);
		if (iter != m_Names.end())
		{
			// Ok, it exists, but it may have been deleted.
			size_t idx = iter - m_Names.begin();
			// Added items cannot be in-use. So we're re-adding.
			if (NameInfo::eDeleted == m_Names[idx].m_eInUse)
			{
				m_Names[idx].m_eInUse = NameInfo::eNotInUse;
				++m_nAdded;
				index = m_ctrlNames->Append(m_Names[idx].m_Name);
				m_ctrlNames->SetClientData(index, (void*)idx);
				m_ctrlVisible->SetValue(true);
				m_ctrlNotes->SetValue(wxT(""));
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
			index = m_ctrlNames->Append(m_Names[idx].m_Name);
			m_ctrlNames->SetClientData(index, (void*)idx);
			m_ctrlVisible->SetValue(true);
			m_ctrlNotes->SetValue(wxT(""));
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
			m_Names[idx].m_eInUse = NameInfo::eDeleted;
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
