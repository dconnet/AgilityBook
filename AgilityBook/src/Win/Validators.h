#pragma once

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
 * @li 2009-02-15 DRC Created
 */

#include "ARBTypes.h"
#include <wx/valgen.h>
#include <wx/valtext.h>
class ARBDate;


/// wxGenericValidator doesn't do shorts and doubles
// Would have preferred to inherit from wxGenericValidator, but there's
// no ctor I can use (it has no default one).
class CGenericValidator : public wxValidator
{
DECLARE_CLASS(CGenericValidator)
public:
	CGenericValidator(short* val);
	CGenericValidator(double* val, int inPrec = 2);
	CGenericValidator(ARBDate* val);
	CGenericValidator(CGenericValidator const& rhs);

	virtual wxObject *Clone() const {return new CGenericValidator(*this);}

	virtual bool TransferFromWindow();
	virtual bool TransferToWindow();
	virtual bool Validate(wxWindow* parent)	{return true;}

private:
	short* m_pShort;
	double* m_pDouble;
	int m_Prec;
	ARBDate* m_pDate;
};


#define TRIMVALIDATOR_NONE			0x0000
#define TRIMVALIDATOR_TRIM_LEFT		0x0001
#define TRIMVALIDATOR_TRIM_RIGHT	0x0002
#define TRIMVALIDATOR_TRIM_BOTH		(TRIMVALIDATOR_TRIM_LEFT | TRIMVALIDATOR_TRIM_RIGHT)
#define TRIMVALIDATOR_NONEMPTY		0x0004


class CTrimValidator : public wxGenericValidator
{
DECLARE_CLASS(CTrimValidator)
public:
	CTrimValidator(
			wxString* valPtr = NULL,
			long trimStyle = TRIMVALIDATOR_TRIM_BOTH | TRIMVALIDATOR_NONEMPTY);
	CTrimValidator(
			wxString* valPtr,
			wxString const& errMsg); // Message to use when validation fails
	CTrimValidator(CTrimValidator const& rhs);

	virtual wxObject *Clone() const {return new CTrimValidator(*this);}
	virtual bool TransferFromWindow();
	virtual bool Validate(wxWindow* parent);

protected:
	long m_TrimStyle;
	wxString m_ErrMsg;
};


/// For use with CQualifyingComboBox
class CQualifyingValidator : public wxValidator
{
DECLARE_CLASS(CQualifyingValidator)
public:
	CQualifyingValidator(ARB_Q* valPtr = NULL, bool bAllowNoSel = false);
	CQualifyingValidator(CQualifyingValidator const& rhs);

	virtual wxObject *Clone() const {return new CQualifyingValidator(*this);}

	virtual bool TransferFromWindow();
	virtual bool TransferToWindow();
	virtual bool Validate(wxWindow* parent);

private:
	ARB_Q* m_pQ;
	bool m_bAllowNoSel;
};
