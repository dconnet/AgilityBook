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
	DECLARE_NO_ASSIGN_IMPLEMENTED(CQualifyingValidator)
	CQualifyingValidator(CQualifyingValidator&& rhs) = delete;

public:
	CQualifyingValidator(ARB_Q* valPtr = nullptr, bool bAllowNoSel = false);
	CQualifyingValidator(CQualifyingValidator const& rhs);
	~CQualifyingValidator()
	{
	}

	wxObject* Clone() const override
	{
		return new CQualifyingValidator(*this);
	}
	bool Copy(CQualifyingValidator const& val);

	bool TransferFromWindow() override;
	bool TransferToWindow() override;
	bool Validate(wxWindow* parent) override;

private:
	ARB_Q* m_pQ;
	bool m_bAllowNoSel;
};
