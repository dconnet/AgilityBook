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
