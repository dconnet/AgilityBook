#pragma once

/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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
 * @brief ARBConfigAction class.
 * @author David Connet
 *
 * Revision History
 * @li 2006-12-03 DRC Complete re-write of action class.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-10-26 DRC Added optional 'Div' to DeleteTitle.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-21 DRC Created
 */

#include "ARBBase.h"
#include "ARBTypes.h"

class IConfigActionCallback
{
public:
	IConfigActionCallback();
	virtual ~IConfigActionCallback();

	/**
	 * Callback made during Apply() before data loss occurs.
	 * Gives the user a chance to abort by setting m_bContinue to false.
	 * @param inMsg Message generated in Apply about what is being deleted.
	 */
	virtual void PreDelete(ARBString const& inMsg) = 0;

	/**
	 * Callback made during Update after data loss. This can occur when
	 * a scoring method changes that invalidates an existing run. This
	 * will occur after all PreDelete/Apply processing and after the new
	 * config has been merged.
	 * @param inMsg Message generated in Apply about what is being deleted.
	 */
	virtual void PostDelete(ARBString const& inMsg) const = 0;

	/**
	 * Should we continue processing?
	 */
	virtual bool CanContinue() const
	{
		return m_bContinue;
	}

protected:
	bool m_bContinue;
};

/**
 * Special class that performs actions during ARBConfig::Update.
 *
 * During the update process, all actions are run in order, operating on the
 * existing configuration and data. The existing configuration is then updated.
 */
