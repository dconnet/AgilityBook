#pragma once

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

#include "ARBTypes.h"
#include <wx/valgen.h>
#include <wx/valtext.h>


/// wxGenericValidator doesn't do shorts and doubles
// Would have preferred to inherit from wxGenericValidator, but there's
// no ctor I can use (it has no default one).
class CGenericValidator : public wxValidator
{
DECLARE_CLASS(CGenericValidator)
public:
	CGenericValidator(short* val);
	CGenericValidator(double* val, int inPrec = 2);
	CGenericValidator(CGenericValidator const& rhs);

	virtual wxObject *Clone() const {return new CGenericValidator(*this);}

	virtual bool TransferFromWindow();
	virtual bool TransferToWindow();
	virtual bool Validate(wxWindow* parent)	{return true;}

private:
	short* m_pShort;
	double* m_pDouble;
	int m_Prec;
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
	CTrimValidator(CTrimValidator const& rhs);

	virtual wxObject *Clone() const {return new CTrimValidator(*this);}
	virtual bool TransferFromWindow();
	virtual bool Validate(wxWindow* parent);

protected:
	long m_TrimStyle;
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
