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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
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
	virtual void PreDelete(wxString const& inMsg) = 0;

	/**
	 * Callback made during Update after data loss. This can occur when
	 * a scoring method changes that invalidates an existing run. This
	 * will occur after all PreDelete/Apply processing and after the new
	 * config has been merged.
	 * @param inMsg Message generated in Apply about what is being deleted.
	 */
	virtual void PostDelete(wxString const& inMsg) const = 0;

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
	virtual wxString GetGenericName() const
	{
		return wxT("");
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 * @note There are no strings to search in this object.
	 */
	virtual size_t GetSearchStrings(std::set<wxString>& ioStrings) const
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
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const = 0;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionDeleteCalPlugin : public ARBConfigAction
{ 
protected:
	ARBConfigActionDeleteCalPlugin(
			wxString const& inName);
	ARBConfigActionDeleteCalPlugin(ARBConfigActionDeleteCalPlugin const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameOtherPoints : public ARBConfigAction
{ 
protected:
	ARBConfigActionRenameOtherPoints(
			wxString const& inOldName,
			wxString const& inNewName);
	ARBConfigActionRenameOtherPoints(ARBConfigActionRenameOtherPoints const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inOldName,
			wxString const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_OldName;
	wxString m_NewName;
};


class ARBConfigActionDeleteOtherPoints : public ARBConfigAction
{ 
protected:
	ARBConfigActionDeleteOtherPoints(
			wxString const& inName);
	ARBConfigActionDeleteOtherPoints(ARBConfigActionDeleteOtherPoints const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameVenue : public ARBConfigAction
{
protected:
	ARBConfigActionRenameVenue(
			wxString const& inOldName,
			wxString const& inNewName);
	ARBConfigActionRenameVenue(ARBConfigActionRenameVenue const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inOldName,
			wxString const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

private:
	wxString m_OldName;
	wxString m_NewName;
};


class ARBConfigActionDeleteVenue : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteVenue(
			wxString const& inName);
	ARBConfigActionDeleteVenue(ARBConfigActionDeleteVenue const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

private:
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameMultiQ : public ARBConfigAction
{
protected:
	ARBConfigActionRenameMultiQ(
			wxString const& inVenue,
			wxString const& inOldName,
			wxString const& inNewName);
	ARBConfigActionRenameMultiQ(ARBConfigActionRenameMultiQ const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inVenue,
			wxString const& inOldName,
			wxString const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_Venue;
	wxString m_OldName;
	wxString m_NewName;
};


class ARBConfigActionDeleteMultiQ : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteMultiQ(
			wxString const& inVenue,
			wxString const& inName);
	ARBConfigActionDeleteMultiQ(ARBConfigActionDeleteMultiQ const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inVenue,
			wxString const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_Venue;
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameDivision : public ARBConfigAction
{
protected:
	ARBConfigActionRenameDivision(
			wxString const& inVenue,
			wxString const& inOldName,
			wxString const& inNewName);
	ARBConfigActionRenameDivision(ARBConfigActionRenameDivision const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inVenue,
			wxString const& inOldName,
			wxString const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_Venue;
	wxString m_OldName;
	wxString m_NewName;
};


class ARBConfigActionDeleteDivision : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteDivision(
			wxString const& inVenue,
			wxString const& inName);
	ARBConfigActionDeleteDivision(ARBConfigActionDeleteDivision const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inVenue,
			wxString const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_Venue;
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameLevel : public ARBConfigAction
{
protected:
	ARBConfigActionRenameLevel(
			wxString const& inVenue,
			wxString const& inDiv,
			wxString const& inLevel,
			wxString const& inOldName,
			wxString const& inNewName);
	ARBConfigActionRenameLevel(ARBConfigActionRenameLevel const& rhs);

public:
	static ARBConfigActionPtr NewLevel(
			wxString const& inVenue,
			wxString const& inDiv,
			wxString const& inOldName,
			wxString const& inNewName);

	static ARBConfigActionPtr NewSubLevel(
			wxString const& inVenue,
			wxString const& inDiv,
			wxString const& inLevel,
			wxString const& inOldName,
			wxString const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_Venue;
	wxString m_Div;
	wxString m_Level; ///< Only used on sublevels.
	wxString m_OldName;
	wxString m_NewName;
};


class ARBConfigActionDeleteLevel : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteLevel(
			wxString const& inVenue,
			wxString const& inDiv,
			wxString const& inLevel,
			wxString const& inName);
	ARBConfigActionDeleteLevel(ARBConfigActionDeleteLevel const& rhs);

public:
	static ARBConfigActionPtr NewLevel(
			wxString const& inVenue,
			wxString const& inDiv,
			wxString const& inName);

	static ARBConfigActionPtr NewSubLevel(
			wxString const& inVenue,
			wxString const& inDiv,
			wxString const& inLevel,
			wxString const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_Venue;
	wxString m_Div;
	wxString m_Level; ///< Only used on sublevels.
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameTitle : public ARBConfigAction
{
protected:
	ARBConfigActionRenameTitle(
			wxString const& inVenue,
			wxString const& inOldName,
			wxString const& inNewName);
	ARBConfigActionRenameTitle(ARBConfigActionRenameTitle const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inVenue,
			wxString const& inOldName,
			wxString const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_Venue;
	wxString m_OldName;
	wxString m_NewName;
};


class ARBConfigActionDeleteTitle : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteTitle(
			wxString const& inVenue,
			wxString const& inDiv,
			wxString const& inOldName,
			wxString const& inNewName);
	ARBConfigActionDeleteTitle(ARBConfigActionDeleteTitle const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inVenue,
			wxString const& inDiv,
			wxString const& inOldName,
			wxString const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_Venue;
	wxString m_Div;
	wxString m_OldName;
	wxString m_NewName;
};

/////////////////////////////////////////////////////////////////////////////

class ARBConfigActionRenameEvent : public ARBConfigAction
{
protected:
	ARBConfigActionRenameEvent(
			wxString const& inVenue,
			wxString const& inOldName,
			wxString const& inNewName);
	ARBConfigActionRenameEvent(ARBConfigActionRenameEvent const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inVenue,
			wxString const& inOldName,
			wxString const& inNewName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_Venue;
	wxString m_OldName;
	wxString m_NewName;
};


class ARBConfigActionDeleteEvent : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteEvent(
			wxString const& inVenue,
			wxString const& inName);
	ARBConfigActionDeleteEvent(ARBConfigActionDeleteEvent const& rhs);

public:
	static ARBConfigActionPtr New(
			wxString const& inVenue,
			wxString const& inName);

	virtual ARBConfigActionPtr Clone() const;

	virtual bool Apply(
			ARBConfig& ioConfig,
			ARBDogList* ioDogs,
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;

protected:
	wxString m_Venue;
	wxString m_Name;
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
			wxString& ioInfo,
			IConfigActionCallback& ioCallBack) const;
};