class ARBConfigAction : public ARBBase
{
protected:
	ARBConfigAction();

public:
	virtual ~ARBConfigAction();
	virtual ARBConfigActionPtr Clone() const = 0;

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual ARBString GetGenericName() const
	{
		return _T("");
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 * @note There are no strings to search in this object.
	 */
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const
	{
		return 0;
	}

	/**
	 * Apply the action.
	 * @param ioConfig Existing Configuration.
	 * @param ioDogs Existing Dogs.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @param ioCallBack Callback object to allow cancelling.
	 * @return Changes occurred.
	 */
	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const = 0;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameOtherPoints : public ARBConfigAction
{ 
protected:
	ARBConfigActionRenameOtherPoints(
			ARBString const& inOldName,
			ARBString const& inNewName);
	ARBConfigActionRenameOtherPoints(ARBConfigActionRenameOtherPoints const& rhs);

public:
	static ARBConfigActionPtr New(
			ARBString const& inOldName,
			ARBString const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	ARBString m_OldName;
	ARBString m_NewName;
};

class ARBConfigActionDeleteOtherPoints : public ARBConfigAction
{ 
protected:
	ARBConfigActionDeleteOtherPoints(
			ARBString const& inName);
	ARBConfigActionDeleteOtherPoints(ARBConfigActionDeleteOtherPoints const& rhs);

public:
	static ARBConfigActionPtr New(
			ARBString const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameVenue : public ARBConfigAction
{
protected:
	ARBConfigActionRenameVenue(
			ARBString const& inOldName,
			ARBString const& inNewName);
	ARBConfigActionRenameVenue(ARBConfigActionRenameVenue const& rhs);

public:
	static ARBConfigActionPtr New(
			ARBString const& inOldName,
			ARBString const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

private:
	ARBString m_OldName;
	ARBString m_NewName;
};

class ARBConfigActionDeleteVenue : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteVenue(
			ARBString const& inName);
	ARBConfigActionDeleteVenue(ARBConfigActionDeleteVenue const& rhs);

public:
	static ARBConfigActionPtr New(
			ARBString const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

private:
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameMultiQ : public ARBConfigAction
{
protected:
	ARBConfigActionRenameMultiQ(
			ARBString const& inVenue,
			ARBString const& inOldName,
			ARBString const& inNewName);
	ARBConfigActionRenameMultiQ(ARBConfigActionRenameMultiQ const& rhs);

public:
	virtual ARBConfigActionPtr Clone() const;

	static ARBConfigActionPtr New(
			ARBString const& inVenue,
			ARBString const& inOldName,
			ARBString const& inNewName);

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	ARBString m_Venue;
	ARBString m_OldName;
	ARBString m_NewName;
};

class ARBConfigActionDeleteMultiQ : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteMultiQ(
			ARBString const& inVenue,
			ARBString const& inName);
	ARBConfigActionDeleteMultiQ(ARBConfigActionDeleteMultiQ const& rhs);

public:
	virtual ARBConfigActionPtr Clone() const;

	static ARBConfigActionPtr New(
			ARBString const& inVenue,
			ARBString const& inName);

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	ARBString m_Venue;
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameDivision : public ARBConfigAction
{
protected:
	ARBConfigActionRenameDivision(
			ARBString const& inVenue,
			ARBString const& inOldName,
			ARBString const& inNewName);
	ARBConfigActionRenameDivision(ARBConfigActionRenameDivision const& rhs);

public:
	virtual ARBConfigActionPtr Clone() const;

	static ARBConfigActionPtr New(
			ARBString const& inVenue,
			ARBString const& inOldName,
			ARBString const& inNewName);

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	ARBString m_Venue;
	ARBString m_OldName;
	ARBString m_NewName;
};

class ARBConfigActionDeleteDivision : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteDivision(
			ARBString const& inVenue,
			ARBString const& inName);
	ARBConfigActionDeleteDivision(ARBConfigActionDeleteDivision const& rhs);

public:
	virtual ARBConfigActionPtr Clone() const;

	static ARBConfigActionPtr New(
			ARBString const& inVenue,
			ARBString const& inName);

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	ARBString m_Venue;
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameLevel : public ARBConfigAction
{
protected:
	ARBConfigActionRenameLevel(
			ARBString const& inVenue,
			ARBString const& inDiv,
			ARBString const& inLevel,
			ARBString const& inOldName,
			ARBString const& inNewName);
	ARBConfigActionRenameLevel(ARBConfigActionRenameLevel const& rhs);

public:
	virtual ARBConfigActionPtr Clone() const;

	static ARBConfigActionPtr NewLevel(
			ARBString const& inVenue,
			ARBString const& inDiv,
			ARBString const& inOldName,
			ARBString const& inNewName);

	static ARBConfigActionPtr NewSubLevel(
			ARBString const& inVenue,
			ARBString const& inDiv,
			ARBString const& inLevel,
			ARBString const& inOldName,
			ARBString const& inNewName);

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	ARBString m_Venue;
	ARBString m_Div;
	ARBString m_Level; ///< Only used on sublevels.
	ARBString m_OldName;
	ARBString m_NewName;
};

class ARBConfigActionDeleteLevel : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteLevel(
			ARBString const& inVenue,
			ARBString const& inDiv,
			ARBString const& inLevel,
			ARBString const& inName);
	ARBConfigActionDeleteLevel(ARBConfigActionDeleteLevel const& rhs);

public:
	virtual ARBConfigActionPtr Clone() const;

	static ARBConfigActionPtr NewLevel(
			ARBString const& inVenue,
			ARBString const& inDiv,
			ARBString const& inName);

	static ARBConfigActionPtr NewSubLevel(
			ARBString const& inVenue,
			ARBString const& inDiv,
			ARBString const& inLevel,
			ARBString const& inName);

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	ARBString m_Venue;
	ARBString m_Div;
	ARBString m_Level; ///< Only used on sublevels.
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameTitle : public ARBConfigAction
{
protected:
	ARBConfigActionRenameTitle(
			ARBString const& inVenue,
			ARBString const& inOldName,
			ARBString const& inNewName);
	ARBConfigActionRenameTitle(ARBConfigActionRenameTitle const& rhs);

public:
	virtual ARBConfigActionPtr Clone() const;

	static ARBConfigActionPtr New(
			ARBString const& inVenue,
			ARBString const& inOldName,
			ARBString const& inNewName);

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	ARBString m_Venue;
	ARBString m_OldName;
	ARBString m_NewName;
};

class ARBConfigActionDeleteTitle : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteTitle(
			ARBString const& inVenue,
			ARBString const& inDiv,
			ARBString const& inOldName,
			ARBString const& inNewName);
	ARBConfigActionDeleteTitle(ARBConfigActionDeleteTitle const& rhs);

public:
	virtual ARBConfigActionPtr Clone() const;

	static ARBConfigActionPtr New(
			ARBString const& inVenue,
			ARBString const& inDiv,
			ARBString const& inOldName,
			ARBString const& inNewName);

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	ARBString m_Venue;
	ARBString m_Div;
	ARBString m_OldName;
	ARBString m_NewName;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameEvent : public ARBConfigAction
{
protected:
	ARBConfigActionRenameEvent(
			ARBString const& inVenue,
			ARBString const& inOldName,
			ARBString const& inNewName);
	ARBConfigActionRenameEvent(ARBConfigActionRenameEvent const& rhs);

public:
	virtual ARBConfigActionPtr Clone() const;

	static ARBConfigActionPtr New(
			ARBString const& inVenue,
			ARBString const& inOldName,
			ARBString const& inNewName);

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	ARBString m_Venue;
	ARBString m_OldName;
	ARBString m_NewName;
};

class ARBConfigActionDeleteEvent : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteEvent(
			ARBString const& inVenue,
			ARBString const& inName);
	ARBConfigActionDeleteEvent(ARBConfigActionDeleteEvent const& rhs);

public:
	virtual ARBConfigActionPtr Clone() const;

	static ARBConfigActionPtr New(
			ARBString const& inVenue,
			ARBString const& inName);

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	ARBString m_Venue;
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigAction objects.
 */
class ARBConfigActionList : public ARBVector<ARBConfigActionPtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	int Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			ARBostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};
