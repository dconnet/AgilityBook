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
 * @brief Global functions
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-14 DRC Separated from AgilityBook.cpp
 */

#include "stdafx.h"
#include "Globals.h"

#include "ListData.h"
#include <wx/tokenzr.h>


/////////////////////////////////////////////////////////////////////////////

size_t BreakLine(
		char inSep,
		tstring inStr,
		std::vector<tstring>& outFields)
{
	outFields.clear();
	tstring::size_type pos = inStr.find(inSep);
	while (tstring::npos != pos)
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
		wxListCtrl const* list,
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
		if (list->GetItem(info))
			val = info.GetText();
	}
	return val;
}


bool SetListColumnText(
		wxListCtrl* list,
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
				tree->SetItemText(item, pData->OnNeedText());
			}
			if (bRecurse)
			{
				wxTreeItemIdValue cookie;
				for (wxTreeItemId child = tree->GetFirstChild(item, cookie);
					child.IsOk();
					child = tree->GetNextChild(child, cookie))
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
