/**********************************************************************

  Audacity: A Digital Audio Editor

  ProgressDialog.h

  Copyright
     Leland Lucius
     Vaughan Johnson

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

*************************************************************************/

#ifndef __AUDACITY_WIDGETS_PROGRESSDIALOG__
#define __AUDACITY_WIDGETS_PROGRESSDIALOG__

#include "../Audacity.h"

#include "../MemoryX.h"
#include <vector>
#include <wx/defs.h>
#include <wx/evtloop.h>
#include <wx/gauge.h>
#include <wx/stattext.h>
#include <wx/utils.h>

#include "wxPanelWrapper.h"

enum class ProgressResult : unsigned
{
   Cancelled = 0, //<! User says that whatever is happening is undesirable and shouldn't have happened at all
   Success,       //<! User says nothing, everything works fine, continue doing whatever we're doing
   Failed,        //<! Something has gone wrong, we should stop and cancel everything we did
   Stopped        //<! Nothing is wrong, but user says we should stop now and leave things as they are now
};

enum ProgressDialogFlags
{
   pdlgEmptyFlags = 0x00000000,
   pdlgHideStopButton = 0x00000001,
   pdlgHideCancelButton = 0x00000002,
   pdlgHideElapsedTime = 0x00000004,
   pdlgConfirmStopCancel = 0x00000008,

   pdlgDefaultFlags = pdlgEmptyFlags
};

////////////////////////////////////////////////////////////
/// ProgressDialog Class
////////////////////////////////////////////////////////////

class AUDACITY_DLL_API ProgressDialog /* not final */ : public wxDialogWrapper
{
public:
   ProgressDialog();

   // Display a simple message.
   ProgressDialog(const wxString & title,
                  const wxString & message = wxEmptyString,
                  int flags = pdlgDefaultFlags,
                  const wxString & sRemainingLabelText = wxEmptyString);

   using MessageColumn = std::vector< wxString >;
   using MessageTable = std::vector< MessageColumn >;

protected:
   // Display a table of messages.
   // Each member of the table is a column of messages.
   // Each member of a column is a string that should have no newlines,
   // and each column should have the same number of elements, to make
   // proper correspondences, but this is not checked.
   ProgressDialog(const wxString & title,
                  const MessageTable & columns,
                  int flags = pdlgDefaultFlags,
                  const wxString & sRemainingLabelText = wxEmptyString);

public:
   virtual ~ProgressDialog();

   bool Create(const wxString & title,
               const wxString & message = wxEmptyString,
               int flags = pdlgDefaultFlags,
               const wxString & sRemainingLabelText = wxEmptyString);

   void Reinit();

protected:
   bool Create(const wxString & title,
               const MessageTable & columns,
               int flags = pdlgDefaultFlags,
               const wxString & sRemainingLabelText = wxEmptyString);

public:
   ProgressResult Update(int value, const wxString & message = wxEmptyString);
   ProgressResult Update(double current, const wxString & message = wxEmptyString);
   ProgressResult Update(double current, double total, const wxString & message = wxEmptyString);
   ProgressResult Update(wxULongLong_t current, wxULongLong_t total, const wxString & message = wxEmptyString);
   ProgressResult Update(wxLongLong current, wxLongLong total, const wxString & message = wxEmptyString);
   ProgressResult Update(wxLongLong_t current, wxLongLong_t total, const wxString & message = wxEmptyString);
   ProgressResult Update(int current, int total, const wxString & message = wxEmptyString);
   void SetMessage(const wxString & message);

protected:
   wxWeakRef<wxWindow> mHadFocus;

   wxStaticText *mElapsed;
   wxStaticText *mRemaining;
   wxGauge *mGauge;

   wxLongLong_t mStartTime;
   wxLongLong_t mLastUpdate;
   wxLongLong_t mYieldTimer;
   int mLastValue; // gauge value, range = [0,1000]

   bool mCancel;
   bool mStop;

   bool mIsTransparent;

   // MY: Booleans to hold the flag values
   bool m_bShowElapsedTime = true;
   bool m_bConfirmAction = false;

private:
   void Init();
   bool SearchForWindow(const wxWindowList & list, const wxWindow *searchfor) const;
   void OnCancel(wxCommandEvent & e);
   void OnStop(wxCommandEvent & e);
   void OnCloseWindow(wxCloseEvent & e);
   void Beep() const;
   
   bool ConfirmAction(const wxString & sPrompt,
                      const wxString & sTitle,
                      int iButtonID = -1);

   void AddMessageAsColumn(wxBoxSizer * pSizer,
                           const MessageColumn &column, bool bFirstColumn);

private:
   // This guarantees we have an active event loop...possible during OnInit()
   wxEventLoopGuarantor mLoop;

   std::unique_ptr<wxWindowDisabler> mDisable;

   wxStaticText *mMessage{} ;
   int mLastW{ 0 };
   int mLastH{ 0 };

   DECLARE_EVENT_TABLE()
};

class AUDACITY_DLL_API TimerProgressDialog final : public ProgressDialog
{
public:
   TimerProgressDialog(const wxLongLong_t duration,
                       const wxString &title,
                       const MessageTable & columns,
                       int flags = pdlgDefaultFlags,
                       const wxString & sRemainingLabelText = wxEmptyString);

   // Oh no, there is an inherited nullary "Update" in wxDialog!
   // Choose another name then...
   ProgressResult UpdateProgress();

protected:
   wxLongLong_t mDuration;

   // Disallow direct use of the inherited overloads of Update because it
   // doesn't support changes of message
   using ProgressDialog::Update;
};

#endif
