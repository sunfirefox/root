// @(#)root/net:$Name:  $:$Id: TGrid.cxx,v 1.2 2002/05/28 16:41:46 rdm Exp $
// Author: Fons Rademakers   3/1/2002

/*************************************************************************
 * Copyright (C) 1995-2002, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGrid                                                                //
//                                                                      //
// Abstract base class defining interface to common GRID services.      //
//                                                                      //
// To open a connection to a GRID use the static method Connect().      //
// The argument of Connect() is of the form:                            //
//    <grid>[://<host>][:<port>], e.g.                                  //
// alien, alien://alice.cern.ch, globus://glsvr1.cern.ch, ...           //
// Depending on the <grid> specified an appropriate plugin library      //
// will be loaded which will provide the real interface.                //
//                                                                      //
// Related classes are TGridResult.                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TGrid.h"
#include "TGridResult.h"
#include "TROOT.h"
#include "TPluginManager.h"


ClassImp(TGrid)

//______________________________________________________________________________
TGrid *TGrid::Connect(const char *grid, const char *uid, const char *pw)
{
   // The grid should be of the form:  <grid>://<host>[:<port>],
   // e.g.:  alien://alice.cern.ch, globus://glsrv1.cern.ch, ...
   // The uid is the username and pw the password that should be used for
   // the connection. Depending on the <grid> the shared library (plugin)
   // for the selected system will be loaded. When the connection could not
   // be opened 0 is returned.

   TPluginHandler *h;
   TGrid *g = 0;

   if (!uid) uid = "";
   if (!pw)  pw = "";

   if ((h = gROOT->GetPluginManager()->FindHandler("TGrid", grid))) {
      if (h->LoadPlugin() == -1)
         return 0;
      g = (TGrid *) gROOT->ProcessLineFast(Form(
           "new %s(\"%s\", \"%s\", \"%s\")", h->GetClass(), grid, uid, pw));
   }

   return g;
}

//______________________________________________________________________________
void TGrid::pwd() const
{
   // Print current working directory.

   const char *p = Pwd();
   if (p && strlen(p) > 0)
      printf("%s\n", p);
}

//______________________________________________________________________________
void TGrid::cd(const char *dir) const
{
   // Change working directory.

   Cd(dir);
}

//______________________________________________________________________________
void TGrid::ls(const char *dir, const char *options) const
{
   // List content of current directory.
   // Supported options:
   //  "l": long listing format
   //  "a": list all entries
   //  "d": list only directories

   TGridResult *r = Ls(dir, options);

   if (r) {
      const char *item;
      while ((item = r->Next())) {
         printf("%s\n", item);
      }
      delete r;
   }
}
