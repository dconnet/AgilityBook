#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Special comboboxes
 * @author David Connet
 *
 * Revision History
 * 2013-04-15 Separated from ComboBoxes.h
 */

#include "ARB/ARBTypes2.h"
#include "ARB/ARB_Q.h"


namespace dconSoft
{


/// Combobox for Qs
class CQualifyingComboBox : public wxChoice
{
	DECLARE_CLASS(CQualifyingComboBox)
public:
	CQualifyingComboBox(
		wxWindow* parent,
		ARB::ARBDogReferenceRunPtr const& inRefRun,
		wxValidator const& validator = wxDefaultValidator);
	CQualifyingComboBox(
		wxWindow* parent,
		ARB::ARBDogRunPtr const& inRun,
		wxValidator const& validator = wxDefaultValidator);
	CQualifyingComboBox(
		wxWindow* parent,
		ARB::ARBDogRunPtr const& inRun,
		ARB::ARBConfigScoringPtr const& inScoring,
		wxValidator const& validator = wxDefaultValidator);

	void ResetContent(ARB::ARBConfigScoringPtr const& inScoring);
	ARB::ARB_Q GetQ(int index) const;

private:
	ARB::ARBDogReferenceRunPtr m_refRun;
	ARB::ARBDogRunPtr m_Run;
};

} // namespace dconSoft
