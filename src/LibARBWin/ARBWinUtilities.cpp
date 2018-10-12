/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Global functions
 * @author David Connet
 *
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2012-10-06 Moved backup function from doc to here.
 * 2012-08-12 Moved FormatBytes to StringUtil
 * 2012-05-04 Added FormatBytes
 * 2011-04-16 Added GetFileTimes
 * 2010-10-30 Moved BreakLine to a separate file.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-14 Separated from AgilityBook.cpp
 */

#include "stdafx.h"
#include "LibARBWin/ARBWinUtilities.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/Widgets.h"
#include <wx/config.h>
#include <wx/display.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


bool CreateBackupFile(
		wxString const& inFilename,
		int nBackups)
{
	bool bChanged = false;
	if (0 < nBackups)
	{
		// First find a hole.
		int nHole = -1;
		int i;
		for (i = 1; i <= nBackups; ++i)
		{
			wxString backup = wxString::Format(L"%s.bck%d", inFilename.c_str(), i);
			if (!wxFile::Exists(backup))
			{
				nHole = i;
				break;
			}
		}
		if (-1 == nHole)
			nHole = nBackups;
		// Then shift all the files into the hole.
		for (i = nHole; i > 1; --i)
		{
			wxString backup = wxString::Format(L"%s.bck%d", inFilename.c_str(), i);
			if (wxFile::Exists(backup))
				wxRemoveFile(backup);
			wxString filename = wxString::Format(L"%s.bck%d", inFilename.c_str(), i-1);
			wxRenameFile(filename, backup);
			bChanged = true;
		}
		// File may not exist if doing a 'save as'
		if (wxFile::Exists(inFilename))
		{
			bChanged = true;
			wxString backup = inFilename + L".bck1";
			wxCopyFile(inFilename, backup, false);
		}
	}
	return bChanged;
}


bool GetFileTimes(
		wxFileName const& filename,
		wxDateTime* dtAccess,
		wxDateTime* dtMod,
		wxDateTime* dtCreate)
{
#if defined(__WXMSW__)
	// Using wx to get the times on network files is really slow.
	// I suspect it's the win32 CreateFile/GetFileTime apis.
	struct __stat64 s;
	if (0 != _tstat64(filename.GetFullPath().wx_str(), &s))
		return false;
	if (dtAccess)
		*dtAccess = wxDateTime(s.st_atime);
	if (dtMod)
		*dtMod = wxDateTime(s.st_mtime);
	if (dtCreate)
		*dtCreate = wxDateTime(s.st_ctime);
	return true;
#else
	return filename.GetTimes(dtAccess, dtMod, dtCreate);
#endif
}


wxWindow* FindWindowInSizer(
		wxSizer* sizer,
		int id)
{
	if (sizer)
	{
		wxSizerItemList& items = sizer->GetChildren();
		for (wxSizerItemList::iterator iter = items.begin(); iter != items.end(); ++iter)
		{
			if ((*iter)->IsWindow())
			{
				if ((*iter)->GetWindow()->GetId() == id)
				{
					return (*iter)->GetWindow();
				}
			}
			if ((*iter)->IsSizer())
			{
				wxWindow* win = FindWindowInSizer((*iter)->GetSizer(), id);
				if (win)
					return win;
			}
		}
	}
	return nullptr;
}


std::wstring GetListColumnText(
		CListCtrl const* list,
		long index,
		long col)
{
	std::wstring val;
	if (list)
	{
		wxListItem info;
		info.SetId(index);
		info.SetColumn(col);
		info.SetMask(wxLIST_MASK_TEXT);
		if (-1 == index)
		{
			if (list->GetColumn(col, info))
				val = StringUtil::stringW(info.GetText());
		}
		else
		{
			if (list->GetItem(info))
				val = StringUtil::stringW(info.GetText());
		}
	}
	return val;
}


bool SetListColumnText(
		CListCtrl* list,
		long index,
		long col,
		std::wstring const& text)
{
	if (!list)
		return false;
	wxListItem info;
	info.SetId(index);
	info.SetColumn(col);
	info.SetText(StringUtil::stringWX(text));
	return list->SetItem(info);
}


void RefreshTreeItem(
		wxTreeCtrl* tree,
		wxTreeItemId item,
		bool bRecurse)
{
	if (tree)
	{
		if (item.IsOk())
		{
			CTreeData* pData = dynamic_cast<CTreeData*>(tree->GetItemData(item));
			if (pData)
			{
				tree->SetItemText(item, StringUtil::stringWX(pData->OnNeedText()));
			}
			if (bRecurse)
			{
				wxTreeItemIdValue cookie;
				for (wxTreeItemId child = tree->GetFirstChild(item, cookie);
					child.IsOk();
					child = tree->GetNextChild(item, cookie))
				{
					RefreshTreeItem(tree, child, bRecurse);
				}
			}
		}
	}
}


void DrawBetterLabel(
		wxDC* pDC,
		wxString const& inStr,
		wxRect& rect,
		int flags,
		bool bCalc)
{
	if (!pDC)
		return;

	if (!bCalc)
		pDC->SetClippingRegion(rect.x, rect.y, rect.width, rect.height);

	wxCoord x = rect.x;
	wxCoord y = rect.y;
	rect.height = 0;

	wxArrayString lines = wxStringTokenize(inStr, L"\n");
	for (size_t i = 0; i < lines.GetCount(); ++i)
	{
		wxString line;
		wxCoord width = 0;
		wxSize szLine;

		wxArrayString words = wxStringTokenize(lines[i], L" ", wxTOKEN_RET_DELIMS);
		for (size_t k = 0; k < words.GetCount(); ++k)
		{
			szLine = pDC->GetTextExtent(words[k]);
			if ((width += szLine.x) > rect.width)
			{
				if (!bCalc)
					pDC->DrawText(line, x, y);
				y += szLine.y;
				rect.height += szLine.y;
				line = L"";
				width = szLine.x;
			}
			if (szLine.x > rect.width)
			{
				wxString s;
				int a = 0;
				size_t len = words[k].Length();

				for (size_t iWord = 0; iWord < len; ++iWord)
				{
					wxSize szChar = pDC->GetTextExtent(words[k][iWord]);
					if ((a += szChar.x) > rect.width)
					{
						if (!bCalc)
							pDC->DrawText(s, x, y);
						y += szChar.y;
						rect.height += szChar.y;
						s = L"";
						a = szChar.x;
					}
					s << words[k][iWord];
				}
				words[k] = s;
				width = a;
			}
			line << words[k];
		}

		if (!line.empty())
		{
			if (!bCalc)
				pDC->DrawText(line, x, y);
			y += szLine.y;
			rect.height += szLine.y;
		}
	}
	if (!bCalc)
		pDC->DestroyClippingRegion();
}
