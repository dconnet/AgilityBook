#include "StdAfx.h"
#include "AgilityBookTreeModel.h"

#include "AgilityBookDoc.h"

#include "ARBTypes.h"
#include "DlgAssignColumns.h"
#include "RegItems.h"
#include "StringUtil.h"
#include <algorithm>
#include <vector>
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeModelNode
{
public:
	CAgilityBookTreeModelNode(CAgilityBookTreeModel* pModel)
		: m_pModel(pModel)
		, m_parent(NULL)
	{
	}
	virtual ~CAgilityBookTreeModelNode();

	typedef enum
	{
		eDog = 0,
		eTrial = 1,
		eRun = 2
	} eNodeType; // Also used as column lookup
	virtual eNodeType Type() const = 0;

	virtual bool IsContainer() const = 0;
	virtual wxIcon GetIcon(unsigned int col) const	{return wxIcon();}
	virtual wxVariant GetColumn(
			ARBConfig const& config,
			std::vector<long> const& columns,
			unsigned int col) const = 0;

	CAgilityBookTreeModelNode* GetParent() const	{return m_parent;}
	unsigned int GetChildren(wxDataViewItemArray& array) const;

	virtual ARBBasePtr GetARBBase() const			{return ARBBasePtr();}
	virtual ARBDogPtr GetDog() const				{return ARBDogPtr();}
	virtual ARBDogTrialPtr GetTrial() const			{return ARBDogTrialPtr();}
	virtual ARBDogRunPtr GetRun() const				{return ARBDogRunPtr();}

	void Append(CAgilityBookTreeModelNode* child);
	void Remove(CAgilityBookTreeModelNode* child);

protected:
	CAgilityBookTreeModel* m_pModel;
private:
	CAgilityBookTreeModelNode* m_parent;
	std::vector<CAgilityBookTreeModelNode*> m_children;
};


CAgilityBookTreeModelNode::~CAgilityBookTreeModelNode()
{
	for (std::vector<CAgilityBookTreeModelNode*>::iterator i = m_children.begin();
		i != m_children.end();
		++i)
	{
		delete *i;
	}
	m_children.clear();
}


unsigned int CAgilityBookTreeModelNode::GetChildren(
		wxDataViewItemArray& array) const
{
	for (std::vector<CAgilityBookTreeModelNode*>::const_iterator i = m_children.begin();
		i != m_children.end();
		++i)
	{
		array.Add(wxDataViewItem(*i));
	}
	return static_cast<unsigned int>(m_children.size());
}


void CAgilityBookTreeModelNode::Append(CAgilityBookTreeModelNode* child)
{
	child->m_parent = this;
	m_children.push_back(child);
}


void CAgilityBookTreeModelNode::Remove(CAgilityBookTreeModelNode* child)
{
	std::vector<CAgilityBookTreeModelNode*>::iterator i = std::find(m_children.begin(), m_children.end(), child);
	if (i != m_children.end())
	{
		CAgilityBookTreeModelNode* node = *i;
		m_children.erase(i);
		delete node;
	}
}

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeModelNodeDog : public CAgilityBookTreeModelNode
{
public:
	CAgilityBookTreeModelNodeDog(
			CAgilityBookTreeModel* pModel,
			ARBDogPtr dog)
		: CAgilityBookTreeModelNode(pModel)
		, m_pDog(dog)
	{
	}
	virtual eNodeType Type() const			{return eDog;}
	virtual bool IsContainer() const		{return true;}
	virtual wxIcon GetIcon(unsigned int col) const;
	virtual wxVariant GetColumn(
			ARBConfig const& config,
			std::vector<long> const& columns,
			unsigned int col) const;

	virtual ARBBasePtr GetARBBase() const			{return m_pDog;}
	virtual ARBDogPtr GetDog() const				{return m_pDog;}

	ARBDogPtr m_pDog;
};


