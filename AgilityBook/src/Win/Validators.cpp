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
 * @li 2012-01-02 DRC Change validator to support default value on empty field.
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

CGenericValidator::CGenericValidator(
		short* val,
		short defVal,
		bool bUseDefOnEmpty,
		wxChar const* errMsg)
	: m_pShort(val)
	, m_pDouble(NULL)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(bUseDefOnEmpty)
	, m_pDate(NULL)
	, m_ErrMsg()
{
	m_Default.s = defVal;
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(
		double* val,
		int inPrec,
		double defVal,
		bool bUseDefOnEmpty,
		wxChar const* errMsg)
	: m_pShort(NULL)
	, m_pDouble(val)
	, m_Prec(inPrec)
	, m_Default()
	, m_bUseDefOnEmpty(bUseDefOnEmpty)
	, m_pDate(NULL)
	, m_ErrMsg()
{
	m_Default.dbl = defVal;
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(
		ARBDate* val,
		wxChar const* errMsg)
	: m_pShort(NULL)
	, m_pDouble(NULL)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(false)
	, m_pDate(val)
	, m_ErrMsg()
{
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(CGenericValidator const& rhs)
	: m_pShort(rhs.m_pShort)
	, m_pDouble(rhs.m_pDouble)
	, m_Prec(rhs.m_Prec)
	, m_Default(rhs.m_Default)
	, m_bUseDefOnEmpty(rhs.m_bUseDefOnEmpty)
	, m_pDate(rhs.m_pDate)
	, m_ErrMsg(rhs.m_ErrMsg)
{
	Copy(rhs);
}


bool CGenericValidator::Copy(CGenericValidator const& val)
{
	wxValidator::Copy(val);
	m_pShort = val.m_pShort;
	m_pDouble = val.m_pDouble;
	m_Prec = val.m_Prec;
	m_Default = val.m_Default;
	m_bUseDefOnEmpty= val.m_bUseDefOnEmpty;
	m_pDate = val.m_pDate;
	m_ErrMsg = val.m_ErrMsg;
	return true;
}


bool CGenericValidator::TransferFromWindow()
{
	// Following the example of wxGenericValidator
	if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
	{
		wxTextCtrl* pTextControl = (wxTextCtrl*)m_validatorWindow;
		wxString textVal = pTextControl->GetValue();
		if (m_pShort)
		{
			if (textVal.empty() && m_bUseDefOnEmpty)
			{
				*m_pShort = m_Default.s;
				return true;
			}
			else
			{
				long val = 0;
				if (!StringUtil::ToLong(textVal, val))
					return false;
				*m_pShort = static_cast<short>(val);
				return true;
			}
		}
		else if (m_pDouble)
		{
			if (textVal.empty() && m_bUseDefOnEmpty)
			{
				*m_pDouble = m_Default.dbl;
				return true;
			}
			else
				return StringUtil::ToDouble(textVal, *m_pDouble);
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
		wxTextCtrl* pTextControl = (wxTextCtrl*)m_validatorWindow;
		if (m_pShort)
		{
			wxString str;
			str.Printf(wxT("%hd"), *m_pShort);
			pTextControl->ChangeValue(str);
			return true;
		}
		else if (m_pDouble)
		{
			pTextControl->ChangeValue(ARBDouble::ToString(*m_pDouble, m_Prec));
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


bool CGenericValidator::Validate(wxWindow* parent)
{
	if (!m_validatorWindow->IsEnabled())
		return true;

	wxString errormsg(m_ErrMsg);
	wxTextCtrl* pTextControl = NULL;
	bool ok = true;
	if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
	{
		pTextControl = (wxTextCtrl*)m_validatorWindow;
		if (pTextControl->IsEditable())
		{
			wxString textVal = pTextControl->GetValue();
			if (m_pShort)
			{
				long val;
				if (textVal.empty() && m_bUseDefOnEmpty)
				{
					wxString str;
					str.Printf(wxT("%hd"), m_Default.s);
					pTextControl->ChangeValue(str);
				}
				else if (!StringUtil::ToLong(textVal, val))
				{
					ok = false;
					if (errormsg.empty())
						errormsg = _("IDS_NEED_NUMBER");
				}
			}
			else if (m_pDouble)
			{
				double dbl;
				if (textVal.empty() && m_bUseDefOnEmpty)
				{
					pTextControl->ChangeValue(ARBDouble::ToString(m_Default.dbl, m_Prec));
				}
				else if (!StringUtil::ToDouble(textVal, dbl))
				{
					ok = false;
					if (errormsg.empty())
						errormsg = _("IDS_NEED_NUMBER");
				}
			}
		}
	}
	// Ignore date control, they're ok.

	if (!ok)
	{
		if (errormsg.empty())
			errormsg = _("IDS_NEED_VALUE");

		if (pTextControl)
			pTextControl->SelectAll();
		m_validatorWindow->SetFocus();
		wxMessageBox(errormsg, _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
	}
	return ok;
}

/////////////////////////////////////////////////////////////////////////////

CTrimValidator::CTrimValidator(
		wxString* valPtr,
		long trimStyle,
		wxChar const* errMsg)
	: wxGenericValidator(valPtr)
	, m_TrimStyle(trimStyle)
	, m_ErrMsg()
{
	if (errMsg)
		m_ErrMsg = errMsg;
}


CTrimValidator::CTrimValidator(CTrimValidator const& rhs)
	: wxGenericValidator(rhs)
	, m_TrimStyle(rhs.m_TrimStyle)
	, m_ErrMsg(rhs.m_ErrMsg)
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
	if (textCtrl && !textCtrl->IsEditable())
		return true;
	wxComboBox* comboCtrl = wxDynamicCast(m_validatorWindow, wxComboBox);
	if (comboCtrl && !comboCtrl->IsEditable())
		return true;
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
		{
#if wxCHECK_VERSION(2, 9, 3)
			comboCtrl->ChangeValue(val);
#else
			comboCtrl->SetValue(val);
#endif
		}
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
