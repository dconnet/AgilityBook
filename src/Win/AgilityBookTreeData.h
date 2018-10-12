#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Data classes to support AgilityBookTree operations.
 * @author David Connet
 *
 * Revision History
 * 2009-07-11 Added GetType()
 * 2009-02-02 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2003-12-30 Implemented customized text in tree.
 */

#include "AgilityBookMenu.h"
#include "ARB/ARBDog.h"
#include "ARB/ARBDogTrial.h"
#include "LibARBWin/ListData.h"
#include <vector>
class CAgilityBookTreeView;
class CAgilityBookTreeDataDog;
class CAgilityBookTreeDataTrial;
class CAgilityBookTreeDataRun;


class CAgilityBookTreeData : public CTreeData
{
public:
	CAgilityBookTreeData(CAgilityBookTreeView* pTree)
		: m_pTree(pTree)
	{}

	// This removes a need for a dynamic cast
	enum CTreeDataType { eTreeDog, eTreeTrial, eTreeRun };
	virtual CTreeDataType GetType() const = 0;

	virtual CAgilityBookTreeData const* GetParent() const = 0;
	virtual ARBBasePtr GetARBBase() const
	{
		return ARBBasePtr();
	}
	virtual ARBDogPtr GetDog() const
	{
		return ARBDogPtr();
	}
	virtual ARBDogTrialPtr GetTrial() const
	{
		return ARBDogTrialPtr();
	}
	virtual ARBDogRunPtr GetRun() const
	{
		return ARBDogRunPtr();
	}
	virtual ARBDogPtr GetDog()
	{
		return ARBDogPtr();
	}
	virtual ARBDogTrialPtr GetTrial()
	{
		return ARBDogTrialPtr();
	}
	virtual ARBDogRunPtr GetRun()
	{
		return ARBDogRunPtr();
	}
	virtual CAgilityBookTreeDataDog const* GetDataDog() const
	{
		return nullptr;
	}
	virtual CAgilityBookTreeDataTrial const* GetDataTrial() const
	{
		return nullptr;
	}
	virtual CAgilityBookTreeDataRun const* GetDataRun() const
	{
		return nullptr;
	}
	virtual CAgilityBookTreeDataDog* GetDataDog()
	{
		return nullptr;
	}
	virtual CAgilityBookTreeDataTrial* GetDataTrial()
	{
		return nullptr;
	}
	virtual CAgilityBookTreeDataRun* GetDataRun()
	{
		return nullptr;
	}

	virtual MenuIdentityPopup GetMenuID() const = 0;
	virtual std::wstring OnNeedText() const = 0;
	virtual int OnNeedIcon() const = 0;

	virtual void Properties() = 0;
	virtual bool DoCopy() = 0;
	virtual bool DoDuplicate() = 0;
	virtual bool DoDelete(bool bSilent) = 0;

protected:
	CAgilityBookTreeView* m_pTree;
	std::vector<long> const& GetDogColumns() const;
	std::vector<long> const& GetTrialColumns() const;
	std::vector<long> const& GetRunColumns() const;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataDog : public CAgilityBookTreeData
{
public:
	CAgilityBookTreeDataDog(
			CAgilityBookTreeView* pTree,
			ARBDogPtr const& inDog);
	~CAgilityBookTreeDataDog();

	virtual CTreeDataType GetType() const
	{
		return CAgilityBookTreeData::eTreeDog;
	}

	virtual CAgilityBookTreeData const* GetParent() const
	{
		return nullptr;
	}
	virtual ARBBasePtr GetARBBase() const
	{
		return m_pDog;
	}
	virtual ARBDogPtr GetDog() const
	{
		return m_pDog;
	}
	virtual ARBDogPtr GetDog()
	{
		return m_pDog;
	}
	virtual CAgilityBookTreeDataDog const* GetDataDog() const
	{
		return this;
	}
	virtual CAgilityBookTreeDataDog* GetDataDog()
	{
		return this;
	}

	virtual MenuIdentityPopup GetMenuID() const
	{
		return IdMenuDog;
	}
	virtual std::wstring OnNeedText() const;
	virtual int OnNeedIcon() const;

	virtual void Properties();
	virtual bool DoCopy();
	virtual bool DoDuplicate();
	virtual bool DoDelete(bool bSilent);

private:
	ARBDogPtr m_pDog;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataTrial : public CAgilityBookTreeData
{
public:
	CAgilityBookTreeDataTrial(
			CAgilityBookTreeView* pTree,
			ARBDogTrialPtr const& inTrial);

	virtual CTreeDataType GetType() const
	{
		return CAgilityBookTreeData::eTreeTrial;
	}

	virtual CAgilityBookTreeData const* GetParent() const
	{
		return GetDataDog();
	}
	virtual ARBBasePtr GetARBBase() const
	{
		return m_pTrial;
	}
	virtual ARBDogPtr GetDog() const;
	virtual ARBDogTrialPtr GetTrial() const
	{
		return m_pTrial;
	}
	virtual ARBDogPtr GetDog();
	virtual ARBDogTrialPtr GetTrial()
	{
		return m_pTrial;
	}
	virtual CAgilityBookTreeDataDog const* GetDataDog() const;
	virtual CAgilityBookTreeDataTrial const* GetDataTrial() const
	{
		return this;
	}
	virtual CAgilityBookTreeDataDog* GetDataDog();
	virtual CAgilityBookTreeDataTrial* GetDataTrial()
	{
		return this;
	}

	virtual MenuIdentityPopup GetMenuID() const
	{
		return IdMenuTrial;
	}
	virtual std::wstring OnNeedText() const;
	virtual int OnNeedIcon() const;

	virtual void Properties();
	virtual bool DoCopy();
	virtual bool DoDuplicate();
	virtual bool DoDelete(bool bSilent);

private:
	ARBDogTrialPtr m_pTrial;
	int m_idxIcon;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataRun : public CAgilityBookTreeData
{
public:
	CAgilityBookTreeDataRun(
			CAgilityBookTreeView* pTree,
			ARBDogRunPtr const& inRun);
	~CAgilityBookTreeDataRun();

	virtual CTreeDataType GetType() const
	{
		return CAgilityBookTreeData::eTreeRun;
	}

	virtual CAgilityBookTreeData const* GetParent() const
	{
		return GetDataTrial();
	}
	virtual ARBBasePtr GetARBBase() const
	{
		return m_pRun;
	}
	virtual ARBDogPtr GetDog() const;
	virtual ARBDogTrialPtr GetTrial() const;
	virtual ARBDogRunPtr GetRun() const
	{
		return m_pRun;
	}
	virtual ARBDogPtr GetDog();
	virtual ARBDogTrialPtr GetTrial();
	virtual ARBDogRunPtr GetRun()
	{
		return m_pRun;
	}
	virtual CAgilityBookTreeDataDog const* GetDataDog() const;
	virtual CAgilityBookTreeDataTrial const* GetDataTrial() const;
	virtual CAgilityBookTreeDataRun const* GetDataRun() const
	{
		return this;
	}
	virtual CAgilityBookTreeDataDog* GetDataDog();
	virtual CAgilityBookTreeDataTrial* GetDataTrial();
	virtual CAgilityBookTreeDataRun* GetDataRun()
	{
		return this;
	}

	virtual MenuIdentityPopup GetMenuID() const
	{
		return IdMenuRun;
	}
	virtual std::wstring OnNeedText() const;
	virtual int OnNeedIcon() const;

	virtual void Properties();
	virtual bool DoCopy();
	virtual bool DoDuplicate();
	virtual bool DoDelete(bool bSilent);

private:
	ARBDogRunPtr m_pRun;
};