wxIcon CAgilityBookTreeModelNodeDog::GetIcon(unsigned int col) const
{
	if (0 == col)
		return m_pModel->IconList().GetIcon(m_pModel->IconList().Dog());
	return wxIcon();
}


wxVariant CAgilityBookTreeModelNodeDog::GetColumn(
		ARBConfig const& config,
		std::vector<long> const& columns,
		unsigned int col) const
{
	wxVariant variant;
	if (0 == col)
	{
		std::wostringstream str;
		for (size_t idx = 0; idx < columns.size(); ++idx)
		{
			if (0 < idx)
				str << L" ";
			switch (columns[idx])
			{
			case IO_TREE_DOG_REGNAME:
				str << m_pDog->GetRegisteredName();
				break;
			case IO_TREE_DOG_CALLNAME:
				str << m_pDog->GetCallName();
				break;
			case IO_TREE_DOG_BREED:
				str << m_pDog->GetBreed();
				break;
			case IO_TREE_DOG_DOB:
				if (m_pDog->GetDOB().IsValid())
					str << m_pDog->GetDOB().GetString();
				break;
			case IO_TREE_DOG_AGE:
				if (m_pDog->GetDOB().IsValid())
				{
					wxDateTime dob;
					m_pDog->GetDOB().GetDate(dob);
					wxDateTime current = wxDateTime::Now();
					if (m_pDog->GetDeceased().IsValid())
						m_pDog->GetDeceased().GetDate(current);
					wxTimeSpan age = current - dob;
					str << StringUtil::stringW(wxString::Format(_("IDS_YEARS"), ARBDouble::ToString(age.GetDays()/365.0, 1).c_str()));
				}
				break;
			}
		}
		variant = str.str();
	}
	return variant;
}

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeModelNodeTrial : public CAgilityBookTreeModelNode
{
public:
	CAgilityBookTreeModelNodeTrial(
			CAgilityBookTreeModel* pModel,
			ARBDogTrialPtr trial)
		: CAgilityBookTreeModelNode(pModel)
		, m_pTrial(trial)
	{
	}
	virtual eNodeType Type() const			{return eTrial;}
	virtual bool IsContainer() const		{return true;}
	virtual wxIcon GetIcon(unsigned int col) const;
	virtual wxVariant GetColumn(
			ARBConfig const& config,
			std::vector<long> const& columns,
			unsigned int col) const;

	virtual ARBBasePtr GetARBBase() const			{return m_pTrial;}
	virtual ARBDogTrialPtr GetTrial() const			{return m_pTrial;}

	ARBDogTrialPtr m_pTrial;
};


wxIcon CAgilityBookTreeModelNodeTrial::GetIcon(unsigned int col) const
{
	if (0 == col)
	{
		int idx = m_pModel->IconList().Trial(m_pModel->GetDocument()->Book().GetConfig(), m_pTrial);
		return m_pModel->IconList().GetIcon(idx);
	}
	return wxIcon();
}


