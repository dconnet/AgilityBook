#pragma once

/*
 * Copyright © 2007-2008 David Connet. All Rights Reserved.
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
 *
 * @brief List of icons
 * @author David Connet
 *
 * Revision History
 * @li 2007-04-22 DRC Created.
 */

class CIconList : public CImageList
{
public:
	CIconList();

	int Dog() const			{return m_idxDog;}
	int Trial() const		{return m_idxTrial;}
	int Run() const			{return m_idxRun;}
	int AAC() const			{return m_idxVenueAAC;}
	int AKC() const			{return m_idxVenueAKC;}
	int ASCA() const		{return m_idxVenueASCA;}
	int CKC() const			{return m_idxVenueCKC;}
	int CPE() const			{return m_idxVenueCPE;}
	int DOCNA() const		{return m_idxVenueDOCNA;}
	int NADAC() const		{return m_idxVenueNADAC;}
	int Sweepstakes() const	{return m_idxVenueSweep;}
	int TDAA() const		{return m_idxVenueTDAA;}
	int UKC() const			{return m_idxVenueUKC;}
	int USDAA() const		{return m_idxVenueUSDAA;}
	int FCI() const			{return m_idxVenueFCI;}
	int SCC() const			{return m_idxVenueSCC;}

protected:
	int m_idxDog;
	int m_idxTrial;
	int m_idxRun;
	int m_idxVenueAAC;
	int m_idxVenueAKC;
	int m_idxVenueASCA;
	int m_idxVenueCKC;
	int m_idxVenueCPE;
	int m_idxVenueDOCNA;
	int m_idxVenueNADAC;
	int m_idxVenueSweep;
	int m_idxVenueTDAA;
	int m_idxVenueUKC;
	int m_idxVenueUSDAA;
	int m_idxVenueFCI;
	int m_idxVenueSCC;
};
