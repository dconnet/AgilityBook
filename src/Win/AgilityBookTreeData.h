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

#include "ARB/ARBDog.h"
#include "ARB/ARBDogTrial.h"
#include "AgilityBookMenu.h"
#include "LibARBWin/ListData.h"
#include <vector>


namespace dconSoft
{
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


class CAgilityBookTreeData : public ARBWin::CTreeData
{
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookTreeData)
public:
	CAgilityBookTreeData(CAgilityBookTreeView* pTree)
		: m_pTree(pTree)
	{
	}

	// This removes a need for a dynamic cast
	virtual ARBTreeDataType GetType() const = 0;

	virtual CAgilityBookTreeData const* GetParent() const = 0;
	virtual ARB::ARBBasePtr GetARBBase() const
	{
		return ARB::ARBBasePtr();
	}
	virtual ARB::ARBDogPtr GetDog() const
	{
		return ARB::ARBDogPtr();
	}
	virtual ARB::ARBDogTrialPtr GetTrial() const
	{
		return ARB::ARBDogTrialPtr();
	}
	virtual ARB::ARBDogRunPtr GetRun() const
	{
		return ARB::ARBDogRunPtr();
	}
	virtual ARB::ARBDogPtr GetDog()
	{
		return ARB::ARBDogPtr();
	}
	virtual ARB::ARBDogTrialPtr GetTrial()
	{
		return ARB::ARBDogTrialPtr();
	}
	virtual ARB::ARBDogRunPtr GetRun()
	{
		return ARB::ARBDogRunPtr();
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
	CAgilityBookTreeDataDog(CAgilityBookTreeView* pTree, ARB::ARBDogPtr const& inDog);
	~CAgilityBookTreeDataDog();

	ARBTreeDataType GetType() const override
	{
		return ARBTreeDataType::Dog;
	}

	CAgilityBookTreeData const* GetParent() const override
	{
		return nullptr;
	}
	ARB::ARBBasePtr GetARBBase() const override
	{
		return m_pDog;
	}
	ARB::ARBDogPtr GetDog() const override
	{
		return m_pDog;
	}
	ARB::ARBDogPtr GetDog() override
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
	ARB::ARBDogPtr m_pDog;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataTrial : public CAgilityBookTreeData
{
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookTreeDataTrial)
public:
	CAgilityBookTreeDataTrial(CAgilityBookTreeView* pTree, ARB::ARBDogTrialPtr const& inTrial);

	ARBTreeDataType GetType() const override
	{
		return ARBTreeDataType::Trial;
	}

	CAgilityBookTreeData const* GetParent() const override
	{
		return GetDataDog();
	}
	ARB::ARBBasePtr GetARBBase() const override
	{
		return m_pTrial;
	}
	ARB::ARBDogPtr GetDog() const override;
	ARB::ARBDogTrialPtr GetTrial() const override
	{
		return m_pTrial;
	}
	ARB::ARBDogPtr GetDog() override;
	ARB::ARBDogTrialPtr GetTrial() override
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
	ARB::ARBDogTrialPtr m_pTrial;
	int m_idxIcon;
};

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeDataRun : public CAgilityBookTreeData
{
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookTreeDataRun)
public:
	CAgilityBookTreeDataRun(CAgilityBookTreeView* pTree, ARB::ARBDogRunPtr const& inRun);
	~CAgilityBookTreeDataRun();

	ARBTreeDataType GetType() const override
	{
		return ARBTreeDataType::Run;
	}

	CAgilityBookTreeData const* GetParent() const override
	{
		return GetDataTrial();
	}
	ARB::ARBBasePtr GetARBBase() const override
	{
		return m_pRun;
	}
	ARB::ARBDogPtr GetDog() const override;
	ARB::ARBDogTrialPtr GetTrial() const override;
	ARB::ARBDogRunPtr GetRun() const override
	{
		return m_pRun;
	}
	ARB::ARBDogPtr GetDog() override;
	ARB::ARBDogTrialPtr GetTrial() override;
	ARB::ARBDogRunPtr GetRun() override
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
	ARB::ARBDogRunPtr m_pRun;
};

} // namespace dconSoft