wxVariant CAgilityBookTreeModelNodeTrial::GetColumn(
		ARBConfig const& config,
		std::vector<long> const& columns,
		unsigned int col) const
{
	wxVariant variant;
	if (0 == col)
	{
		std::wostringstream str;
		bool bNeedSpace = false;
		for (size_t idx = 0; idx < columns.size(); ++idx)
		{
			switch (columns[idx])
			{
			case IO_TREE_TRIAL_START:
				if (m_pTrial->GetRuns().GetStartDate().IsValid())
				{
					if (bNeedSpace)
					{
						if (IO_TREE_TRIAL_END == columns[idx-1])
							str << L"-";
						else
							str << L" ";
					}
					str << m_pTrial->GetRuns().GetStartDate().GetString();
					bNeedSpace = true;
				}
				break;
			case IO_TREE_TRIAL_END:
				if (m_pTrial->GetRuns().GetEndDate().IsValid())
				{
					if (bNeedSpace)
					{
						if (IO_TREE_TRIAL_START == columns[idx-1])
							str << L"-";
						else
							str << L" ";
					}
					str << m_pTrial->GetRuns().GetEndDate().GetString();
					bNeedSpace = true;
				}
				break;
			case IO_TREE_TRIAL_VERIFIED:
				if (bNeedSpace)
					str << L" ";
				if (m_pTrial->IsVerified())
					str << L"*";
				else
					str << L"  "; // 2 spaces due to font (variable spacing)
				bNeedSpace = true;
				break;
			case IO_TREE_TRIAL_CLUB:
				{
					if (bNeedSpace && 0 < m_pTrial->GetClubs().size())
						str << L" ";
					int i = 0;
					for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
						iter != m_pTrial->GetClubs().end();
						++iter, ++i)
					{
						if (0 < i)
							str << L"/";
						str << (*iter)->GetName();
						bNeedSpace = true;
					}
				}
				break;
			case IO_TREE_TRIAL_VENUE:
				{
					if (bNeedSpace && 0 < m_pTrial->GetClubs().size())
						str << L" ";
					int i = 0;
					for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
						iter != m_pTrial->GetClubs().end();
						++iter, ++i)
					{
						if (0 < i)
							str << L"/";
						str << (*iter)->GetVenue();
						bNeedSpace = true;
					}
				}
				break;
			case IO_TREE_TRIAL_LOCATION:
				if (!m_pTrial->GetLocation().empty())
				{
					if (bNeedSpace)
						str << L" ";
					str << m_pTrial->GetLocation();
					bNeedSpace = true;
				}
				break;
			case IO_TREE_TRIAL_NOTES:
				if (!m_pTrial->GetNote().empty())
				{
					if (bNeedSpace)
						str << L" ";
					str << StringUtil::Replace(m_pTrial->GetNote(), L"\n", L" ");
					bNeedSpace = true;
				}
				break;
			}
		}
		variant = str.str();
	}
	return variant;
}

/////////////////////////////////////////////////////////////////////////////

class CAgilityBookTreeModelNodeRun : public CAgilityBookTreeModelNode
{
public:
	CAgilityBookTreeModelNodeRun(
			CAgilityBookTreeModel* pModel,
			ARBDogPtr dog,
			ARBDogTrialPtr trial,
			ARBDogRunPtr run)
		: CAgilityBookTreeModelNode(pModel)
		, m_pDog(dog)
		, m_pTrial(trial)
		, m_pRun(run)
	{
	}
	virtual eNodeType Type() const			{return eRun;}
	virtual bool IsContainer() const		{return false;}
	virtual wxIcon GetIcon(unsigned int col) const;
	virtual wxVariant GetColumn(
			ARBConfig const& config,
			std::vector<long> const& columns,
			unsigned int col) const;

	virtual ARBBasePtr GetARBBase() const			{return m_pRun;}
	virtual ARBDogRunPtr GetRun() const				{return m_pRun;}

	ARBDogPtr m_pDog;
	ARBDogTrialPtr m_pTrial;
	ARBDogRunPtr m_pRun;
};


wxIcon CAgilityBookTreeModelNodeRun::GetIcon(unsigned int col) const
{
	if (0 == col)
		return m_pModel->IconList().GetIcon(m_pModel->IconList().Run());
	return wxIcon();
}


