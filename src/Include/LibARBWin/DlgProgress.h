#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Progress dialog
 * @author David Connet
 *
 * This is simply an interface in case I want to push it down into the ARB
 * code at some point. If that happens, this interface declaration will move
 * and the factory method will remain behind.
 *
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2009-02-11 Ported to wxWidgets.
 * 2004-10-01 Created
 */

#include "LibwxARBWin.h"


class ARBWIN_API IDlgProgress
{
public:
	static IDlgProgress* CreateProgress(short nBars = 1, wxWindow* parent = nullptr);

	virtual ~IDlgProgress() = default;

	// Setup dialog.
	/// Set the caption of the dialog.
	virtual void SetCaption(std::wstring const& inCaption) = 0;
	/// Set a visible message.
	virtual void SetMessage(std::wstring const& inMessage) = 0;

	// Progress bar interface (these are thin wrappers on the progress bar)
	virtual void SetRange(short inBar, int inRange) = 0;
	virtual void SetStep(short inBar, int inStep) = 0;
	virtual void StepIt(short inBar) = 0;
	virtual void OffsetPos(short inBar, int inDelta) = 0;
	virtual void SetPos(short inBar, int inPos) = 0;
	virtual int GetPos(short inBar) = 0;
	// Returns state before trying to change state.
	virtual bool EnableCancel(bool bEnable = true) = 0;
	virtual bool HasCanceled() const = 0;

	/// Show/hide the dialog.
	virtual void ShowProgress(bool bShow = true) = 0;
	/// Force dialog to have focus
	virtual void SetForegroundWindow() = 0;
	/// Shut down (delete) the dialog.
	virtual void Dismiss() = 0;
};
