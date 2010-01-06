/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief Global functions
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-14 DRC Separated from AgilityBook.cpp
 */

#include "stdafx.h"
#include "Globals.h"

#include "ListData.h"
#include "Widgets.h"
#include <wx/tokenzr.h>


/////////////////////////////////////////////////////////////////////////////

size_t BreakLine(
		char inSep,
		wxString inStr,
		std::vector<wxString>& outFields)
{
	outFields.clear();
	wxString::size_type pos = inStr.find(inSep);
	while (wxString::npos != pos)
	{
		outFields.push_back(inStr.substr(0, pos));
		inStr = inStr.substr(pos+1);
		pos = inStr.find(inSep);
	}
	outFields.push_back(inStr);
	return outFields.size();
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
	return NULL;
}


wxString GetListColumnText(
		CListCtrl const* list,
		long index,
		long col)
{
	wxString val;
	if (list)
	{
		wxListItem info;
		info.SetId(index);
		info.SetColumn(col);
		info.SetMask(wxLIST_MASK_TEXT);
		if (-1 == index)
		{
			if (list->GetColumn(col, info))
				val = info.GetText();
		}
		else
		{
			if (list->GetItem(info))
				val = info.GetText();
		}
	}
	return val;
}


bool SetListColumnText(
		CListCtrl* list,
		long index,
		long col,
		wxString const& text)
{
	if (!list)
		return false;
	wxListItem info;
	info.SetId(index);
	info.SetColumn(col);
	info.SetText(text);
	return list->SetItem(info);
}


void RefreshTreeItem(
		CTreeCtrl* tree,
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
				tree->SetItemText(item, pData->OnNeedText());
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

	wxArrayString lines = wxStringTokenize(inStr, wxT("\n"));
	for (size_t i = 0; i < lines.GetCount(); ++i)
	{
		wxString line;
		wxCoord width = 0;
		wxSize szLine;

		wxArrayString words = wxStringTokenize(lines[i], wxT(" "), wxTOKEN_RET_DELIMS);
		for (size_t k = 0; k < words.GetCount(); ++k)
		{
			szLine = pDC->GetTextExtent(words[k]);
			if ((width += szLine.x) > rect.width)
			{
				if (!bCalc)
					pDC->DrawText(line, x, y);
				y += szLine.y;
				rect.height += szLine.y;
				line = wxT("");
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
						s = wxT("");
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