wxVariant CAgilityBookTreeModelNodeRun::GetColumn(
		ARBConfig const& config,
		std::vector<long> const& columns,
		unsigned int col) const
{
	wxVariant variant;
	if (0 < col && m_pRun)
	{
		std::wostringstream str;
		short val;
//TODO: not sure how this will work now
		// Col 0 is special: it has the icons. Instead of saving it,
		// we simply ignore it - so col is always off by 1.
		switch (columns[col-1])
		{
		default:
			break;

		case IO_RUNS_REG_NAME:
			str << m_pDog->GetRegisteredName();
			break;
		case IO_RUNS_CALL_NAME:
			str << m_pDog->GetCallName();
			break;
		case IO_RUNS_DATE:
			str << m_pRun->GetDate().GetString();
			break;
		case IO_RUNS_VENUE:
			{
				int i = 0;
				for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
					iter != m_pTrial->GetClubs().end();
					++iter, ++i)
				{
					if (0 < i)
						str << L"/";
					str << (*iter)->GetVenue();
				}
			}
			break;
		case IO_RUNS_CLUB:
			{
				int i = 0;
				for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
					iter != m_pTrial->GetClubs().end();
					++iter, ++i)
				{
					if (0 < i)
						str << L"/";
					str << (*iter)->GetName();
				}
			}
			break;
		case IO_RUNS_LOCATION:
			str << m_pTrial->GetLocation();
			break;
		case IO_RUNS_TRIAL_NOTES:
			str << m_pTrial->GetNote();
			break;
		case IO_RUNS_DIVISION:
			str << m_pRun->GetDivision();
			break;
		case IO_RUNS_LEVEL:
			str << m_pRun->GetLevel();
			break;
		case IO_RUNS_EVENT:
			str << m_pRun->GetEvent();
			break;
		case IO_RUNS_HEIGHT:
			str << m_pRun->GetHeight();
			break;
		case IO_RUNS_JUDGE:
			str << m_pRun->GetJudge();
			break;
		case IO_RUNS_HANDLER:
			str << m_pRun->GetHandler();
			break;
		case IO_RUNS_CONDITIONS:
			str << m_pRun->GetConditions();
			break;
		case IO_RUNS_COURSE_FAULTS:
			str << m_pRun->GetScoring().GetCourseFaults();
			break;
		case IO_RUNS_TIME:
			str << ARBDouble::ToString(m_pRun->GetScoring().GetTime());
			break;
		case IO_RUNS_YARDS:
			if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType()
			&& 0.0 < m_pRun->GetScoring().GetYards())
			{
				str << ARBDouble::ToString(m_pRun->GetScoring().GetYards(), 0);
			}
			break;
		case IO_RUNS_MIN_YPS:
			{
				double yps;
				if (m_pRun->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps))
				{
					str << ARBDouble::ToString(yps, 3);
				}
			}
			break;
		case IO_RUNS_YPS:
			{
				double yps;
				if (m_pRun->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps))
				{
					str << ARBDouble::ToString(yps, 3);
				}
			}
			break;
		case IO_RUNS_OBSTACLES:
			{
				short obstacles = m_pRun->GetScoring().GetObstacles();
				if (0 < obstacles)
					str << obstacles;
			}
			break;
		case IO_RUNS_OPS:
			{
				double ops;
				if (m_pRun->GetScoring().GetObstaclesPS(CAgilityBookOptions::GetTableInYPS(), CAgilityBookOptions::GetRunTimeInOPS(), ops))
				{
					str << ARBDouble::ToString(ops, 3);
				}
			}
			break;
		case IO_RUNS_SCT:
			if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType()
			&& 0.0 < m_pRun->GetScoring().GetSCT())
			{
				str << ARBDouble::ToString(m_pRun->GetScoring().GetSCT());
			}
			break;
		case IO_RUNS_TOTAL_FAULTS:
			if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType())
			{
				ARBConfigScoringPtr pScoring;
				if (m_pTrial->GetClubs().GetPrimaryClub())
					config.GetVenues().FindEvent(
						m_pTrial->GetClubs().GetPrimaryClubVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel(),
						m_pRun->GetDate(),
						NULL,
						&pScoring);
				double faults = m_pRun->GetScoring().GetCourseFaults() + m_pRun->GetScoring().GetTimeFaults(pScoring);
				str << ARBDouble::ToString(faults, 0);
			}
			break;
		case IO_RUNS_REQ_OPENING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetNeedOpenPts();
			}
			break;
		case IO_RUNS_REQ_CLOSING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetNeedClosePts();
			}
			break;
		case IO_RUNS_OPENING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetOpenPts();
			}
			break;
		case IO_RUNS_CLOSING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetClosePts();
			}
			break;
		case IO_RUNS_REQ_POINTS:
			if (ARBDogRunScoring::eTypeByPoints == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetNeedOpenPts();
			}
			break;
		case IO_RUNS_POINTS:
			if (ARBDogRunScoring::eTypeByPoints == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetOpenPts();
			}
			break;
		case IO_RUNS_PLACE:
			val = m_pRun->GetPlace();
			if (0 > val)
				str << L"?";
			else if (0 == val)
				str << L"-";
			else
				str << val;
			break;
		case IO_RUNS_IN_CLASS:
			val = m_pRun->GetInClass();
			if (0 >= val)
				str << L"?";
			else
				str << val;
			break;
		case IO_RUNS_DOGSQD:
			val = m_pRun->GetDogsQd();
			if (0 > val)
				str << L"?";
			else
				str << m_pRun->GetDogsQd();
			break;
		case IO_RUNS_Q:
			{
				bool bSet = false;
				if (m_pRun->GetQ().Qualified())
				{
					std::wstring q;
					std::vector<ARBConfigMultiQPtr> multiQs;
					if (0 < m_pRun->GetMultiQs(multiQs))
					{
						for (std::vector<ARBConfigMultiQPtr>::iterator iMultiQ = multiQs.begin(); iMultiQ != multiQs.end(); ++iMultiQ)
						{
							if (!q.empty())
								q += L"/";
							q += (*iMultiQ)->GetShortName();
						}
					}
					if (ARB_Q::eQ_SuperQ == m_pRun->GetQ())
					{
						bSet = true;
						if (!q.empty())
							str << q << L"/";
						str << _("IDS_SQ");
					}
					else
					{
						if (!q.empty())
						{
							bSet = true;
							str << q;
						}
					}
				}
				if (!bSet)
					str << m_pRun->GetQ().str();
			}
			break;
		case IO_RUNS_SCORE:
			if (m_pRun->GetQ().Qualified()
			|| ARB_Q::eQ_NQ == m_pRun->GetQ())
			{
				ARBConfigScoringPtr pScoring;
				if (m_pTrial->GetClubs().GetPrimaryClub())
					config.GetVenues().FindEvent(
						m_pTrial->GetClubs().GetPrimaryClubVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel(),
						m_pRun->GetDate(),
						NULL,
						&pScoring);
				if (pScoring)
				{
					str << ARBDouble::ToString(m_pRun->GetScore(pScoring));
				}
			}
			break;
		case IO_RUNS_TITLE_POINTS:
			{
				double pts = 0;
				if (m_pRun->GetQ().Qualified())
				{
					ARBConfigScoringPtr pScoring;
					if (m_pTrial->GetClubs().GetPrimaryClub())
						config.GetVenues().FindEvent(
							m_pTrial->GetClubs().GetPrimaryClubVenue(),
							m_pRun->GetEvent(),
							m_pRun->GetDivision(),
							m_pRun->GetLevel(),
							m_pRun->GetDate(),
							NULL,
							&pScoring);
					if (pScoring)
					{
						pts = m_pRun->GetTitlePoints(pScoring);
					}
				}
				str << pts;
			}
			break;
		case IO_RUNS_COMMENTS:
			str << StringUtil::Replace(m_pRun->GetNote(), L"\n", L" ");
			break;
		case IO_RUNS_FAULTS:
			{
				int i = 0;
				for (ARBDogFaultList::const_iterator iter = m_pRun->GetFaults().begin();
					iter != m_pRun->GetFaults().end();
					++i, ++iter)
				{
					if (0 < i)
						str << L", ";
					str << (*iter);
				}
			}
			break;
		case IO_RUNS_SPEED:
			{
				ARBConfigScoringPtr pScoring;
				if (m_pTrial->GetClubs().GetPrimaryClub())
					config.GetVenues().FindEvent(
						m_pTrial->GetClubs().GetPrimaryClubVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel(),
						m_pRun->GetDate(),
						NULL,
						&pScoring);
				if (pScoring)
				{
					if (pScoring->HasSpeedPts() && m_pRun->GetQ().Qualified())
					{
						str << m_pRun->GetSpeedPoints(pScoring);
					}
				}
			}
			break;
		}
		variant = str.str();
	}

	return variant;
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeModel::CAgilityBookTreeModel()
	: m_pDoc(NULL)
	, m_Ctrl(NULL)
	, m_Columns()
	, m_roots()
{
}


