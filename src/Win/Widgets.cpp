/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief Change certain default wxWidget behaviors.
 * @author David Connet
 *
 * Revision History
 * @li 2009-10-11 DRC Created.
 */

#include "stdafx.h"
#include "Widgets.h"


IMPLEMENT_CLASS(CListCtrl, wxListView)
IMPLEMENT_CLASS(CTreeCtrl, wxTreeCtrl)
IMPLEMENT_CLASS(CTextCtrl, wxTextCtrl)
