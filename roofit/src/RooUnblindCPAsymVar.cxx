#include "BaBar/BaBar.hh"
/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id: RooUnblindCPAsymVar.cc,v 1.11 2004/08/07 06:45:41 bartoldu Exp $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2004, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

// -- CLASS DESCRIPTION [REAL] --
//
// Implementation of BlindTools' CP asymmetry blinding method
// A RooUnblindCPAsymVar object is a real valued function
// object, constructed from a blind value holder and a 
// set of unblinding parameters. When supplied to a PDF
// in lieu of a regular parameter, the blind value holder
// supplied to the unblinder objects will in a fit be minimized 
// to blind value corresponding to the actual minimum of the
// parameter. The transformation is chosen such that the
// the error on the blind parameters is indentical to that
// of the unblind parameter

#include "RooFitCore/RooArgSet.hh"
#include "RooFitModels/RooUnblindCPAsymVar.hh"


ClassImp(RooUnblindCPAsymVar)
;



RooUnblindCPAsymVar::RooUnblindCPAsymVar() : _blindEngine("") 
{
  // Default constructor
}


RooUnblindCPAsymVar::RooUnblindCPAsymVar(const char *name, const char *title,
					 const char *blindString, RooAbsReal& cpasym)
  : RooAbsHiddenReal(name,title), 
  _asym("asym","CP Asymmetry",this,cpasym),
  _blindEngine(blindString)
{  
  // Constructor from a given RooAbsReal (to hold the blind value) and a set of blinding parameters
}


RooUnblindCPAsymVar::RooUnblindCPAsymVar(const char *name, const char *title,
					 const char *blindString, RooAbsReal& cpasym, RooAbsCategory& blindState)
  : RooAbsHiddenReal(name,title,blindState), 
  _asym("asym","CP Asymmetry",this,cpasym),
  _blindEngine(blindString)
{  
  // Constructor from a given RooAbsReal (to hold the blind value) and a set of blinding parameters
}


RooUnblindCPAsymVar::RooUnblindCPAsymVar(const RooUnblindCPAsymVar& other, const char* name) : 
  RooAbsHiddenReal(other, name), 
  _asym("asym",this,other._asym),
  _blindEngine(other._blindEngine) 
{
  // Copy constructor
}


RooUnblindCPAsymVar::~RooUnblindCPAsymVar() 
{
  // Copy constructor
}


Double_t RooUnblindCPAsymVar::evaluate() const
{
  if (isHidden()) {
    // Blinding active for this event
    return _blindEngine.UnHideAsym(_asym);
  } else {
    // Blinding not active for this event
    return _asym ;
  }
}