CAgilityBookTreeModel::~CAgilityBookTreeModel()
{
	DeleteAllItems();
}


CAgilityBookTreeModelNode* CAgilityBookTreeModel::GetNode(const wxDataViewItem& item) const
{
	if (item.IsOk())
		return reinterpret_cast<CAgilityBookTreeModelNode*>(item.GetID());
	return NULL;
}


void CAgilityBookTreeModel::CreateColumns(
		wxDataViewCtrl* ctrl,
		CAgilityBookDoc* pDoc)
{
	m_pDoc = pDoc;
	m_Ctrl = ctrl;

	UpdateColumns();
}


void CAgilityBookTreeModel::UpdateColumns()
{
	m_Ctrl->ClearColumns();

	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_DOG, m_Columns[0]);
	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_TRIAL, m_Columns[1]);
	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_RUNS_LIST, m_Columns[2]);

	if (CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_RUNS_LIST, m_Columns[2]))
	{
		for (size_t iCol = 0; iCol <= m_Columns[2].size(); ++iCol)
		{
			std::wstring str;
			int fmt = 0;
			if (0 < iCol)
			{
				str = CDlgAssignColumns::GetNameFromColumnID(m_Columns[2][iCol-1]);
				fmt = CDlgAssignColumns::GetFormatFromColumnID(m_Columns[2][iCol-1]);
			}

			wxDataViewRenderer* renderer;
			if (0 == iCol)
				renderer = new wxDataViewIconTextRenderer("string", wxDATAVIEW_CELL_INERT);
			else
				renderer = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);

			wxDataViewColumn* column = new wxDataViewColumn(
				StringUtil::stringWX(str),
				renderer,
				iCol,
				wxCOL_WIDTH_AUTOSIZE,
				wxALIGN_LEFT,
				wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
			m_Ctrl->AppendColumn(column);
		}
	}
}


