/*
 * Copyright © 2008 David Connet. All Rights Reserved.
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
 * @brief Test ARBConfigAction class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigAction.h"
#include "Element.h"

#if 0
class IConfigActionCallback
{
	IConfigActionCallback();
	virtual void PreDelete(tstring const& inMsg) = 0;
	virtual void PostDelete(tstring const& inMsg) const = 0;
	virtual bool CanContinue() const
};

class ARBConfigAction : public ARBBase
{
	virtual ARBConfigActionPtr Clone() const = 0;
	virtual tstring GetGenericName() const
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const = 0;
};

class ARBConfigActionDeleteCalPlugin : public ARBConfigAction
{ 
	static ARBConfigActionPtr New(
			tstring const& inName);
	virtual ARBConfigActionPtr Clone() const;
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionRenameOtherPoints : public ARBConfigAction
{ 
	static ARBConfigActionPtr New(
			tstring const& inOldName,
			tstring const& inNewName);
	virtual ARBConfigActionPtr Clone() const;
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionDeleteOtherPoints : public ARBConfigAction
{ 
	static ARBConfigActionPtr New(
			tstring const& inName);
	virtual ARBConfigActionPtr Clone() const;
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionRenameVenue : public ARBConfigAction
{
	static ARBConfigActionPtr New(
			tstring const& inOldName,
			tstring const& inNewName);
	virtual ARBConfigActionPtr Clone() const;
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionDeleteVenue : public ARBConfigAction
{
	static ARBConfigActionPtr New(
			tstring const& inName);
	virtual ARBConfigActionPtr Clone() const;
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionRenameMultiQ : public ARBConfigAction
{
	virtual ARBConfigActionPtr Clone() const;
	static ARBConfigActionPtr New(
			tstring const& inVenue,
			tstring const& inOldName,
			tstring const& inNewName);
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionDeleteMultiQ : public ARBConfigAction
{
	virtual ARBConfigActionPtr Clone() const;
	static ARBConfigActionPtr New(
			tstring const& inVenue,
			tstring const& inName);
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionRenameDivision : public ARBConfigAction
{
	virtual ARBConfigActionPtr Clone() const;
	static ARBConfigActionPtr New(
			tstring const& inVenue,
			tstring const& inOldName,
			tstring const& inNewName);
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionDeleteDivision : public ARBConfigAction
{
	virtual ARBConfigActionPtr Clone() const;
	static ARBConfigActionPtr New(
			tstring const& inVenue,
			tstring const& inName);
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionRenameLevel : public ARBConfigAction
{
	virtual ARBConfigActionPtr Clone() const;
	static ARBConfigActionPtr NewLevel(
			tstring const& inVenue,
			tstring const& inDiv,
			tstring const& inOldName,
			tstring const& inNewName);
	static ARBConfigActionPtr NewSubLevel(
			tstring const& inVenue,
			tstring const& inDiv,
			tstring const& inLevel,
			tstring const& inOldName,
			tstring const& inNewName);
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionDeleteLevel : public ARBConfigAction
{
	virtual ARBConfigActionPtr Clone() const;
	static ARBConfigActionPtr NewLevel(
			tstring const& inVenue,
			tstring const& inDiv,
			tstring const& inName);
	static ARBConfigActionPtr NewSubLevel(
			tstring const& inVenue,
			tstring const& inDiv,
			tstring const& inLevel,
			tstring const& inName);
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionRenameTitle : public ARBConfigAction
{
	virtual ARBConfigActionPtr Clone() const;
	static ARBConfigActionPtr New(
			tstring const& inVenue,
			tstring const& inOldName,
			tstring const& inNewName);
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionDeleteTitle : public ARBConfigAction
{
	virtual ARBConfigActionPtr Clone() const;
	static ARBConfigActionPtr New(
			tstring const& inVenue,
			tstring const& inDiv,
			tstring const& inOldName,
			tstring const& inNewName);
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionRenameEvent : public ARBConfigAction
{
	virtual ARBConfigActionPtr Clone() const;
	static ARBConfigActionPtr New(
			tstring const& inVenue,
			tstring const& inOldName,
			tstring const& inNewName);
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionDeleteEvent : public ARBConfigAction
{
	virtual ARBConfigActionPtr Clone() const;
	static ARBConfigActionPtr New(
			tstring const& inVenue,
			tstring const& inName);
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};

class ARBConfigActionList : public ARBVector<ARBConfigActionPtr>
{
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	int Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};
#endif

#pragma message PRAGMA_MESSAGE("TODO: Implement tests")
