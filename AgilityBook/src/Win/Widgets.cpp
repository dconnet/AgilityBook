/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Change certain default wxWidget behaviors.
 * @author David Connet
 *
 * Revision History
 * 2009-10-11 Created.
 */

#include "stdafx.h"
#include "Widgets.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_CLASS(CListCtrl, wxListView)
IMPLEMENT_CLASS(CTreeCtrl, wxTreeCtrl)
IMPLEMENT_CLASS(CTextCtrl, wxTextCtrl)