void CAgilityBookTreeModel::LoadData()
{
	wxBusyCursor wait;

	m_Ctrl->Freeze();

	// Remember the currently selected item.
	std::wstring strCallName;
	CAgilityBookTreeModelNode* node = GetNode(m_Ctrl->GetSelection());
	if (!node)
	{
		strCallName = wxConfig::Get()->Read(CFG_SETTINGS_LASTDOG, wxString());
	}
	std::list<ARBBasePtr> baseItems;
	while (node)
	{
		baseItems.push_front(node->GetARBBase());
		node = node->GetParent();
	}

	DeleteAllItems();

	// Load the data
	wxDataViewItem item;
	for (ARBDogList::const_iterator iterDog = m_pDoc->Book().GetDogs().begin();
		iterDog != m_pDoc->Book().GetDogs().end();
		++iterDog)
	{
		CAgilityBookTreeModelNodeDog* nodeDog = new CAgilityBookTreeModelNodeDog(this, *iterDog);
		m_roots.push_back(nodeDog);
		ItemAdded(wxDataViewItem(0), wxDataViewItem(nodeDog));
		if (0 < strCallName.length() && (*iterDog)->GetCallName() == strCallName)
		{
			item = wxDataViewItem(nodeDog);
		}

		for (ARBDogTrialList::const_iterator iterTrial = (*iterDog)->GetTrials().begin();
			iterTrial != (*iterDog)->GetTrials().end();
			++iterTrial)
		{
			if (!(*iterTrial)->IsFiltered())
			{
				CAgilityBookTreeModelNodeTrial* nodeTrial = new CAgilityBookTreeModelNodeTrial(this, *iterTrial);
				nodeDog->Append(nodeTrial);
				for (ARBDogRunList::const_iterator iterRun = (*iterTrial)->GetRuns().begin();
					iterRun != (*iterTrial)->GetRuns().end();
					++iterRun)
				{
					if (!(*iterRun)->IsFiltered())
					{
						CAgilityBookTreeModelNodeRun* nodeRun = new CAgilityBookTreeModelNodeRun(this, *iterDog, *iterTrial, *iterRun);
						nodeTrial->Append(nodeRun);
					}
				}
			}
		}
	}
	Resort();
	Expand(m_Ctrl);

	if (!item.IsOk())
	{
#if 0
		// Work thru all the base items...
		// Otherwise, after a reorder, the wrong item is selected.
		hItem = m_Ctrl->GetRootItem();
		for (std::list<ARBBasePtr>::iterator iter = baseItems.begin();
			iter != baseItems.end();
			++iter)
		{
			pData = FindData(hItem, (*iter));
			if (pData)
				hItem = pData->GetId();
		}
		if (hItem == m_Ctrl->GetRootItem())
			hItem = wxTreeItemId();
#endif
	}
	if (!item.IsOk())
	{
		//wxTreeItemIdValue cookie;
		//hItem = m_Ctrl->GetFirstChild(m_Ctrl->GetRootItem(), cookie);
	}

	if (item.IsOk())
	{
		m_Ctrl->Select(item);
		m_Ctrl->EnsureVisible(item);
	}

	// Reset the context menu reset.
	// We may have reloaded during a context menu operation.
	//m_bReset = false;

	//if (m_Ctrl->IsShownOnScreen())
	//	UpdateMessages();

	m_Ctrl->Thaw();
	//m_bSuppressSelect = false;
}


