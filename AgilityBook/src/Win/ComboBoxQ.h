#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Special comboboxes
 * @author David Connet
 *
 * Revision History
 * @li 2013-04-15 DRC Separated from ComboBoxes.h
 */

#include "ARB/ARB_Q.h"
#include "ARB/ARBTypes2.h"


/// Combobox for Qs
class CQualifyingComboBox : public wxChoice
{
DECLARE_CLASS(CQualifyingComboBox)
public:
	CQualifyingComboBox(
			wxWindow* parent,
			ARBDogReferenceRunPtr refRun,
			wxValidator const& validator = wxDefaultValidator);
	CQualifyingComboBox(
			wxWindow* parent,
			ARBDogRunPtr run,
			wxValidator const& validator = wxDefaultValidator);
	CQualifyingComboBox(
			wxWindow* parent,
			ARBDogRunPtr run,
			ARBConfigScoringPtr scoring,
			wxValidator const& validator = wxDefaultValidator);

	void ResetContent(ARBConfigScoringPtr scoring);
	ARB_Q GetQ(int index) const;

private:
	ARBDogReferenceRunPtr m_refRun;
	ARBDogRunPtr m_Run;
};
