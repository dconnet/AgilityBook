#pragma once

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
 * 2021-09-19 Changed infonote from a combobox to a listctrl.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-10 Ported to wxWidgets.
 * 2008-02-01 Add ability to see what was last selected.
 * 2008-01-01 Added visible flag.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2003-12-07 Created
 */

#include "ARB/ARBInfo.h"
#include "LibARBWin/ReportListHeader.h"
#include <set>
#include <vector>
class CAgilityBookDoc;
class CReportListCtrl;


ARB_TYPEDEF(InfoNoteListData)

class NameInfo
{
public:
	enum class Usage
	{
		NotInUse,
		InUse,
		Deleted
	};

	NameInfo()
		: m_name()
		, m_usage(Usage::NotInUse)
	{
	}
	NameInfo(std::wstring const& inName)
		: m_name(inName)
		, m_usage(Usage::NotInUse)
	{
	}
	NameInfo(NameInfo const& rhs)
		: m_name(rhs.m_name)
		, m_usage(rhs.m_usage)
	{
	}
	NameInfo(NameInfo&& rhs)
		: m_name(std::move(rhs.m_name))
		, m_usage(std::move(rhs.m_usage))
	{
	}
	~NameInfo()
	{
	}

	NameInfo& operator=(NameInfo const& rhs)
	{
		if (this != &rhs)
		{
			m_name = rhs.m_name;
			m_usage = rhs.m_usage;
		}
		return *this;
	}
	NameInfo& operator=(NameInfo&& rhs)
	{
		if (this != &rhs)
		{
			m_name = std::move(rhs.m_name);
			m_usage = std::move(rhs.m_usage);
		}
		return *this;
	}
	bool operator==(NameInfo const& rhs) const
	{
		return m_name == rhs.m_name;
	}
	bool operator!=(NameInfo const& rhs) const
	{
		return !operator==(rhs);
	}

	std::wstring m_name;
	Usage m_usage;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgInfoNote : public wxDialog
{
	friend class InfoNoteListData;

public:
	CDlgInfoNote(CAgilityBookDoc* pDoc, ARBInfoType inType, std::wstring const& inSelect, wxWindow* pParent = nullptr);

	wxString GetCaption() const;
	std::wstring CurrentSelection() const;

	bool NameExists(std::wstring const& name);
	enum class UpdateStatus
	{
		Exists,
		Added,
		ReAdded,
	};
	size_t AddName(std::wstring const& name, UpdateStatus& status);
	bool DeleteName(size_t idxName);

	size_t GetAddedCount() const
	{
		return m_nAdded;
	}
	ARBInfoItemPtr FindName(std::wstring const& name) const;

	std::vector<NameInfo> const& GetNames() const
	{
		return m_Names;
	}

private:
	InfoNoteListDataPtr GetData(long index) const;
	bool IsItemVisible(size_t idxName) const;
	void LoadItems(std::wstring const* inSelect = nullptr);
	void UpdateText();
	bool UpdateItem(long index, size_t idxName);
	void UpdateControls();
	void DoEdit();
	void DoEdit(long index);

	CAgilityBookDoc* m_pDoc;
	ARBInfoType m_type;
	std::set<std::wstring> m_NamesInUse;
	ARBInfoItemList const& m_InfoOrig;
	ARBInfoItemList m_Info;
	std::vector<NameInfo> m_Names;
	size_t m_nAdded;
	std::wstring m_CurSel;
	enum class ViewVis
	{
		All,
		Visible,
		Hidden,
	};
	ViewVis m_viewVis;
	enum class ViewUse
	{
		All,
		InUse,
		NotInUse,
	};
	ViewUse m_viewUse;

	wxButton* m_ctrlEdit;
	wxButton* m_ctrlDelete;
	wxButton* m_ctrlCopy; 
	CReportListCtrl* m_ctrlList;
	CReportListHeader m_reportColumn;
	int m_imgNone;
	int m_imgNote;
	int m_imgAdded;
	int m_imgNoteAdded;
	wxStaticText* m_textCount;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()

	void OnItemFocused(wxListEvent& evt);
	void OnDeleteItem(wxCommandEvent& evt);
	void OnCopyItems(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