void CAgilityBookTreeModel::Delete(const wxDataViewItem& item)
{
	CAgilityBookTreeModelNode* node = GetNode(item);
	if (!node) // happens if item.IsOk()==false
		return;

	wxDataViewItem parent(node->GetParent());
	if (!parent.IsOk())
	{
		std::vector<CAgilityBookTreeModelNode*>::iterator i = std::find(m_roots.begin(), m_roots.end(), node);
		wxASSERT(i != m_roots.end());
		m_roots.erase(i);
		delete node;
	}
	else
	{
		node->GetParent()->Remove(node);
	}

	// notify control
	ItemDeleted(parent, item);
}


void CAgilityBookTreeModel::DeleteAllItems()
{
	while (m_roots.size() > 0)
	{
		std::vector<CAgilityBookTreeModelNode*>::iterator i = m_roots.begin();
		CAgilityBookTreeModelNode* node = *i;
		m_roots.erase(i);

		wxDataViewItem parent(0);
		wxDataViewItem item(node);
		delete node;

		ItemDeleted(parent, item);
	}
}


void CAgilityBookTreeModel::Expand(wxDataViewCtrl* list)
{
	for (std::vector<CAgilityBookTreeModelNode*>::iterator i = m_roots.begin();
		i != m_roots.end();
		++i)
	{
		list->Expand(wxDataViewItem(*i));
		if ((*i)->IsContainer())
		{
			wxDataViewItemArray array;
			(*i)->GetChildren(array);
			for (wxDataViewItemArray::iterator iArray = array.begin();
				iArray != array.end();
				++iArray)
			{
				list->Expand(*iArray);
			}
		}
	}
}


