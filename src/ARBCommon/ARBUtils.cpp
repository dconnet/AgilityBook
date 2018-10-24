/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

#include "stdafx.h"
#include "ARBCommon/ARBUtils.h"

#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


#if USE_STACKTRACER && defined(WIN32)
int CStackTracer::fIndent = 0;
#endif


CConfigPathHelper::CConfigPathHelper(wxString const& key)
	: m_path(wxConfig::Get()->GetPath())
{
	if (!key.empty())
	{
		wxConfig::Get()->SetPath(key);
	}
}


CConfigPathHelper::~CConfigPathHelper()
{
	wxConfig::Get()->SetPath(m_path);
}
