#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief List of icons
 * @author David Connet
 *
 * Revision History
 * 2016-06-19 Add a blank icon to fix issue on Mac.
 * 2012-12-29 Move icon management to global list.
 * 2009-01-27 Ported to wxWidgets.
 * 2007-04-22 Created.
 */

#include "ARB/ARBTypes2.h"
#include <wx/imaglist.h>


class CIconList : public wxImageList
{
public:
	CIconList();
	bool Create(wxWindow const* pWindow);

	int IndexEmpty() const	{return m_idxEmpty;}
	int IndexARB() const	{return m_idxARB;}
	int IndexDog() const	{return m_idxDog;}
	int IndexTrial() const	{return m_idxTrial;}
	int IndexRun() const	{return m_idxRun;}
	int AAC() const			{return m_idxVenueAAC;}
	int AKC() const			{return m_idxVenueAKC;}
	int ASCA() const		{return m_idxVenueASCA;}
	int BHA() const			{return m_idxVenueBHA;}
	int CKC() const			{return m_idxVenueCKC;}
	int CKCSC() const		{return m_idxVenueCKCSC;}
	int CPE() const			{return m_idxVenueCPE;}
	int DOCNA() const		{return m_idxVenueDOCNA;}
	int FCI() const			{return m_idxVenueFCI;}
	int NADAC() const		{return m_idxVenueNADAC;}
	int SCC() const			{return m_idxVenueSCC;}
	int Sweepstakes() const	{return m_idxVenueSweep;}
	int TDAA() const		{return m_idxVenueTDAA;}
	int UKC() const			{return m_idxVenueUKC;}
	int UKI() const			{return m_idxVenueUKI;}
	int USDAA() const		{return m_idxVenueUSDAA;}
	int VALOR() const		{return m_idxVenueVALOR;}

	int Trial(ARBConfig& config, ARBDogTrialPtr const& inTrial) const;

protected:
	int m_idxEmpty;
	int m_idxARB;
	int m_idxDog;
	int m_idxTrial;
	int m_idxRun;
	int m_idxVenueAAC;
	int m_idxVenueAKC;
	int m_idxVenueASCA;
	int m_idxVenueBHA;
	int m_idxVenueCKC;
	int m_idxVenueCKCSC;
	int m_idxVenueCPE;
	int m_idxVenueDOCNA;
	int m_idxVenueFCI;
	int m_idxVenueNADAC;
	int m_idxVenueSCC;
	int m_idxVenueSweep;
	int m_idxVenueTDAA;
	int m_idxVenueUKC;
	int m_idxVenueUKI;
	int m_idxVenueUSDAA;
	int m_idxVenueVALOR;
};
