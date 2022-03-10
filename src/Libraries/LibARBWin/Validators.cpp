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
 * 2018-10-11 Moved to Win LibARBWin
 * 2012-10-26 Changed ARBDate::GetTime to avoid time_t when possible.
 * 2012-08-13 Moved CQualifyingValidator to separate file.
 * 2012-03-15 Missed upper casing the Validate() method.
 * 2012-03-01 Change time to upper case before parsing.
 * 2012-01-02 Change validator to support default value on empty field.
 * 2011-12-30 Use ChangeValue on textctrl to prevent change message.
 * 2010-12-07 Added wxDateTime support.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-15 Created
 */

#include "stdafx.h"
#include "LibARBWin/Validators.h"

#include "ARBCommon/ARBDate.h"
#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"
#include <wx/datectrl.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

namespace
{
constexpr wchar_t k_TimeFormatHMS[] = L"%I:%M:%S %p";
constexpr wchar_t k_TimeFormatHM[] = L"%I:%M %p";
} // namespace

wxIMPLEMENT_CLASS(CGenericValidator, wxValidator)
wxIMPLEMENT_CLASS(CTrimValidator, wxGenericValidator)

/////////////////////////////////////////////////////////////////////////////

CGenericValidator::CGenericValidator(
	unsigned short* val,
	unsigned short defVal,
	bool bUseDefOnEmpty,
	wxChar const* errMsg)
	: m_pUShort(val)
	, m_pShort(nullptr)
	, m_pLong(nullptr)
	, m_pDouble(nullptr)
	, m_strip(ARBDouble::ZeroStrip::Compatible)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(bUseDefOnEmpty)
	, m_pDate(nullptr)
	, m_pTime(nullptr)
	, m_showTimeSeconds(true)
	, m_ErrMsg()
{
	m_Default.us = defVal;
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(short* val, short defVal, bool bUseDefOnEmpty, wxChar const* errMsg)
	: m_pUShort(nullptr)
	, m_pShort(val)
	, m_pLong(nullptr)
	, m_pDouble(nullptr)
	, m_strip(ARBDouble::ZeroStrip::Compatible)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(bUseDefOnEmpty)
	, m_pDate(nullptr)
	, m_pTime(nullptr)
	, m_showTimeSeconds(true)
	, m_ErrMsg()
{
	m_Default.s = defVal;
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(long* val, long defVal, bool bUseDefOnEmpty, wxChar const* errMsg)
	: m_pUShort(nullptr)
	, m_pShort(nullptr)
	, m_pLong(val)
	, m_pDouble(nullptr)
	, m_strip(ARBDouble::ZeroStrip::Compatible)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(bUseDefOnEmpty)
	, m_pDate(nullptr)
	, m_pTime(nullptr)
	, m_showTimeSeconds(true)
	, m_ErrMsg()
{
	m_Default.l = defVal;
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(double* val, int inPrec, double defVal, bool bUseDefOnEmpty, wxChar const* errMsg)
	: CGenericValidator(val, ARBDouble::ZeroStrip::Compatible, inPrec, defVal, bUseDefOnEmpty, errMsg)
{
}


CGenericValidator::CGenericValidator(
	double* val,
	ARBDouble::ZeroStrip strip,
	int inPrec,
	double defVal,
	bool bUseDefOnEmpty,
	wxChar const* errMsg)
	: m_pUShort(nullptr)
	, m_pShort(nullptr)
	, m_pLong(nullptr)
	, m_pDouble(val)
	, m_strip(strip)
	, m_Prec(inPrec)
	, m_Default()
	, m_bUseDefOnEmpty(bUseDefOnEmpty)
	, m_pDate(nullptr)
	, m_pTime(nullptr)
	, m_showTimeSeconds(true)
	, m_ErrMsg()
{
	m_Default.dbl = defVal;
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(ARBDate* val, wxChar const* errMsg)
	: m_pUShort(nullptr)
	, m_pShort(nullptr)
	, m_pLong(nullptr)
	, m_pDouble(nullptr)
	, m_strip(ARBDouble::ZeroStrip::Compatible)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(false)
	, m_pDate(val)
	, m_pTime(nullptr)
	, m_showTimeSeconds(true)
	, m_ErrMsg()
{
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(wxDateTime* val, wxChar const* errMsg)
	: m_pUShort(nullptr)
	, m_pShort(nullptr)
	, m_pLong(nullptr)
	, m_pDouble(nullptr)
	, m_strip(ARBDouble::ZeroStrip::Compatible)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(false)
	, m_pDate(nullptr)
	, m_pTime(val)
	, m_showTimeSeconds(true)
	, m_ErrMsg()
{
	if (errMsg)
		m_ErrMsg = errMsg;
}


CGenericValidator::CGenericValidator(wxDateTime* val, bool showSeconds, wxChar const* errMsg)
	: m_pUShort(nullptr)
	, m_pShort(nullptr)
	, m_pLong(nullptr)
	, m_pDouble(nullptr)
	, m_strip(ARBDouble::ZeroStrip::Compatible)
	, m_Prec(0)
	, m_Default()
	, m_bUseDefOnEmpty(false)
	, m_pDate(nullptr)
	, m_pTime(val)
	, m_showTimeSeconds(showSeconds)
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
	, m_strip(rhs.m_strip)
	, m_Prec(rhs.m_Prec)
	, m_Default(rhs.m_Default)
	, m_bUseDefOnEmpty(rhs.m_bUseDefOnEmpty)
	, m_pDate(rhs.m_pDate)
	, m_pTime(rhs.m_pTime)
	, m_showTimeSeconds(rhs.m_showTimeSeconds)
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
	m_strip = val.m_strip;
	m_Prec = val.m_Prec;
	m_Default = val.m_Default;
	m_bUseDefOnEmpty = val.m_bUseDefOnEmpty;
	m_pDate = val.m_pDate;
	m_pTime = val.m_pTime;
	m_showTimeSeconds = val.m_showTimeSeconds;
	m_ErrMsg = val.m_ErrMsg;
	return true;
}


bool CGenericValidator::TransferFromWindow()
{
	// Following the example of wxGenericValidator
	if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
	{
		wxTextCtrl* pTextControl = dynamic_cast<wxTextCtrl*>(m_validatorWindow);
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
			if (m_showTimeSeconds)
				return m_pTime->ParseFormat(textVal, k_TimeFormatHMS);
			else
				return m_pTime->ParseFormat(textVal, k_TimeFormatHM);
		}
	}
	else if (m_validatorWindow->IsKindOf(CLASSINFO(wxDatePickerCtrlBase)))
	{
		wxDatePickerCtrlBase* pControl = dynamic_cast<wxDatePickerCtrlBase*>(m_validatorWindow);
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
		wxTextCtrl* pTextControl = dynamic_cast<wxTextCtrl*>(m_validatorWindow);
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
			pTextControl->ChangeValue(StringUtil::stringWX(ARBDouble::ToString(*m_pDouble, m_Prec, true, m_strip)));
			return true;
		}
		else if (m_pTime)
		{
			if (m_showTimeSeconds)
				pTextControl->ChangeValue(m_pTime->Format(k_TimeFormatHMS));
			else
				pTextControl->ChangeValue(m_pTime->Format(k_TimeFormatHM));
			return true;
		}
	}
	else if (m_validatorWindow->IsKindOf(CLASSINFO(wxDatePickerCtrlBase)))
	{
		wxDatePickerCtrlBase* pControl = dynamic_cast<wxDatePickerCtrlBase*>(m_validatorWindow);
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
	wxTextCtrl* pTextControl = nullptr;
	bool ok = true;
	if (m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
	{
		pTextControl = dynamic_cast<wxTextCtrl*>(m_validatorWindow);
		if (pTextControl->IsEditable())
		{
			wxString textVal = pTextControl->GetValue();
			if (m_pUShort)
			{
				long val = 0;
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
				long val = 0;
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
				double dbl = 0.0;
				if (textVal.empty() && m_bUseDefOnEmpty)
				{
					pTextControl->ChangeValue(
						StringUtil::stringWX(ARBDouble::ToString(m_Default.dbl, m_Prec, true, m_strip)));
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
				if (m_showTimeSeconds)
					ok = date.ParseFormat(textVal, k_TimeFormatHMS);
				else
					ok = date.ParseFormat(textVal, k_TimeFormatHM);
				if (!ok)
				{
					if (errormsg.empty())
						errormsg = _("Please enter a valid time");
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

CTrimValidator::CTrimValidator(wxString* valPtr, long trimStyle, wxChar const* errMsg)
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
			comboCtrl->ChangeValue(val);
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
