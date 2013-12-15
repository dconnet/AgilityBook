#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Image Manager
 * @author David Connet
 *
 * Revision History
 * 2012-12-29 Created.
 */

#include <wx/icon.h>


class CImageManager
{
private:
	static bool s_bNoReInit;
	static CImageManager* s_pImageManager;
	CImageManager();
	~CImageManager();

public:
	static CImageManager* Get();
	static void Reset();
	static void Delete();

	wxIcon TabRuns() const				{return m_iconRun;}
	wxIcon TabPoints() const			{return m_iconPoints;}
	wxIcon TabCalendar() const			{return m_iconCal;}
	wxIcon TabTraining() const			{return m_iconLog;}

	wxIcon ARB16() const				{return m_iconARB16;}
	wxIcon ARB32() const				{return m_iconARB32;}
	wxIcon ARB48() const				{return m_iconARB48;}
	wxIcon ARB256() const				{return m_iconARB256;}
	wxIcon Dog() const					{return m_iconDog;}
	wxIcon Trial() const				{return m_iconTrial;}
	wxIcon Run() const					{return m_iconRun;}
	wxIcon AAC() const					{return m_iconVenueAAC;}
	wxIcon AKC() const					{return m_iconVenueAKC;}
	wxIcon ASCA() const					{return m_iconVenueASCA;}
	wxIcon CKC() const					{return m_iconVenueCKC;}
	wxIcon CPE() const					{return m_iconVenueCPE;}
	wxIcon DOCNA() const				{return m_iconVenueDOCNA;}
	wxIcon NADAC() const				{return m_iconVenueNADAC;}
	wxIcon Sweepstakes() const			{return m_iconVenueSweep;}
	wxIcon TDAA() const					{return m_iconVenueTDAA;}
	wxIcon UKC() const					{return m_iconVenueUKC;}
	wxIcon USDAA() const				{return m_iconVenueUSDAA;}
	wxIcon FCI() const					{return m_iconVenueFCI;}
	wxIcon SCC() const					{return m_iconVenueSCC;}

	wxIcon CRCD() const					{return m_iconCRCD;}

	wxIcon Blank() const				{return m_iconBlank;}
	wxIcon Checked() const				{return m_iconChecked;}
	wxIcon UnChecked() const			{return m_iconUnChecked;}
	wxIcon Check() const				{return m_iconCheck;}
	wxIcon Question() const				{return m_iconQuestion;}

	wxIcon AccomConfirm() const			{return m_iconAccConfirm;}
	wxIcon AccomNone() const			{return m_iconAccNone;}
	wxIcon AccomTodo() const			{return m_iconAccTodo;}

	wxIcon CalEntered() const			{return m_iconCalEntered;}
	wxIcon CalEnteredTentative() const	{return m_iconCalEnteredTentative;}
	wxIcon CalPending() const			{return m_iconCalPending;}
	wxIcon CalPendingTentative() const	{return m_iconCalPendingTentative;}
	wxIcon CalPlanning() const			{return m_iconCalPlan;}
	wxIcon CalPlanTentative() const		{return m_iconCalPlanTentative;}
	wxIcon CalTentative() const			{return m_iconCalTentative;}

	wxIcon HeaderDown() const			{return m_iconHdrDown;}
	wxIcon HeaderUp() const				{return m_iconHdrUp;}

	// Added: +
	// Note: Yellow-sticky
	wxIcon NoteAdded() const			{return m_iconNoteAdded;}
	wxIcon NoteNote() const				{return m_iconNoteNote;}
	wxIcon NoteNoteAdded() const		{return m_iconNodeNoteAdded;}

	// Hidden: !
	// Titled: ribbon
	// Have: check
	wxIcon TitleHidden() const				{return m_iconTitleHidden;}
	wxIcon TitleTitledHidden() const		{return m_iconTitledHidden;}
	wxIcon TitleTitledHiddenHave() const	{return m_iconTitledHiddenHave;}
	wxIcon TitleTitled() const				{return m_iconTitled;}
	wxIcon TitleTitledHave() const			{return m_iconTitledHave;}

	wxBitmap NoteButton() const			{return m_bmpNote;}

	wxBitmap InfoEmpty() const			{return m_bmpInfoEmpty;}
	wxBitmap InfoNote() const			{return m_bmpInfoNote;}
	wxBitmap InfoNoteAdded() const		{return m_bmpInfoNoteAdded;}
	wxBitmap InfoNoteNoteAdded() const	{return m_bmpInfoNoteNoteAdded;}

protected:
	wxIcon m_iconARB16;
	wxIcon m_iconARB32;
	wxIcon m_iconARB48;
	wxIcon m_iconARB256;
	wxIcon m_iconRun;
	wxIcon m_iconPoints;
	wxIcon m_iconCal;
	wxIcon m_iconLog;
	wxIcon m_iconDog;
	wxIcon m_iconTrial;
	wxIcon m_iconVenueAAC;
	wxIcon m_iconVenueAKC;
	wxIcon m_iconVenueASCA;
	wxIcon m_iconVenueCKC;
	wxIcon m_iconVenueCPE;
	wxIcon m_iconVenueDOCNA;
	wxIcon m_iconVenueNADAC;
	wxIcon m_iconVenueSweep;
	wxIcon m_iconVenueTDAA;
	wxIcon m_iconVenueUKC;
	wxIcon m_iconVenueUSDAA;
	wxIcon m_iconVenueFCI;
	wxIcon m_iconVenueSCC;

	wxIcon m_iconCRCD;
	wxIcon m_iconBlank;
	wxIcon m_iconChecked;
	wxIcon m_iconUnChecked;
	wxIcon m_iconCheck;
	wxIcon m_iconQuestion;
	wxIcon m_iconAccConfirm;
	wxIcon m_iconAccNone;
	wxIcon m_iconAccTodo;
	wxIcon m_iconCalEntered;
	wxIcon m_iconCalEnteredTentative;
	wxIcon m_iconCalPending;
	wxIcon m_iconCalPendingTentative;
	wxIcon m_iconCalPlan;
	wxIcon m_iconCalPlanTentative;
	wxIcon m_iconCalTentative;
	wxIcon m_iconHdrDown;
	wxIcon m_iconHdrUp;
	wxIcon m_iconNoteAdded;
	wxIcon m_iconNoteNote;
	wxIcon m_iconNodeNoteAdded;
	wxIcon m_iconTitleHidden;
	wxIcon m_iconTitledHidden;
	wxIcon m_iconTitledHiddenHave;
	wxIcon m_iconTitled;
	wxIcon m_iconTitledHave;

	wxBitmap m_bmpNote;

	wxBitmap m_bmpInfoEmpty;
	wxBitmap m_bmpInfoNote;
	wxBitmap m_bmpInfoNoteAdded;
	wxBitmap m_bmpInfoNoteNoteAdded;
};
