#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigAction class.
 * @author David Connet
 *
 * Revision History
 * 2013-01-11 Fix filters on configuration import.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-12-03 Complete re-write of action class.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-26 Added optional 'Div' to DeleteTitle.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-01-21 Created
 */

#include "ARB/ARBBase.h"
#include "ARB/ARBTypes2.h"


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
	virtual void PreDelete(std::wstring const& inMsg) = 0;

	/**
	 * Callback made during Update after data loss. This can occur when
	 * a scoring method changes that invalidates an existing run. This
	 * will occur after all PreDelete/Apply processing and after the new
	 * config has been merged.
	 * @param inMsg Message generated in Apply about what is being deleted.
	 */
	virtual void PostDelete(std::wstring const& inMsg) const = 0;

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
	ARBConfigAction(short configVersion);
	ARBConfigAction(ARBConfigAction const& rhs);

public:
	virtual ~ARBConfigAction();
	virtual ARBConfigActionPtr Clone() const = 0;

	/**
	 * Get the version of the config this action can be applied against.
	 * @return version, or 0 for all
	 */
	short GetVersion() const
	{
		return m_configVersion;
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual std::wstring GetGenericName() const
	{
		return std::wstring();
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 * @note There are no strings to search in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const
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
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const = 0;

	/**
	 * Allow the UI to update filter contents.
	 */
	virtual bool Update(
			ARBConfig const& inConfigCurrent,
			std::wstring& ioVenue,
			std::wstring& ioDivision,
			std::wstring& ioSubLevel) const
	{
		return false;
	}

protected:
	// Apply action up to this configuration. Basically, this is when the action
	// was added. Once we're past that configuration, there's no need to apply
	// this action again.
	short m_configVersion;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionDeleteCalPlugin : public ARBConfigAction
{ 
protected:
	ARBConfigActionDeleteCalPlugin(
			std::wstring const& inName);
	ARBConfigActionDeleteCalPlugin(ARBConfigActionDeleteCalPlugin const& rhs);

public:
	static ARBConfigActionPtr New(
			std::wstring const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	std::wstring m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameOtherPoints : public ARBConfigAction
{ 
protected:
	ARBConfigActionRenameOtherPoints(
			short configVersion,
			std::wstring const& inOldName,
			std::wstring const& inNewName);
	ARBConfigActionRenameOtherPoints(ARBConfigActionRenameOtherPoints const& rhs);

public:
	static ARBConfigActionPtr New(
			short configVersion,
			std::wstring const& inOldName,
			std::wstring const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	std::wstring m_OldName;
	std::wstring m_NewName;
};


class ARBConfigActionDeleteOtherPoints : public ARBConfigAction
{ 
protected:
	ARBConfigActionDeleteOtherPoints(
			short configVersion,
			std::wstring const& inName);
	ARBConfigActionDeleteOtherPoints(ARBConfigActionDeleteOtherPoints const& rhs);

public:
	static ARBConfigActionPtr New(
			short configVersion,
			std::wstring const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	std::wstring m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameVenue : public ARBConfigAction
{
protected:
	ARBConfigActionRenameVenue(
			short configVersion,
			std::wstring const& inOldName,
			std::wstring const& inNewName);
	ARBConfigActionRenameVenue(ARBConfigActionRenameVenue const& rhs);

public:
	static ARBConfigActionPtr New(
			short configVersion,
			std::wstring const& inOldName,
			std::wstring const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

	virtual bool Update(
			ARBConfig const& inConfigCurrent,
			std::wstring& ioVenue,
			std::wstring& ioDivision,
			std::wstring& ioSubLevel) const;

private:
	std::wstring m_OldName;
	std::wstring m_NewName;
};


class ARBConfigActionDeleteVenue : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteVenue(
			short configVersion,
			std::wstring const& inName);
	ARBConfigActionDeleteVenue(ARBConfigActionDeleteVenue const& rhs);

public:
	static ARBConfigActionPtr New(
			short configVersion,
			std::wstring const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

private:
	std::wstring m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameMultiQ : public ARBConfigAction
{
protected:
	ARBConfigActionRenameMultiQ(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName);
	ARBConfigActionRenameMultiQ(ARBConfigActionRenameMultiQ const& rhs);

public:
	static ARBConfigActionPtr New(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	std::wstring m_Venue;
	std::wstring m_OldName;
	std::wstring m_NewName;
};


class ARBConfigActionDeleteMultiQ : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteMultiQ(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inName);
	ARBConfigActionDeleteMultiQ(ARBConfigActionDeleteMultiQ const& rhs);

public:
	static ARBConfigActionPtr New(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	std::wstring m_Venue;
	std::wstring m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameDivision : public ARBConfigAction
{
protected:
	ARBConfigActionRenameDivision(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName);
	ARBConfigActionRenameDivision(ARBConfigActionRenameDivision const& rhs);

public:
	static ARBConfigActionPtr New(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

	virtual bool Update(
			ARBConfig const& inConfigCurrent,
			std::wstring& ioVenue,
			std::wstring& ioDivision,
			std::wstring& ioSubLevel) const;

protected:
	std::wstring m_Venue;
	std::wstring m_OldName;
	std::wstring m_NewName;
};


class ARBConfigActionDeleteDivision : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteDivision(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inName);
	ARBConfigActionDeleteDivision(ARBConfigActionDeleteDivision const& rhs);

public:
	static ARBConfigActionPtr New(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	std::wstring m_Venue;
	std::wstring m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameLevel : public ARBConfigAction
{
protected:
	ARBConfigActionRenameLevel(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inLevel,
			std::wstring const& inOldName,
			std::wstring const& inNewName);
	ARBConfigActionRenameLevel(ARBConfigActionRenameLevel const& rhs);

public:
	static ARBConfigActionPtr NewLevel(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inOldName,
			std::wstring const& inNewName);

	static ARBConfigActionPtr NewSubLevel(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inLevel,
			std::wstring const& inOldName,
			std::wstring const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

	virtual bool Update(
			ARBConfig const& inConfigCurrent,
			std::wstring& ioVenue,
			std::wstring& ioDivision,
			std::wstring& ioSubLevel) const;

protected:
	std::wstring m_Venue;
	std::wstring m_Div;
	std::wstring m_Level; ///< Only used on sublevels.
	std::wstring m_OldName;
	std::wstring m_NewName;
};


class ARBConfigActionDeleteLevel : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteLevel(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inLevel,
			std::wstring const& inName);
	ARBConfigActionDeleteLevel(ARBConfigActionDeleteLevel const& rhs);

public:
	static ARBConfigActionPtr NewLevel(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inName);

	static ARBConfigActionPtr NewSubLevel(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inLevel,
			std::wstring const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	std::wstring m_Venue;
	std::wstring m_Div;
	std::wstring m_Level; ///< Only used on sublevels.
	std::wstring m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameTitle : public ARBConfigAction
{
protected:
	ARBConfigActionRenameTitle(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName);
	ARBConfigActionRenameTitle(ARBConfigActionRenameTitle const& rhs);

public:
	static ARBConfigActionPtr New(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	std::wstring m_Venue;
	std::wstring m_OldName;
	std::wstring m_NewName;
};


class ARBConfigActionDeleteTitle : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteTitle(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inOldName,
			std::wstring const& inNewName);
	ARBConfigActionDeleteTitle(ARBConfigActionDeleteTitle const& rhs);

public:
	static ARBConfigActionPtr New(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inOldName,
			std::wstring const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	std::wstring m_Venue;
	std::wstring m_Div;
	std::wstring m_OldName;
	std::wstring m_NewName;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameEvent : public ARBConfigAction
{
protected:
	ARBConfigActionRenameEvent(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName);
	ARBConfigActionRenameEvent(ARBConfigActionRenameEvent const& rhs);

public:
	static ARBConfigActionPtr New(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inOldName,
			std::wstring const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	std::wstring m_Venue;
	std::wstring m_OldName;
	std::wstring m_NewName;
};


class ARBConfigActionDeleteEvent : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteEvent(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inName);
	ARBConfigActionDeleteEvent(ARBConfigActionDeleteEvent const& rhs);

public:
	static ARBConfigActionPtr New(
			short configVersion,
			std::wstring const& inVenue,
			std::wstring const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	std::wstring m_Venue;
	std::wstring m_Name;
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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	int Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			std::wostringstream& ioInfo,
			IConfigActionCallback& ioCallBack) const;

	/**
	 * Allow the UI to update filter contents.
	 * @param configVersionPreUpdate Version of inConfigCurrent prior to update.
	 * @param inConfigCurrent Existing configuration to check from.
	 * @param ioVenue Name of venue
	 * @param ioDivision Name of division
	 * @param ioSubLevel Name of sublevel (or level)
	 * @return A name was updated.
	 */
	bool Update(
			short configVersionPreUpdate,
			ARBConfig const& inConfigCurrent,
			std::wstring& ioVenue,
			std::wstring& ioDivision,
			std::wstring& ioSubLevel) const;
};
