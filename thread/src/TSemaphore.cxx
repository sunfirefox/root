// @(#)root/thread:$Name:  $:$Id: TSemaphore.cxx,v 1.3 2005/12/09 15:12:19 rdm Exp $
// Author: Fons Rademakers   02/07/97

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSemaphore                                                           //
//                                                                      //
// This class implements a counting semaphore. Use a semaphore          //
// to synchronize threads.                                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TSemaphore.h"

ClassImp(TSemaphore)

//______________________________________________________________________________
TSemaphore::TSemaphore(UInt_t initial) : fCond(&fMutex)
{
   // Create counting semaphore.

   fValue = initial;
}

//______________________________________________________________________________
TSemaphore::TSemaphore(const TSemaphore &s) : TObject(s)
{
   // TSemaphore copy ctor.
}

//______________________________________________________________________________
TSemaphore& TSemaphore::operator=(const TSemaphore&)
{
   // TSemaphore assignment operator.
   return *this;
}

//______________________________________________________________________________
Int_t TSemaphore::Wait(Int_t millisec)
{
   // If semaphore value is > 0 then decrement it and carry on. If it's
   // already 0 then block. If millisec > 0, apply a relative timeout
   // of millisec milliseconds.

   Int_t rc = 0;

   if ((rc = fMutex.Lock())) {
      Error("Wait","Lock returns %d [%ld]", rc, TThread::SelfId());
      return rc;
   }

   while (fValue == 0) {

      int crc = (millisec > 0) ? fCond.TimedWaitRelative(millisec)
                               : fCond.Wait();

      if (crc != 0) {
         Error("Wait", "TCondition::Wait() returns %d [%ld]",
               crc, TThread::SelfId());
         if ((rc = fMutex.UnLock()))
            Error("Wait", "UnLock on error returns %d [%ld]",
                  rc, TThread::SelfId());
         return crc;
      }
   }

   fValue--;

   if ((rc = fMutex.UnLock())) {
      Error("Wait", "UnLock returns %d [%ld]", rc, TThread::SelfId());
      return rc;
   }

   return 0;
}

//______________________________________________________________________________
Int_t TSemaphore::TryWait()
{
   // If semaphore value is > 0 then decrement it and return 0. If it's
   // already 0 then return 1.

   int r = fMutex.Lock();
   if (r) { Error("TryWait","Lock returns %d [%ld]", r, TThread::SelfId()); return r; }

   if (fValue == 0) {
      r = fMutex.UnLock();
      if (r) Error("TryWait","UnLock on fail returns %d [%ld]", r, TThread::SelfId());
      return 1;
   }

   fValue--;

   r = fMutex.UnLock();
   if (r) { Error("TryWait","UnLock returns %d [%ld]", r, TThread::SelfId()); return r; }

   return 0;
}

//______________________________________________________________________________
Int_t TSemaphore::Post()
{
   // If any threads are blocked in Wait(), wake one of them up. Otherwise
   // increment the value of the semaphore.

   int r = fMutex.Lock();
   if (r) { Error("Post","Lock returns %d [%ld]", r, TThread::SelfId()); return r; }

   Bool_t doSignal = fValue == 0;
   fValue++;

   r = fMutex.UnLock();
   if (r) { Error("Post","UnLock returns %d [%ld]", r, TThread::SelfId()); return r; }

   if (doSignal) fCond.Signal();

   return 0;
}
