#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Interface for plug-in to update progress.
 * @author David Connet
 *
 * Revision History
 * 2007-08-19 Created
 */

namespace dconSoft
{

class IProgressMeter
{
public:
	virtual ~IProgressMeter() = default;

	/**
	 * Update a message.
	 * (default is none)
	 */
	virtual void SetMessage(wxString const& message) = 0;

	/**
	 * Set the range of the progress meter. If this is done in the middle
	 * of a progress, it will reset the tic to the beginning.
	 * (default is 0 to 10)
	 */
	virtual void SetRange(int inRange) = 0;

	/**
	 * Set the step size. (default is 1)
	 */
	virtual void SetStep(int inStep) = 0;

	/**
	 * Advance one step.
	 */
	virtual void StepIt() = 0;

	/**
	 * Set the actual position.
	 */
	virtual void SetPos(int pos) = 0;

	/**
	 * Get the current position.
	 */
	virtual int GetPos() = 0;

	/**
	 * User canceled
	 */
	virtual bool HasCanceled() const = 0;
};

} // namespace dconSoft
