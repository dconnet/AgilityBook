#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Various validators.
 * @author David Connet
 *
 * Revision History
 * 2012-08-13 Moved CQualifyingValidator to separate file.
 * 2009-02-15 Created
 */

#include "ARB/ARB_Q.h"
#include <wx/validate.h>


/// For use with CQualifyingComboBox
class CQualifyingValidator : public wxValidator
{
DECLARE_CLASS(CQualifyingValidator)
public:
	CQualifyingValidator(ARB_Q* valPtr = NULL, bool bAllowNoSel = false);
	CQualifyingValidator(CQualifyingValidator const& rhs);

	virtual wxObject *Clone() const {return new CQualifyingValidator(*this);}
	bool Copy(CQualifyingValidator const& val);

	virtual bool TransferFromWindow();
	virtual bool TransferToWindow();
	virtual bool Validate(wxWindow* parent);

private:
	ARB_Q* m_pQ;
	bool m_bAllowNoSel;
};
