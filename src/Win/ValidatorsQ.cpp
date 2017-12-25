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
 */

#include "stdafx.h"
#include "ValidatorsQ.h"

#include "ComboBoxQ.h"

#include "ARBCommon/StringUtil.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_CLASS(CQualifyingValidator, wxValidator)


CQualifyingValidator::CQualifyingValidator(
		ARB_Q* valPtr,
		bool bAllowNoSel)
	: m_pQ(valPtr)
	, m_bAllowNoSel(bAllowNoSel)
{
}


CQualifyingValidator::CQualifyingValidator(CQualifyingValidator const& rhs)
	: m_pQ(rhs.m_pQ)
	, m_bAllowNoSel(rhs.m_bAllowNoSel)
{
	Copy(rhs);
}


bool CQualifyingValidator::Copy(CQualifyingValidator const& val)
{
	wxValidator::Copy(val);
	m_pQ = val.m_pQ;
	m_bAllowNoSel = val.m_bAllowNoSel;
	return true;
}


bool CQualifyingValidator::TransferFromWindow()
{
	if (m_validatorWindow->IsKindOf(CLASSINFO(CQualifyingComboBox)))
	{
		CQualifyingComboBox* pControl = (CQualifyingComboBox*)m_validatorWindow;
		if (!m_bAllowNoSel && wxNOT_FOUND == pControl->GetSelection())
			return false;
		if (m_pQ)
		{
			*m_pQ = pControl->GetQ(pControl->GetSelection());
			return true;
		}
	}
	return false;
}


bool CQualifyingValidator::TransferToWindow()
{
	if (m_validatorWindow->IsKindOf(CLASSINFO(CQualifyingComboBox)))
	{
		CQualifyingComboBox* pControl = (CQualifyingComboBox*)m_validatorWindow;
		if (m_pQ)
		{
			int idx = pControl->FindString(StringUtil::stringWX(m_pQ->str()), true);
			pControl->SetSelection(idx);
			return true;
		}
	}
	return false;
}


bool CQualifyingValidator::Validate(wxWindow* parent)
{
	if (!m_validatorWindow->IsKindOf(CLASSINFO(CQualifyingComboBox)))
	{
		m_validatorWindow->SetFocus();
		wxMessageBox(L"ERROR: Invalid control", _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
		return false;
	}
	if (!m_bAllowNoSel)
	{
		CQualifyingComboBox* pControl = (CQualifyingComboBox*)m_validatorWindow;
		if (wxNOT_FOUND == pControl->GetSelection())
		{
			m_validatorWindow->SetFocus();
			wxMessageBox(_("IDS_SELECT_Q"), _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
			return false;
		}
	}
	return true;
}
