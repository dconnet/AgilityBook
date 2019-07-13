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


enum class ARBTreeDataType
{
	Dog,
	Trial,
	Run
};


class CAgilityBookTreeData : public CTreeData
{
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookTreeData)
public:
	CAgilityBookTreeData(CAgilityBookTreeView* pTree)
		: m_pTree(pTree)
	{}

	// This removes a need for a dynamic cast
	virtual ARBTreeDataType GetType() const = 0;

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
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookTreeDataDog)
public:
	CAgilityBookTreeDataDog(
			CAgilityBookTreeView* pTree,
			ARBDogPtr const& inDog);
	~CAgilityBookTreeDataDog();

	ARBTreeDataType GetType() const override
	{
		return ARBTreeDataType::Dog;
	}

	CAgilityBookTreeData const* GetParent() const override
	{
		return nullptr;
	}
	ARBBasePtr GetARBBase() const override
	{
		return m_pDog;
	}
	ARBDogPtr GetDog() const override
	{
		return m_pDog;
	}
	ARBDogPtr GetDog() override
	{
		return m_pDog;
	}
	CAgilityBookTreeDataDog const* GetDataDog() const override
	{
		return this;
	}
	CAgilityBookTreeDataDog* GetDataDog() override
	{
		return this;
	}

	MenuIdentityPopup GetMenuID() const override
	{
		return IdMenuDog;
	}
	std::wstring OnNeedText() const override;
	int OnNeedIcon() const override;

	void Properties() override;
	bool DoCopy() override;
	bool DoDuplicate() override;
	bool DoDelete(bool bSilent) override;

private:
	ARBDogPtr m_pDog;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataTrial : public CAgilityBookTreeData
{
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookTreeDataTrial)
public:
	CAgilityBookTreeDataTrial(
			CAgilityBookTreeView* pTree,
			ARBDogTrialPtr const& inTrial);

	ARBTreeDataType GetType() const override
	{
		return ARBTreeDataType::Trial;
	}

	CAgilityBookTreeData const* GetParent() const override
	{
		return GetDataDog();
	}
	ARBBasePtr GetARBBase() const override
	{
		return m_pTrial;
	}
	ARBDogPtr GetDog() const override;
	ARBDogTrialPtr GetTrial() const override
	{
		return m_pTrial;
	}
	ARBDogPtr GetDog() override;
	ARBDogTrialPtr GetTrial() override
	{
		return m_pTrial;
	}
	CAgilityBookTreeDataDog const* GetDataDog() const override;
	CAgilityBookTreeDataTrial const* GetDataTrial() const override
	{
		return this;
	}
	CAgilityBookTreeDataDog* GetDataDog() override;
	CAgilityBookTreeDataTrial* GetDataTrial() override
	{
		return this;
	}

	MenuIdentityPopup GetMenuID() const override
	{
		return IdMenuTrial;
	}
	std::wstring OnNeedText() const override;
	int OnNeedIcon() const override;

	void Properties() override;
	bool DoCopy() override;
	bool DoDuplicate() override;
	bool DoDelete(bool bSilent) override;

private:
	ARBDogTrialPtr m_pTrial;
	int m_idxIcon;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataRun : public CAgilityBookTreeData
{
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookTreeDataRun)
public:
	CAgilityBookTreeDataRun(
			CAgilityBookTreeView* pTree,
			ARBDogRunPtr const& inRun);
	~CAgilityBookTreeDataRun();

	ARBTreeDataType GetType() const override
	{
		return ARBTreeDataType::Run;
	}

	CAgilityBookTreeData const* GetParent() const override
	{
		return GetDataTrial();
	}
	ARBBasePtr GetARBBase() const override
	{
		return m_pRun;
	}
	ARBDogPtr GetDog() const override;
	ARBDogTrialPtr GetTrial() const override;
	ARBDogRunPtr GetRun() const override
	{
		return m_pRun;
	}
	ARBDogPtr GetDog() override;
	ARBDogTrialPtr GetTrial() override;
	ARBDogRunPtr GetRun() override
	{
		return m_pRun;
	}
	CAgilityBookTreeDataDog const* GetDataDog() const override;
	CAgilityBookTreeDataTrial const* GetDataTrial() const override;
	CAgilityBookTreeDataRun const* GetDataRun() const override
	{
		return this;
	}
	CAgilityBookTreeDataDog* GetDataDog() override;
	CAgilityBookTreeDataTrial* GetDataTrial() override;
	CAgilityBookTreeDataRun* GetDataRun() override
	{
		return this;
	}

	MenuIdentityPopup GetMenuID() const override
	{
		return IdMenuRun;
	}
	std::wstring OnNeedText() const override;
	int OnNeedIcon() const override;

	void Properties() override;
	bool DoCopy() override;
	bool DoDuplicate() override;
	bool DoDelete(bool bSilent) override;

private:
	ARBDogRunPtr m_pRun;
};
