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
 * @li 2012-10-26 DRC Changed ARBDate::GetTime to avoid time_t when possible.
 * @li 2012-08-13 DRC Moved CQualifyingValidator to separate file.
 * @li 2012-03-15 DRC Missed upper casing the Validate() method.
 * @li 2012-03-01 DRC Change time to upper case before parsing.
 * @li 2012-01-02 DRC Change validator to support default value on empty field.
 * @li 2011-12-30 DRC Use ChangeValue on textctrl to prevent change message.
 * @li 2010-12-07 DRC Added wxDateTime support.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-15 DRC Created
 */

#include "stdafx.h"
#include "Validators.h"

#include "ARB/ARBDate.h"
#include "ARB/ARBTypes.h"
#include "ARB/StringUtil.h"
#include <wx/datectrl.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_CLASS(CGenericValidator, wxValidator)
IMPLEMENT_CLASS(CTrimValidator, wxGenericValidator)

/////////////////////////////////////////////////////////////////////////////

CGenericValidator::CGenericValidator(
		unsigned short* val,
		unsigned short defVal,
		bool bUseDefOnEmpty,
		wxChar const* errMsg)
	: m_pUShort(val)
	, m_pShort(NULL)
	, m_pLong(NULL)
	, m_pDouble(NULL)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(bUseDefOnEmpty)
	, m_pDate(NULL)
	, m_pTime(NULL)
	, m_ErrMsg()
{
	m_Default.us = defVal;
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(
		short* val,
		short defVal,
		bool bUseDefOnEmpty,
		wxChar const* errMsg)
	: m_pUShort(NULL)
	, m_pShort(val)
	, m_pLong(NULL)
	, m_pDouble(NULL)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(bUseDefOnEmpty)
	, m_pDate(NULL)
	, m_pTime(NULL)
	, m_ErrMsg()
{
	m_Default.s = defVal;
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(
		long* val,
		long defVal,
		bool bUseDefOnEmpty,
		wxChar const* errMsg)
	: m_pUShort(NULL)
	, m_pShort(NULL)
	, m_pLong(val)
	, m_pDouble(NULL)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(bUseDefOnEmpty)
	, m_pDate(NULL)
	, m_pTime(NULL)
	, m_ErrMsg()
{
	m_Default.l = defVal;
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(
		double* val,
		int inPrec,
		double defVal,
		bool bUseDefOnEmpty,
		wxChar const* errMsg)
	: m_pUShort(NULL)
	, m_pShort(NULL)
	, m_pLong(NULL)
	, m_pDouble(val)
	, m_Prec(inPrec)
	, m_Default()
	, m_bUseDefOnEmpty(bUseDefOnEmpty)
	, m_pDate(NULL)
	, m_pTime(NULL)
	, m_ErrMsg()
{
	m_Default.dbl = defVal;
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(
		ARBDate* val,
		wxChar const* errMsg)
	: m_pUShort(NULL)
	, m_pShort(NULL)
	, m_pLong(NULL)
	, m_pDouble(NULL)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(false)
	, m_pDate(val)
	, m_pTime(NULL)
	, m_ErrMsg()
{
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(
		wxDateTime* val,
		wxChar const* errMsg)
	: m_pUShort(NULL)
	, m_pShort(NULL)
	, m_pLong(NULL)
	, m_pDouble(NULL)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(false)
	, m_pDate(NULL)
	, m_pTime(val)
	, m_ErrMsg()
{
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(CGenericValidator const& rhs)
	: m_pUShort(rhs.m_pUShort)
	, m_pShort(rhs.m_pShort)
	, m_pLong(rhs.m_pLong)
	, m_pDouble(rhs.m_pDouble)
	, m_Prec(rhs.m_Prec)
	, m_Default(rhs.m_Default)
	, m_bUseDefOnEmpty(rhs.m_bUseDefOnEmpty)
	, m_pDate(rhs.m_pDate)
	, m_pTime(rhs.m_pTime)
	, m_ErrMsg(rhs.m_ErrMsg)
{
	Copy(rhs);
}


bool CGenericValidator::Copy(CGenericValidator const& val)
{
	wxValidator::Copy(val);
	m_pUShort = val.m_pUShort;
	m_pShort = val.m_pShort;
	m_pLong = val.m_pLong;
	m_pDouble = val.m_pDouble;
	m_Prec = val.m_Prec;
	m_Default = val.m_Default;
	m_bUseDefOnEmpty = val.m_bUseDefOnEmpty;
	m_pDate = val.m_pDate;
	m_pTime = val.m_pTime;
	m_ErrMsg = val.m_ErrMsg;
	return true;
}

static const wxChar* s_TimeFormat = L"%I:%M:%S %p";

bool CGenericValidator::TransferFromWindow()
{
	// Following the example of wxGenericValidator
	if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
	{
		wxTextCtrl* pTextControl = (wxTextCtrl*)m_validatorWindow;
		wxString textVal = pTextControl->GetValue();
		if (m_pUShort)
		{
			if (textVal.empty() && m_bUseDefOnEmpty)
			{
				*m_pUShort = m_Default.us;
			}
			else
			{
				long val;
				if (!StringUtil::ToLong(StringUtil::stringW(textVal), val))
					return false;
				*m_pUShort = static_cast<unsigned short>(val);
			}
			return true;
		}
		else if (m_pShort)
		{
			if (textVal.empty() && m_bUseDefOnEmpty)
			{
				*m_pShort = m_Default.s;
			}
			else
			{
				long val;
				if (!StringUtil::ToLong(StringUtil::stringW(textVal), val))
					return false;
				*m_pShort = static_cast<short>(val);
			}
			return true;
		}
		else if (m_pLong)
		{
			if (textVal.empty() && m_bUseDefOnEmpty)
			{
				*m_pLong = m_Default.l;
				return true;
			}
			return StringUtil::ToLong(StringUtil::stringW(textVal), *m_pLong);
		}
		else if (m_pDouble)
		{
			if (textVal.empty() && m_bUseDefOnEmpty)
			{
				*m_pDouble = m_Default.dbl;
				return true;
			}
			return StringUtil::ToDouble(StringUtil::stringW(textVal), *m_pDouble);
		}
		else if (m_pTime)
		{
			textVal.MakeUpper();
#if wxCHECK_VERSION(2, 9, 5)
			return m_pTime->ParseFormat(textVal, s_TimeFormat);
#else
			return NULL != m_pTime->ParseFormat(textVal, s_TimeFormat);
#endif
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
		else if (m_pTime)
		{
			*m_pTime = pControl->GetValue();
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
		if (m_pUShort)
		{
			wxString str;
			str.Printf(L"%hu", *m_pUShort);
			pTextControl->ChangeValue(str);
			return true;
		}
		else if (m_pShort)
		{
			wxString str;
			str.Printf(L"%hd", *m_pShort);
			pTextControl->ChangeValue(str);
			return true;
		}
		else if (m_pLong)
		{
			wxString str;
			str.Printf(L"%ld", *m_pLong);
			pTextControl->ChangeValue(str);
			return true;
		}
		else if (m_pDouble)
		{
			pTextControl->ChangeValue(StringUtil::stringWX(ARBDouble::ToString(*m_pDouble, m_Prec)));
			return true;
		}
		else if (m_pTime)
		{
			pTextControl->ChangeValue(m_pTime->Format(s_TimeFormat));
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
				wxDateTime date;
				if (m_pDate->GetDate(date))
					pControl->SetValue(date);
				return true;
			}
			else
			{
				if (pControl->HasFlag(wxDP_ALLOWNONE))
				{
					wxDateTime date;
					pControl->SetValue(date);
				}
				else
				{
					wxDateTime date;
					if (ARBDate::Today().GetDate(date))
						pControl->SetValue(date);
				}
				return true;
			}
		}
		else if (m_pTime)
		{
			pControl->SetValue(*m_pTime);
			return true;
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
			if (m_pUShort)
			{
				long val;
				if (textVal.empty() && m_bUseDefOnEmpty)
				{
					wxString str;
					str.Printf(L"%hu", m_Default.us);
					pTextControl->ChangeValue(str);
				}
				else if (!StringUtil::ToLong(StringUtil::stringW(textVal), val))
				{
					ok = false;
					if (errormsg.empty())
						errormsg = _("Please enter a valid number");
				}
			}
			if (m_pShort || m_pLong)
			{
				long val;
				if (textVal.empty() && m_bUseDefOnEmpty)
				{
					wxString str;
					if (m_pShort)
						str.Printf(L"%hd", m_Default.s);
					else
						str.Printf(L"%ld", m_Default.l);
					pTextControl->ChangeValue(str);
				}
				else if (!StringUtil::ToLong(StringUtil::stringW(textVal), val))
				{
					ok = false;
					if (errormsg.empty())
						errormsg = _("Please enter a valid number");
				}
			}
			else if (m_pDouble)
			{
				double dbl;
				if (textVal.empty() && m_bUseDefOnEmpty)
				{
					pTextControl->ChangeValue(StringUtil::stringWX(ARBDouble::ToString(m_Default.dbl, m_Prec)));
				}
				else if (!StringUtil::ToDouble(StringUtil::stringW(textVal), dbl))
				{
					ok = false;
					if (errormsg.empty())
						errormsg = _("Please enter a valid number");
				}
			}
			else if (m_pTime)
			{
				textVal.MakeUpper();
				wxDateTime date(*m_pTime);
				if (!date.ParseFormat(textVal, s_TimeFormat))
				{
					ok = false;
					if (errormsg.empty())
						errormsg = _("Please enter a valid number");
				}
			}
		}
	}
	// Ignore date control, they're ok.

	if (!ok)
	{
		if (errormsg.empty())
			errormsg = _("Please enter a value");

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
#if wxCHECK_VERSION(2, 9, 5)
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
			errormsg = _("Please enter a value");
	}

	if (!ok)
	{
		m_validatorWindow->SetFocus();
		wxMessageBox(errormsg, _("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
	}
	return ok;
}
