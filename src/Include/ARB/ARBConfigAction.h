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
 * 2016-06-17 Add support for Lifetime names.
 * 2013-01-11 Fix filters on configuration import.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-12-03 Complete re-write of action class.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-26 Added optional 'Div' to DeleteTitle.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-01-21 Created
 */

#include "ARBBase.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"


namespace dconSoft
{
namespace ARB
{

class ARB_API IConfigActionCallback
{
	DECLARE_NO_COPY_IMPLEMENTED(IConfigActionCallback)
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


#define DECLARE_NO_COPY_ACTION(cls) \
	cls() = delete; \
	cls(cls&& rhs) = delete; \
	cls& operator=(cls const& rhs) = delete; \
	cls& operator=(cls&& rhs) = delete;


/**
 * Special class that performs actions during ARBConfig::Update.
 *
 * During the update process, all actions are run in order, operating on the
 * existing configuration and data. The existing configuration is then updated.
 */
class ARB_API ARBConfigAction : public ARBBase
{
protected:
	explicit ARBConfigAction(short configVersion);
	ARBConfigAction(ARBConfigAction const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigAction)

public:
	~ARBConfigAction();
	virtual ARBConfigActionPtr Clone() const = 0;

	bool operator==(ARBConfigAction const& rhs) const
	{
		return false;
	}
	bool operator!=(ARBConfigAction const& rhs) const
	{
		return true;
	}

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
	wxString GetGenericName() const override
	{
		return wxString();
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 * @note There are no strings to search in this object.
	 */
	size_t GetSearchStrings(std::set<wxString>& ioStrings) const override
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
	virtual bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const
		= 0;

	/**
	 * Allow the UI to update filter contents.
	 */
	virtual bool Update(ARBConfig const& inConfigCurrent, wxString& ioVenue, wxString& ioDivision, wxString& ioSubLevel)
		const
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

class ARB_API ARBConfigActionRenameOtherPoints : public ARBConfigAction
{
protected:
	ARBConfigActionRenameOtherPoints(short configVersion, wxString const& inOldName, wxString const& inNewName);
	ARBConfigActionRenameOtherPoints(ARBConfigActionRenameOtherPoints const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionRenameOtherPoints)

public:
	static ARBConfigActionPtr New(short configVersion, wxString const& inOldName, wxString const& inNewName);
	~ARBConfigActionRenameOtherPoints()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

protected:
	wxString m_OldName;
	wxString m_NewName;
};


class ARB_API ARBConfigActionDeleteOtherPoints : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteOtherPoints(short configVersion, wxString const& inName);
	ARBConfigActionDeleteOtherPoints(ARBConfigActionDeleteOtherPoints const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionDeleteOtherPoints)

public:
	static ARBConfigActionPtr New(short configVersion, wxString const& inName);
	~ARBConfigActionDeleteOtherPoints()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

protected:
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARB_API ARBConfigActionRenameVenue : public ARBConfigAction
{
protected:
	ARBConfigActionRenameVenue(short configVersion, wxString const& inOldName, wxString const& inNewName);
	ARBConfigActionRenameVenue(ARBConfigActionRenameVenue const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionRenameVenue)

public:
	static ARBConfigActionPtr New(short configVersion, wxString const& inOldName, wxString const& inNewName);
	~ARBConfigActionRenameVenue()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

	bool Update(ARBConfig const& inConfigCurrent, wxString& ioVenue, wxString& ioDivision, wxString& ioSubLevel)
		const override;

private:
	wxString m_OldName;
	wxString m_NewName;
};


class ARB_API ARBConfigActionDeleteVenue : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteVenue(short configVersion, wxString const& inName);
	ARBConfigActionDeleteVenue(ARBConfigActionDeleteVenue const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionDeleteVenue)

public:
	static ARBConfigActionPtr New(short configVersion, wxString const& inName);
	~ARBConfigActionDeleteVenue()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

private:
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARB_API ARBConfigActionRenameMultiQ : public ARBConfigAction
{
protected:
	ARBConfigActionRenameMultiQ(
		short configVersion,
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName);
	ARBConfigActionRenameMultiQ(ARBConfigActionRenameMultiQ const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionRenameMultiQ)

public:
	static ARBConfigActionPtr New(
		short configVersion,
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName);
	~ARBConfigActionRenameMultiQ()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

protected:
	wxString m_Venue;
	wxString m_OldName;
	wxString m_NewName;
};


class ARB_API ARBConfigActionDeleteMultiQ : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteMultiQ(short configVersion, wxString const& inVenue, wxString const& inName);
	ARBConfigActionDeleteMultiQ(ARBConfigActionDeleteMultiQ const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionDeleteMultiQ)

public:
	static ARBConfigActionPtr New(short configVersion, wxString const& inVenue, wxString const& inName);
	~ARBConfigActionDeleteMultiQ()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

protected:
	wxString m_Venue;
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARB_API ARBConfigActionRenameDivision : public ARBConfigAction
{
protected:
	ARBConfigActionRenameDivision(
		short configVersion,
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName);
	ARBConfigActionRenameDivision(ARBConfigActionRenameDivision const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionRenameDivision)

public:
	static ARBConfigActionPtr New(
		short configVersion,
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName);
	~ARBConfigActionRenameDivision()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

	bool Update(ARBConfig const& inConfigCurrent, wxString& ioVenue, wxString& ioDivision, wxString& ioSubLevel)
		const override;

protected:
	wxString m_Venue;
	wxString m_OldName;
	wxString m_NewName;
};


class ARB_API ARBConfigActionDeleteDivision : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteDivision(short configVersion, wxString const& inVenue, wxString const& inName);
	ARBConfigActionDeleteDivision(ARBConfigActionDeleteDivision const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionDeleteDivision)

public:
	static ARBConfigActionPtr New(short configVersion, wxString const& inVenue, wxString const& inName);
	~ARBConfigActionDeleteDivision()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

protected:
	wxString m_Venue;
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARB_API ARBConfigActionRenameLevel : public ARBConfigAction
{
protected:
	ARBConfigActionRenameLevel(
		short configVersion,
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inLevel,
		wxString const& inOldName,
		wxString const& inNewName);
	ARBConfigActionRenameLevel(ARBConfigActionRenameLevel const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionRenameLevel)

public:
	static ARBConfigActionPtr NewLevel(
		short configVersion,
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inOldName,
		wxString const& inNewName);
	~ARBConfigActionRenameLevel()
	{
	}

	static ARBConfigActionPtr NewSubLevel(
		short configVersion,
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inLevel,
		wxString const& inOldName,
		wxString const& inNewName);

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

	bool Update(ARBConfig const& inConfigCurrent, wxString& ioVenue, wxString& ioDivision, wxString& ioSubLevel)
		const override;

protected:
	wxString m_Venue;
	wxString m_Div;
	wxString m_Level; ///< Only used on sublevels.
	wxString m_OldName;
	wxString m_NewName;
};


class ARB_API ARBConfigActionDeleteLevel : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteLevel(
		short configVersion,
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inLevel,
		wxString const& inName);
	ARBConfigActionDeleteLevel(ARBConfigActionDeleteLevel const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionDeleteLevel)

public:
	static ARBConfigActionPtr NewLevel(
		short configVersion,
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inName);

	static ARBConfigActionPtr NewSubLevel(
		short configVersion,
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inLevel,
		wxString const& inName);

	~ARBConfigActionDeleteLevel()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

protected:
	wxString m_Venue;
	wxString m_Div;
	wxString m_Level; ///< Only used on sublevels.
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARB_API ARBConfigActionRenameTitle : public ARBConfigAction
{
protected:
	ARBConfigActionRenameTitle(
		short configVersion,
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName);
	ARBConfigActionRenameTitle(ARBConfigActionRenameTitle const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionRenameTitle)

public:
	static ARBConfigActionPtr New(
		short configVersion,
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName);
	~ARBConfigActionRenameTitle()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

protected:
	wxString m_Venue;
	wxString m_OldName;
	wxString m_NewName;
};


class ARB_API ARBConfigActionDeleteTitle : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteTitle(
		short configVersion,
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inOldName,
		wxString const& inNewName);
	ARBConfigActionDeleteTitle(ARBConfigActionDeleteTitle const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionDeleteTitle)

public:
	static ARBConfigActionPtr New(
		short configVersion,
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inOldName,
		wxString const& inNewName);
	~ARBConfigActionDeleteTitle()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

protected:
	wxString m_Venue;
	wxString m_Div;
	wxString m_OldName;
	wxString m_NewName;
};

/////////////////////////////////////////////////////////////////////////////

class ARB_API ARBConfigActionRenameEvent : public ARBConfigAction
{
protected:
	ARBConfigActionRenameEvent(
		short configVersion,
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName);
	ARBConfigActionRenameEvent(ARBConfigActionRenameEvent const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionRenameEvent)

public:
	static ARBConfigActionPtr New(
		short configVersion,
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName);
	~ARBConfigActionRenameEvent()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

protected:
	wxString m_Venue;
	wxString m_OldName;
	wxString m_NewName;
};


class ARB_API ARBConfigActionDeleteEvent : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteEvent(short configVersion, wxString const& inVenue, wxString const& inName);
	ARBConfigActionDeleteEvent(ARBConfigActionDeleteEvent const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionDeleteEvent)

public:
	static ARBConfigActionPtr New(short configVersion, wxString const& inVenue, wxString const& inName);
	~ARBConfigActionDeleteEvent()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

protected:
	wxString m_Venue;
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

class ARB_API ARBConfigActionRenameLifetimeName : public ARBConfigAction
{
protected:
	ARBConfigActionRenameLifetimeName(
		short configVersion,
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName);
	ARBConfigActionRenameLifetimeName(ARBConfigActionRenameLifetimeName const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionRenameLifetimeName)

public:
	static ARBConfigActionPtr New(
		short configVersion,
		wxString const& inVenue,
		wxString const& inOldName,
		wxString const& inNewName);
	~ARBConfigActionRenameLifetimeName()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

protected:
	wxString m_Venue;
	wxString m_OldName;
	wxString m_NewName;
};


class ARB_API ARBConfigActionDeleteLifetimeName : public ARBConfigAction
{
protected:
	ARBConfigActionDeleteLifetimeName(short configVersion, wxString const& inVenue, wxString const& inName);
	ARBConfigActionDeleteLifetimeName(ARBConfigActionDeleteLifetimeName const& rhs);
	DECLARE_NO_COPY_ACTION(ARBConfigActionDeleteLifetimeName)

public:
	static ARBConfigActionPtr New(short configVersion, wxString const& inVenue, wxString const& inName);
	~ARBConfigActionDeleteLifetimeName()
	{
	}

	ARBConfigActionPtr Clone() const override;

	bool Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack)
		const override;

protected:
	wxString m_Venue;
	wxString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigAction objects.
 */
class ARB_API ARBConfigActionList : public ARBVectorNoSave<ARBConfigActionPtr>
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
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	int Apply(ARBConfig& ioConfig, ARBDogList* ioDogs, wxString& ioInfo, IConfigActionCallback& ioCallBack) const;

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
		wxString& ioVenue,
		wxString& ioDivision,
		wxString& ioSubLevel) const;
};

} // namespace ARB
} // namespace dconSoft
