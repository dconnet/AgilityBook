#pragma once

/*
 * Copyright (c) 2007-2009 David Connet. All Rights Reserved.
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
 * @brief Interface for plug-in to update progress.
 * @author David Connet
 *
 * Revision History
 * @li 2007-08-19 DRC Created
 */

class IProgressMeter
{
public:
	/**
	 * Update a message.
	 * (default is none)
	 */
	virtual void SetMessage(char const* pMessage) = 0;

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
