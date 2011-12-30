/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Various validators.
 * @author David Connet
 *
 * Revision History
 * @li 2011-12-30 DRC Use ChangeValue on textctrl to prevent change message.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-15 DRC Created
 */

#include "stdafx.h"
#include "Validators.h"

#include "ARBDate.h"
#include "ARBTypes.h"
#include "ComboBoxes.h"
#include <wx/datectrl.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_CLASS(CGenericValidator, wxValidator)
IMPLEMENT_CLASS(CTrimValidator, wxGenericValidator)
IMPLEMENT_CLASS(CQualifyingValidator, wxValidator)

/////////////////////////////////////////////////////////////////////////////

CGenericValidator::CGenericValidator(short* val)
	: m_pShort(val)
	, m_pDouble(NULL)
	, m_Prec(0)
	, m_pDate(NULL)
{
}


CGenericValidator::CGenericValidator(double* val, int inPrec)
	: m_pShort(NULL)
	, m_pDouble(val)
	, m_Prec(inPrec)
	, m_pDate(NULL)
{
}


CGenericValidator::CGenericValidator(ARBDate* val)
	: m_pShort(NULL)
	, m_pDouble(NULL)
	, m_Prec(0)
	, m_pDate(val)
{
}


CGenericValidator::CGenericValidator(CGenericValidator const& rhs)
	: m_pShort(rhs.m_pShort)
	, m_pDouble(rhs.m_pDouble)
	, m_Prec(rhs.m_Prec)
	, m_pDate(rhs.m_pDate)
{
	Copy(rhs);
}


bool CGenericValidator::Copy(CGenericValidator const& val)
{
	wxValidator::Copy(val);
	m_pShort = val.m_pShort;
	m_pDouble = val.m_pDouble;
	m_Prec = val.m_Prec;
	m_pDate = val.m_pDate;
	return true;
}


bool CGenericValidator::TransferFromWindow()
{
	// Following the example of wxGenericValidator
	if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
	{
		wxTextCtrl* pControl = (wxTextCtrl*)m_validatorWindow;
		if (m_pShort)
		{
			long val;
			if (!tstringUtil::ToLong(pControl->GetValue(), val))
				return false;
			*m_pShort = static_cast<short>(val);
			return true;
		}
		else if (m_pDouble)
		{
			return tstringUtil::ToDouble(pControl->GetValue(), *m_pDouble);
		}
	}
	else if (m_validatorWindow->IsKindOf(CLASSINFO(wxDatePickerCtrlBase)))
	{
		wxDatePickerCtrlBase* pControl = (wxDatePickerCtrlBase*)m_validatorWindow;
		if (m_pDate)
		{
			wxDateTime date = pControl->GetValue();
			if (date.IsValid())
				m_pDate->SetDate(date.GetYear(), date.GetMonth() + 1, date.GetDay());
			else
				m_pDate->clear();
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
			str.Printf(wxT("%hd"), *m_pShort);
			pControl->ChangeValue(str);
			return true;
		}
		else if (m_pDouble)
		{
			pControl->ChangeValue(ARBDouble::ToString(*m_pDouble, m_Prec));
			return true;
		}
	}
	else if (m_validatorWindow->IsKindOf(CLASSINFO(wxDatePickerCtrlBase)))
	{
		wxDatePickerCtrlBase* pControl = (wxDatePickerCtrlBase*)m_validatorWindow;
		if (m_pDate)
		{
			if (m_pDate->IsValid())
			{
				pControl->SetValue(m_pDate->GetDate());
				return true;
			}
			else
			{
				if (pControl->HasFlag(wxDP_ALLOWNONE))
				{
					wxDateTime date;
					pControl->SetValue(date);
					return true;
				}
				else
				{
					pControl->SetValue(ARBDate::Today().GetDate());
					return true;
				}
			}
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
	, m_ErrMsg()
{
}


CTrimValidator::CTrimValidator(
		wxString* valPtr,
		wxString const& errMsg)
	: wxGenericValidator(valPtr)
	, m_TrimStyle(TRIMVALIDATOR_TRIM_BOTH | TRIMVALIDATOR_NONEMPTY)
	, m_ErrMsg(errMsg)
{
}


CTrimValidator::CTrimValidator(CTrimValidator const& rhs)
	: wxGenericValidator(rhs)
	, m_TrimStyle(rhs.m_TrimStyle)
{
	Copy(rhs);
}


bool CTrimValidator::Copy(CTrimValidator const& val)
{
	wxGenericValidator::Copy(val);
	m_TrimStyle = val.m_TrimStyle;
	m_ErrMsg = val.m_ErrMsg;
	return true;
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
			textCtrl->ChangeValue(val);
		else if (comboCtrl)
			comboCtrl->SetValue(val);
	}

	bool ok = true;
	wxString errormsg;
	if ((m_TrimStyle & TRIMVALIDATOR_NONEMPTY) && val.empty())
	{
		ok = false;
		errormsg = m_ErrMsg;
		if (errormsg.empty())
			errormsg = _("IDS_NEED_VALUE");
	}

	if (!ok)
	{
		m_validatorWindow->SetFocus();
		wxMessageBox(errormsg, _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
	}
	return ok;
}

/////////////////////////////////////////////////////////////////////////////

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
			int idx = pControl->FindString(m_pQ->str(), true);
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
		wxMessageBox(wxT("ERROR: Invalid control"), _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
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
