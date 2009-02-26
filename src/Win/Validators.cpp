/*
 * Copyright © 2009 David Connet. All Rights Reserved.
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
 * @brief Text validator with white space trimming.
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-15 DRC Created
 */

#include "stdafx.h"
#include "Validators.h"

#include "ARBTypes.h"


IMPLEMENT_CLASS(CGenericValidator, wxValidator)
IMPLEMENT_CLASS(CTrimValidator, wxGenericValidator)

/////////////////////////////////////////////////////////////////////////////

CGenericValidator::CGenericValidator(short* val)
	: m_pShort(val)
	, m_pDouble(NULL)
	, m_Prec(0)
{
}


CGenericValidator::CGenericValidator(double* val, int inPrec)
	: m_pShort(NULL)
	, m_pDouble(val)
	, m_Prec(inPrec)
{
}


CGenericValidator::CGenericValidator(CGenericValidator const& rhs)
	: m_pShort(rhs.m_pShort)
	, m_pDouble(rhs.m_pDouble)
	, m_Prec(rhs.m_Prec)
{
	Copy(rhs);
}


bool CGenericValidator::TransferFromWindow()
{
	// Following the example of wxGenericValidator
	if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
	{
		wxTextCtrl* pControl = (wxTextCtrl*)m_validatorWindow;
		if (m_pShort)
		{
			*m_pShort = static_cast<short>(wxAtoi(pControl->GetValue()));
			return true;
		}
		else if (m_pDouble)
		{
			pControl->GetValue().ToDouble(m_pDouble);
			return true;
		}
	}
	return false;
}


bool CGenericValidator::TransferToWindow()
{
	if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
	{
		wxTextCtrl* pControl = (wxTextCtrl*)m_validatorWindow;
		if (m_pShort)
		{
			wxString str;
			str.Printf(wxT("%d"), *m_pShort);
			pControl->SetValue(str);
			return true;
		}
		else if (m_pDouble)
		{
			tstring str = ARBDouble::str(*m_pDouble, m_Prec);
			pControl->SetValue(str.c_str());
			return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////

CTrimValidator::CTrimValidator(
		wxString* valPtr,
		long trimStyle)
	: wxGenericValidator(valPtr)
	, m_TrimStyle(trimStyle)
{
}


CTrimValidator::CTrimValidator(CTrimValidator const& rhs)
	: wxGenericValidator(rhs)
	, m_TrimStyle(rhs.m_TrimStyle)
{
}


bool CTrimValidator::TransferFromWindow()
{
	if (!wxGenericValidator::TransferFromWindow())
		return false;
	if (m_pString && (m_TrimStyle & TRIMVALIDATOR_TRIM_BOTH))
	{
		if (m_TrimStyle & TRIMVALIDATOR_TRIM_RIGHT)
			m_pString->Trim(true);
		if (m_TrimStyle & TRIMVALIDATOR_TRIM_LEFT)
			m_pString->Trim(false);
	}
	return true;
}


bool CTrimValidator::Validate(wxWindow* parent)
{
	if (!wxGenericValidator::Validate(parent))
		return false;

	if (!m_validatorWindow->IsEnabled())
		return true;

	wxTextCtrl* textCtrl = wxDynamicCast(m_validatorWindow, wxTextCtrl);
	wxComboBox* comboCtrl = wxDynamicCast(m_validatorWindow, wxComboBox);
	wxString val;
	if (textCtrl)
		val = textCtrl->GetValue();
	else if (comboCtrl)
		val = comboCtrl->GetValue();
	else
		assert(0);
	wxString orig(val);
	if (m_TrimStyle & TRIMVALIDATOR_TRIM_RIGHT)
		val.Trim(true);
	if (m_TrimStyle & TRIMVALIDATOR_TRIM_LEFT)
		val.Trim(false);
	if (val != orig)
	{
		if (textCtrl)
			textCtrl->SetValue(val);
		else if (comboCtrl)
			comboCtrl->SetValue(val);
	}

	bool ok = true;
	wxString errormsg;
	if ((m_TrimStyle & TRIMVALIDATOR_NONEMPTY) && val.empty())
	{
		ok = false;
#pragma message PRAGMA_MESSAGE("TODO: CTrimTextValidator translation")
		errormsg = wxT("String may not be empty");
	}

	if (!ok)
	{
		m_validatorWindow->SetFocus();
		wxMessageBox(errormsg, _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
	}
	return ok;
}