int CAgilityBookTreeModel::Compare(
		const wxDataViewItem& item1,
		const wxDataViewItem& item2,
		unsigned int column,
		bool ascending) const
{
	wxASSERT(item1.IsOk() && item2.IsOk()); // should never happen
	CAgilityBookTreeModelNode* node1 = GetNode(item1);
	CAgilityBookTreeModelNode* node2 = GetNode(item2);

	if (node1->IsContainer() && node2->IsContainer())
		column = 0;

	wxVariant value1, value2;
	GetValue(value1, item1, column);
	GetValue(value2, item2, column);

	if (!ascending)
	{
		wxVariant temp = value1;
		value1 = value2;
		value2 = temp;
	}

	if (value1.GetType() == wxT("string"))
	{
		wxString str1 = value1.GetString();
		wxString str2 = value2.GetString();
		int res = str1.CmpNoCase(str2);
		if (res)
			return res;
	}
	else if (value1.GetType() == wxT("long"))
	{
		long l1 = value1.GetLong();
		long l2 = value2.GetLong();
		long res = l1-l2;
		if (res)
			return res;
	}
	else if (value1.GetType() == wxT("double"))
	{
		double d1 = value1.GetDouble();
		double d2 = value2.GetDouble();
		if (d1 < d2)
			return 1;
		if (d1 > d2)
			return -1;
	}
	else if (value1.GetType() == wxT("datetime"))
	{
		wxDateTime dt1 = value1.GetDateTime();
		wxDateTime dt2 = value2.GetDateTime();
		if (dt1.IsEarlierThan(dt2))
			return 1;
		if (dt2.IsEarlierThan(dt1))
			return -1;
	}

	// items must be different
	wxUIntPtr id1 = wxPtrToUInt(item1.GetID());
	wxUIntPtr id2 = wxPtrToUInt(item2.GetID());
	return ascending ? id1 - id2 : id2 - id1;
}


unsigned int CAgilityBookTreeModel::GetColumnCount() const
{
	return static_cast<unsigned int>(m_Columns[2].size());
}


wxString CAgilityBookTreeModel::GetColumnType(unsigned int col) const
{
	return wxT("string");
}


void CAgilityBookTreeModel::GetValue(
		wxVariant& variant,
		const wxDataViewItem& item,
		unsigned int col) const
{
	CAgilityBookTreeModelNode* node = GetNode(item);
	wxASSERT(node);

	wxIcon icon = node->GetIcon(col);
	if (icon.IsOk())
	{
		wxDataViewIconText data;
		data.SetIcon(icon);
		data.SetText(node->GetColumn(m_pDoc->Book().GetConfig(), m_Columns[node->Type()], col));
		variant << data;
	}
	else
		variant = node->GetColumn(m_pDoc->Book().GetConfig(), m_Columns[node->Type()], col);
}


bool CAgilityBookTreeModel::SetValue(
		const wxVariant& variant,
		const wxDataViewItem& item,
		unsigned int col)
{
	return false;
}


wxDataViewItem CAgilityBookTreeModel::GetParent(const wxDataViewItem& item) const
{
	// the invisible root node has no parent
	if (!item.IsOk())
		return wxDataViewItem(0);

	CAgilityBookTreeModelNode* node = GetNode(item);
	return wxDataViewItem(node->GetParent());
}


bool CAgilityBookTreeModel::IsContainer(const wxDataViewItem& item) const
{
	// the invisble root node can have children
	if (!item.IsOk())
		return true;
	return GetNode(item)->IsContainer();
}


unsigned int CAgilityBookTreeModel::GetChildren(
		const wxDataViewItem& parent,
		wxDataViewItemArray& array) const
{
	CAgilityBookTreeModelNode* node = GetNode(parent);
	if (!node)
	{
		/*
		if (m_roots.size() == 0 && m_emptyRoot)
		{
			array.Add(wxDataViewItem(m_emptyRoot));
			return 1;
		}
		*/
		for (std::vector<CAgilityBookTreeModelNode*>::const_iterator i = m_roots.begin();
			i != m_roots.end();
			++i)
		{
			array.Add(wxDataViewItem(*i));
		}
		return m_roots.size();
	}

#pragma PRAGMA_TODO(remove once in use)
_("IDS_NUM_RUNS_QS");

	return node->GetChildren(array);
}
