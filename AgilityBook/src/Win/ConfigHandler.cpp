/*
 * Copyright © 2008-2009 David Connet. All Rights Reserved.
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
 * @brief Implementation of IARBConfigHandler
 * @author David Connet
 *
 * Revision History
 * @li 2008-10-31 DRC Created
 */

#include "stdafx.h"
#include "ConfigHandler.h"

#include "Element.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CConfigHandler::CConfigHandler()
{
}


ElementNodePtr CConfigHandler::LoadDefaultConfig() const
{
	// We could just distribute the .xml file and load it, but I'm taking
	// advantage of Win32 resources and stashing the default config inside
	// the program.
	bool bOk = false;
	tstring errMsg;
	ARBErrorCallback err(errMsg);
	ElementNodePtr tree(ElementNode::New());
#if defined(_WIN32) && defined(_MFC_VER)
	HRSRC hrSrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_XML_DEFAULT_CONFIG), _T("XML"));
	if (hrSrc)
	{
		HGLOBAL hRes = LoadResource(AfxGetResourceHandle(), hrSrc);
		if (hRes)
		{
			DWORD size = SizeofResource(AfxGetResourceHandle(), hrSrc);
			char const* pData = reinterpret_cast<char const*>(LockResource(hRes));
			bOk = tree->LoadXMLBuffer(pData, size, errMsg);
			FreeResource(hRes);
		}
	}
#elif defined(WXWIDGETS)
#pragma message ( __FILE__ "(" STRING(__LINE__) ") : TODO: WXWIDGETS DefaultConfig.xml usage" )
	bOk = tree->LoadXMLFile("DefaultConfig.xml", errMsg);
#else
#pragma message ( __FILE__ "(" STRING(__LINE__) ") : TODO: DefaultConfig.xml usage" )
	// @todo: Porting issues: This needs more work...
	// This will work, but we need to make sure DefaultConfig.xml is
	// distributed - there's also the issue of paths...
	bOk = tree->LoadXMLFile("DefaultConfig.xml", errMsg);
#endif
	return bOk ? tree : ElementNodePtr();
}


std::string CConfigHandler::LoadDTD(bool bNormalizeCRNL) const
{
	std::string dtd;
#if defined(_WIN32) && defined(_MFC_VER)
	HRSRC hrSrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_DTD_AGILITYRECORDBOOK), _T("DTD"));
	if (hrSrc)
	{
		HGLOBAL hRes = LoadResource(AfxGetResourceHandle(), hrSrc);
		if (hRes)
		{
			DWORD size = SizeofResource(AfxGetResourceHandle(), hrSrc);
			char const* pData = reinterpret_cast<char const*>(LockResource(hRes));
			dtd = std::string(pData, size);
			if (bNormalizeCRNL)
				dtd = tstringUtil::Replace(dtd, "\r\n", "\n");
			FreeResource(hRes);
		}
	}
#elif defined(WXWIDGETS)
#pragma message ( __FILE__ "(" STRING(__LINE__) ") : TODO: WXWIDGETS DTD usage" )
	// @todo: Porting issues: Not currently implemented
	dtd = "<!-- Not implemented on non-windows platforms -->\n";
#else
#pragma message ( __FILE__ "(" STRING(__LINE__) ") : TODO: DTD usage" )
	// @todo: Porting issues: Not currently implemented
	dtd = "<!-- Not implemented on non-windows platforms -->\n";
#endif
	return dtd;
}
