/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 *    File: $Id$
 * Authors:                                                                  *
 *   MB, Max Baak,   Nikhef,        mbaak@nikhef.nl                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#include <iostream>

#include "TMath.h"
#include "TMatrixDSymEigen.h"
#include "RooKeysPdf.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooRandom.h"
#include "RooDataSet.h"
#include "RooHist.h"

using namespace std;

ClassImp(RooKeysPdf)


RooKeysPdf::RooKeysPdf(const char *name, const char *title,
			   const RooArgList& varList, RooDataSet& data,
			   TString options, Double_t rho, Double_t nSigma,
			   RooAbsReal& weight) :
  RooAbsPdf(name,title),
  _varList("varList","List of variables",this),
  _data(data),
  _options(options),
  _widthFactor(rho),
  _nSigma(nSigma),
  _weight("weight","weight formula",this,weight,kFALSE,kFALSE),
  _weightParams("weightParams","weight parameters",this),
  _weightDep(0),
  _weights(&_weights0)
{

  // Constructor
  _varItr    = _varList.createIterator() ;
  _weightItr = _weightParams.createIterator() ;

  TIterator* varItr = varList.createIterator() ;
  RooAbsArg* var ;
  for (Int_t i=0; (var = (RooAbsArg*)varItr->Next()); ++i) {
    if (!dynamic_cast<RooAbsReal*>(var)) {
      cout << "RooKeysPdf::ctor(" << GetName() << ") ERROR: variable " << var->GetName() 
           << " is not of type RooAbsReal" << endl ;
      assert(0) ;
    }
    _varList.add(*var) ;
    _varName.push_back(var->GetName());
  }
  delete varItr ;

  // Add parameters of weight function to weightParams 
  RooArgSet* wgtParams = (RooArgSet*) weight.getParameters(_data) ;
  _weightParams.add(*wgtParams) ;
  delete wgtParams ;

  createPdf();
}


RooKeysPdf::RooKeysPdf(const char *name, const char *title,
                           RooAbsReal& x, RooDataSet& data,
                           Mirror mirror, Double_t rho, Double_t nSigma,
			   RooAbsReal& weight) :
  RooAbsPdf(name,title),
  _varList("varList","List of variables",this),
  _data(data),
  _widthFactor(rho),
  _nSigma(nSigma), 
  _weight("weight","weight formula",this,weight,kFALSE,kFALSE),
  _weightParams("weightParams","weight parameters",this),
  _weightDep(0),
  _weights(&_weights0)
{ 
  // Constructor
  _varItr = _varList.createIterator() ;
  _weightItr = _weightParams.createIterator() ;
  
  _varList.add(x) ;
  _varName.push_back(x.GetName());

  if (mirror!=NoMirror) {
    if (mirror!=MirrorBoth) 
      cout << "RooKeysPdf::RooKeysPdf() : Warning : asymmetric mirror(s) no longer supported." 
	   << endl;
    _options="m";
  }

  // Add parameters of weight function to weightParams 
  RooArgSet* wgtParams = (RooArgSet*) weight.getParameters(_data) ;
  _weightParams.add(*wgtParams) ;
  delete wgtParams ;

  createPdf();
}


RooKeysPdf::RooKeysPdf(const char *name, const char *title, RooAbsReal& x, RooAbsReal & y,
                           RooDataSet& data, TString options, Double_t rho, Double_t nSigma,
			   RooAbsReal& weight) :
  RooAbsPdf(name,title),
  _varList("varList","List of variables",this),
  _data(data),
  _options(options),
  _widthFactor(rho),
  _nSigma(nSigma),
  _weight("weight","weight formula",this,weight,kFALSE,kFALSE),
  _weightParams("weightParams","weight parameters",this),
  _weightDep(0),
  _weights(&_weights0)
{ 
  // Constructor
  _varItr = _varList.createIterator() ;
  _weightItr = _weightParams.createIterator() ;

  _varList.add(RooArgSet(x,y)) ;
  _varName.push_back(x.GetName());
  _varName.push_back(y.GetName());

  // Add parameters of weight function to weightParams 
  RooArgSet* wgtParams = (RooArgSet*) weight.getParameters(_data) ;
  _weightParams.add(*wgtParams) ;
  delete wgtParams ;

  createPdf();
}


RooKeysPdf::RooKeysPdf(const RooKeysPdf& other, const char* name) :
  RooAbsPdf(other,name), 
  _varList("varList",this,other._varList),
  _data(other._data),
  _options(other._options),
  _widthFactor(other._widthFactor),
  _nSigma(other._nSigma),
  _weight("weight",this,other._weight),
  _weightParams("weightParams",this,other._weightParams),
  _weightDep(0),
  _weights(&_weights0)
{
  // Constructor
  _varItr      = _varList.createIterator() ;
  _weightItr   = _weightParams.createIterator() ;

  _fixedShape  = other._fixedShape;
  _mirror      = other._mirror;
  _debug       = other._debug;
  _verbose     = other._verbose;
  _sqrt2pi     = other._sqrt2pi;
  _nDim        = other._nDim;
  _nEvents     = other._nEvents;
  _nEventsM    = other._nEventsM;
  _nEventsW    = other._nEventsW;
  _d           = other._d;
  _n           = other._n;
  _dataPts     = other._dataPts;
  _dataPtsR    = other._dataPtsR;
  _weights0    = other._weights0;
  _weights1    = other._weights1;
  if (_options.Contains("a")) { _weights = &_weights1; }
  //_sortIdcs    = other._sortIdcs;
  _sortTVIdcs  = other._sortTVIdcs;
  _varName     = other._varName;
  _rho         = other._rho;
  _x           = other._x;
  _x0          = other._x0 ;
  _x1          = other._x1 ;
  _x2          = other._x2 ;
  _xDatLo      = other._xDatLo;
  _xDatHi      = other._xDatHi;
  _xDatLo3s    = other._xDatLo3s;
  _xDatHi3s    = other._xDatHi3s;
  _mean        = other._mean;
  _sigma       = other._sigma;

  // BoxInfo
  _netFluxZ    = other._netFluxZ;
  _nEventsBW   = other._nEventsBW;
  _nEventsBMSW = other._nEventsBMSW;
  _xVarLo      = other._xVarLo;
  _xVarHi      = other._xVarHi;
  _xVarLoM3s   = other._xVarLoM3s;
  _xVarLoP3s   = other._xVarLoP3s;
  _xVarHiM3s   = other._xVarHiM3s;
  _xVarHiP3s   = other._xVarHiP3s;
  _bpsIdcs     = other._bpsIdcs;
  _sIdcs       = other._sIdcs;
  _bIdcs       = other._bIdcs;
  _bmsIdcs     = other._bmsIdcs;

  _rangeBoxInfo= other._rangeBoxInfo ;
  _fullBoxInfo = other._fullBoxInfo ;

  _idx         = other._idx;
  _minWeight   = other._minWeight;
  _maxWeight   = other._maxWeight;
  _wMap        = other._wMap;

  _covMat      = new TMatrixDSym(*other._covMat);
  _corrMat     = new TMatrixDSym(*other._corrMat);
  _rotMat      = new TMatrixD(*other._rotMat);
  _sigmaR      = new TVectorD(*other._sigmaR);
  _dx          = new TVectorD(*other._dx);
  _sigmaAvgR   = other._sigmaAvgR;
}


RooKeysPdf::~RooKeysPdf() 
{
  if (_varItr)    delete _varItr;
  if (_weightItr) delete _weightItr;
  if (_covMat)    delete _covMat;
  if (_corrMat)   delete _corrMat;
  if (_rotMat)    delete _rotMat;
  if (_sigmaR)    delete _sigmaR;
  if (_dx)        delete _dx;
  if (_weightDep) delete _weightDep ;

  // delete all the boxinfos map
  while ( !_rangeBoxInfo.empty() ) {
    map<pair<string,int>,BoxInfo*>::iterator iter = _rangeBoxInfo.begin();
    BoxInfo* box= (*iter).second;
    _rangeBoxInfo.erase(iter);
    delete box;
  }

  _dataPts.clear();
  _dataPtsR.clear();
  _weights0.clear();
  _weights1.clear();
  //_sortIdcs.clear();
  _sortTVIdcs.clear();
}


void
RooKeysPdf::createPdf(Bool_t firstCall) const
{
  // evaluation order of constructor.

  if (firstCall) {
    // set options
    setOptions();
    // initialization
    initialize();
  }

  // copy dataset, calculate sigma_i's, determine min and max event weight
  loadDataSet(firstCall);
  // mirror dataset around dataset boundaries -- does not depend on event weights
  if (_mirror) mirrorDataSet();
  // store indices and weights of events with high enough weights
  loadWeightSet();

  // store indices of events in variable boundaries and box shell.
//calculateShell(&_fullBoxInfo);
  // calculate normalization needed in analyticalIntegral()
//calculatePreNorm(&_fullBoxInfo);

  // lookup table for determining which events contribute to a certain coordinate
  sortDataIndices();

  // determine static and/or adaptive bandwidth
  calculateBandWidth();
}


void 
RooKeysPdf::setOptions() const
{
  // set the configuration

  _options.ToLower(); 

  if( _options.Contains("a") ) { _weights = &_weights1; }
  else                         { _weights = &_weights0; }
  if( _options.Contains("m") )   _mirror = true;
  else                           _mirror = false;
  if( _options.Contains("d") )   _debug  = true;
  else                           _debug  = false;
  if( _options.Contains("v") ) { _debug = true;  _verbose = true; } 
  else                         { _debug = false; _verbose = false; }

  if( _debug || _verbose ) {
    // coutD("InputOptions") << 
    cout << "RooKeysPdf::setOptions()    options = " << _options 
	 << "\n\tbandWidthType    = " << _options.Contains("a")    
	 << "\n\tmirror           = " << _mirror
	 << "\n\tdebug            = " << _debug            
	 << "\n\tverbose          = " << _verbose          
	 << endl;
  }

  if (_nSigma<2.0) {
    cout << "RooKeysPdf::setOptions() : Warning : nSigma = " << _nSigma << " < 2.0. "
	 << "Calculated normalization could be too large." 
	 << endl;
  }
}


void
RooKeysPdf::initialize() const
{
  // initialization
  _sqrt2pi   = sqrt(2.0*TMath::Pi()) ;
  _nDim      = _varList.getSize();
  _nEvents   = (Int_t)_data.numEntries();
  _nEventsM  = _nEvents;
  _fixedShape= kFALSE;

  if(_nDim==0) {
    cout << "ERROR:  RooKeysPdf::initialize() : The observable list is empty. "
	 << "Unable to begin generating the PDF." << endl;
    assert (_nDim!=0);
  }

  if(_nEvents==0) {
    cout << "ERROR:  RooKeysPdf::initialize() : The input data set is empty. "
	 << "Unable to begin generating the PDF." << endl;
    assert (_nEvents!=0);
  }

  _d         = static_cast<Double_t>(_nDim);

  vector<Double_t> dummy(_nDim,0.);
  _dataPts.resize(_nEvents,dummy);
  _weights0.resize(_nEvents,dummy);
  //_sortIdcs.resize(_nDim);
  _sortTVIdcs.resize(_nDim);

  _rho.resize(_nDim,_widthFactor);

  _x.resize(_nDim,0.);
  _x0.resize(_nDim,0.);
  _x1.resize(_nDim,0.);
  _x2.resize(_nDim,0.);

  _mean.resize(_nDim,0.);
  _sigma.resize(_nDim,0.);

  _xDatLo.resize(_nDim,0.);
  _xDatHi.resize(_nDim,0.);
  _xDatLo3s.resize(_nDim,0.);
  _xDatHi3s.resize(_nDim,0.);

  boxInfoInit(&_fullBoxInfo,0,0xFFFF);

  _minWeight=0;
  _maxWeight=0;
  _wMap.clear();

  _covMat = 0;
  _corrMat= 0;
  _rotMat = 0;
  _sigmaR = 0;
  _dx = new TVectorD(_nDim); _dx->Zero();
  _dataPtsR.resize(_nEvents,*_dx);

  _varItr->Reset() ;
  RooRealVar* var ;
  for(Int_t j=0; (var=(RooRealVar*)_varItr->Next()); ++j) {
    _xDatLo[j] = var->getMin();
    _xDatHi[j] = var->getMax();
  }
}


void
RooKeysPdf::loadDataSet(Bool_t firstCall) const
{
  // copy the dataset and calculate some useful variables

  // first some initialization
  _nEventsW=0.;
  TMatrixD mat(_nDim,_nDim); 
  if (!_covMat)  _covMat = new TMatrixDSym(_nDim);    
  if (!_corrMat) _corrMat= new TMatrixDSym(_nDim);    
  if (!_rotMat)  _rotMat = new TMatrixD(_nDim,_nDim); 
  if (!_sigmaR)  _sigmaR = new TVectorD(_nDim);       
  mat.Zero();
  _covMat->Zero();
  _corrMat->Zero();
  _rotMat->Zero();
  _sigmaR->Zero();

  const RooArgSet* values= _data.get();  
  vector<RooRealVar*> dVars(_nDim);
  for  (Int_t j=0; j<_nDim; j++) {
    dVars[j] = (RooRealVar*)values->find(_varName[j].c_str());
    _x0[j]=_x1[j]=_x2[j]=0.;
  }

  RooArgSet* weightObs = _weight.arg().getObservables(_data) ;
  Int_t nWObs = weightObs->getSize();

  _idx.clear();
  for (Int_t i=0; i<_nEvents; i++) {
    _data.get(i); // fills dVars
    _idx.push_back(i);
    vector<Double_t>& point  = _dataPts[i];
    TVectorD& pointV = _dataPtsR[i];

    // update weight if weight depends on observables of event
    if ( nWObs>0 ) { *weightObs = *values; } // update _weight
    if ( fabs(_weight)>_maxWeight ) { _maxWeight = fabs(_weight); }
    _nEventsW += _weight;

    //cout << "Weight : " << _weight << endl;

    for (Int_t j=0; j<_nDim; j++) {
      for (Int_t k=0; k<_nDim; k++) 
	mat(j,k) += dVars[j]->getVal() * dVars[k]->getVal() * _weight;

      // only need to do once
      if (firstCall) 
	point[j] = pointV[j] = dVars[j]->getVal();

      _x0[j] += 1. * _weight; 
      _x1[j] += point[j] * _weight ; 
      _x2[j] += point[j] * point[j] * _weight ;

      // only need to do once
      if (firstCall) {
	if (point[j]<_xDatLo[j]) { _xDatLo[j]=point[j]; }
	if (point[j]>_xDatHi[j]) { _xDatHi[j]=point[j]; }
      }
    }
  }

  _n = TMath::Power(4./(_nEventsW*(_d+2.)), 1./(_d+4.)) ; 
  // = (4/[n(dim(R) + 2)])^1/(dim(R)+4); dim(R) = 2
  _minWeight = (0.5 - TMath::Erf(_nSigma/sqrt(2.))/2.) * _maxWeight;

  for (Int_t j=0; j<_nDim; j++) {
    _mean[j]  = _x1[j]/_x0[j];
    _sigma[j] = sqrt(_x2[j]/_x0[j]-_mean[j]*_mean[j]);
  }

  for (Int_t j=0; j<_nDim; j++) {
    for (Int_t k=0; k<_nDim; k++) 
      (*_covMat)(j,k) = mat(j,k)/_x0[j] - _mean[j]*_mean[k] ;
  }

  // find decorrelation matrix and eigenvalues (R)
  TMatrixDSymEigen evCalculator(*_covMat);
  *_rotMat = evCalculator.GetEigenVectors();
  *_rotMat = _rotMat->T(); // transpose
  *_sigmaR = evCalculator.GetEigenValues();
  for (Int_t j=0; j<_nDim; j++) { (*_sigmaR)[j] = sqrt((*_sigmaR)[j]); }

  for (Int_t j=0; j<_nDim; j++) {
    for (Int_t k=0; k<_nDim; k++) 
      (*_corrMat)(j,k) = (*_covMat)(j,k)/(_sigma[j]*_sigma[k]) ;
  }


  if (_verbose) {
    //_covMat->Print();
    _rotMat->Print();
    _corrMat->Print();
    _sigmaR->Print();
  }

  _sigmaAvgR=1.;
  for (Int_t j=0; j<_nDim; j++) { _sigmaAvgR *= (*_sigmaR)[j]; }
  _sigmaAvgR = TMath::Power(_sigmaAvgR, 1./_d) ;

  for (Int_t i=0; i<_nEvents; i++) {
    TVectorD& pointR = _dataPtsR[i];
    pointR *= *_rotMat;
  }

  if (_verbose)
    cout << "RooKeysPdf::loadDataSet(" << this << ")" 
	 << "\n Number of events in dataset: " << _nEvents 
	 << "\n Weighted number of events in dataset: " << _nEventsW 
	 << endl; 
}


void
RooKeysPdf::mirrorDataSet() const
{
  // determine mirror dataset.
  // mirror points are added around the physical boundaries of the dataset
  // Two steps:
  // 1. For each entry, determine if it should be mirrored (the mirror configuration).
  // 2. For each mirror configuration, make the mirror points.

  for (Int_t j=0; j<_nDim; j++) {
    _xDatLo3s[j] = _xDatLo[j] + _nSigma * (_rho[j] * _n * _sigma[j]);
    _xDatHi3s[j] = _xDatHi[j] - _nSigma * (_rho[j] * _n * _sigma[j]);
  }

  vector<Double_t> dummy(_nDim,0.);
  
  // 1.
  for (Int_t i=0; i<_nEvents; i++) {    
    vector<Double_t>& x = _dataPts[i];
    
    Int_t size = 1;
    vector<vector<Double_t> > mpoints(size,dummy);
    vector<vector<Int_t> > mjdcs(size);
    
    // create all mirror configurations for event i
    for (Int_t j=0; j<_nDim; j++) {
      
      vector<Int_t>& mjdxK = mjdcs[0];
      vector<Double_t>& mpointK = mpoints[0];
      
      // single mirror *at physical boundaries*
      if ((x[j]>_xDatLo[j] && x[j]<_xDatLo3s[j]) && x[j]<(_xDatLo[j]+_xDatHi[j])/2.) { 
	mpointK[j] = 2.*_xDatLo[j]-x[j]; 
	mjdxK.push_back(j);
      } else if ((x[j]<_xDatHi[j] && x[j]>_xDatHi3s[j]) && x[j]>(_xDatLo[j]+_xDatHi[j])/2.) { 
	mpointK[j] = 2.*_xDatHi[j]-x[j]; 
	mjdxK.push_back(j);
      }
    }
    
    vector<Int_t>& mjdx0 = mjdcs[0];
    // no mirror point(s) for this event
    if (size==1 && mjdx0.size()==0) continue;
    
    // 2.
    // generate all mirror points for event i
    vector<Int_t>& mjdx = mjdcs[0];
    vector<Double_t>& mpoint = mpoints[0];
    
    // number of mirror points for this mirror configuration
    Int_t eMir = 1 << mjdx.size(); 
    vector<vector<Double_t> > epoints(eMir,x);
    
    for (Int_t m=0; m<Int_t(mjdx.size()); m++) {
      Int_t size1 = 1 << m;
      Int_t size2 = 1 << m+1;
      // copy all previous mirror points
      for (Int_t l=size1; l<size2; ++l) { 
	epoints[l] = epoints[l-size1];
	// fill high mirror points
	vector<Double_t>& epoint = epoints[l];
	epoint[mjdx[Int_t(mjdx.size()-1)-m]] = mpoint[mjdx[Int_t(mjdx.size()-1)-m]];
      }
    }
    
    // remove duplicate mirror points
    // note that: first epoint == x
    epoints.erase(epoints.begin());

    // add mirror points of event i to total dataset
    TVectorD pointR(_nDim); 

    for (Int_t m=0; m<Int_t(epoints.size()); m++) {
      _idx.push_back(i);
      _dataPts.push_back(epoints[m]);
      //_weights0.push_back(_weights0[i]);
      for (Int_t j=0; j<_nDim; j++) { pointR[j] = (epoints[m])[j]; }
      _dataPtsR.push_back(pointR);
    }
    
    epoints.clear();
    mpoints.clear();
    mjdcs.clear();
  } // end of event loop

  _nEventsM = Int_t(_dataPts.size());
}


void
RooKeysPdf::loadWeightSet() const
{
  _wMap.clear();
  const RooArgSet* values= _data.get();  

  RooArgSet* weightObs = _weight.arg().getObservables(_data) ;

  Int_t nWObs =  weightObs->getSize();

  for (Int_t i=0; i<_nEventsM; i++) {
    _data.get(_idx[i]);
    if ( nWObs>0 ) { *weightObs = *values; } // update _weight
    if ( fabs(_weight)>_minWeight ) { 
      //cout << "weight : " << _weight << endl;
      _wMap[i] = _weight; 
    }
  }

  if (_verbose) { 
    cout << "RooKeysPdf::loadWeightSet(" << this << ") : Number of weighted events : " << _wMap.size() << endl;
  }
}


void
RooKeysPdf::calculateShell(BoxInfo* bi) const 
{
  // determine points in +/- nSigma shell around the box determined by the variable
  // ranges. These points are needed in the normalization, to determine probability
  // leakage in and out of the box.

  //bi->netFluxZ = kTRUE;
  //bi->bpsIdcs.clear();
  //bi->sIdcs.clear();
  //bi->bmsIdcs.clear();

  for (Int_t j=0; j<_nDim; j++) {
    if (bi->xVarLo[j]==_xDatLo[j] && bi->xVarHi[j]==_xDatHi[j]) { 
      //cout << bi->xVarLo[j] << " " << _xDatLo[j] << " " << bi->xVarHi[j] << " " << _xDatHi[j] << endl;
      bi->netFluxZ = bi->netFluxZ && kTRUE; 
    } else { bi->netFluxZ = kFALSE; }

    bi->xVarLoM3s[j] = bi->xVarLo[j] - _nSigma * (_rho[j] * _n * _sigma[j]);
    bi->xVarLoP3s[j] = bi->xVarLo[j] + _nSigma * (_rho[j] * _n * _sigma[j]);
    bi->xVarHiM3s[j] = bi->xVarHi[j] - _nSigma * (_rho[j] * _n * _sigma[j]);
    bi->xVarHiP3s[j] = bi->xVarHi[j] + _nSigma * (_rho[j] * _n * _sigma[j]);
  }

  map<Int_t,Double_t>::iterator wMapItr = _wMap.begin();

  //for (Int_t i=0; i<_nEventsM; i++) {    
  for (; wMapItr!=_wMap.end(); ++wMapItr) {
    Int_t i = (*wMapItr).first;

    const vector<Double_t>& x = _dataPts[i];
    Bool_t inVarRange(kTRUE);
    Bool_t inVarRangePlusShell(kTRUE);

    for (Int_t j=0; j<_nDim; j++) {

      if (x[j]>bi->xVarLo[j] && x[j]<bi->xVarHi[j]) { 
	inVarRange = inVarRange && kTRUE; 
      } else { inVarRange = inVarRange && kFALSE; }    

      if (x[j]>bi->xVarLoM3s[j] && x[j]<bi->xVarHiP3s[j]) { 
	inVarRangePlusShell = inVarRangePlusShell && kTRUE;
      } else { inVarRangePlusShell = inVarRangePlusShell && kFALSE; }
    }

    // event in range?
    if (inVarRange) { 
      bi->bIdcs.push_back(i);
    }

    // event in shell?
    if (inVarRangePlusShell) { 
      bi->bpsIdcs[i] = kTRUE;
      Bool_t inShell(kFALSE);      
      for (Int_t j=0; j<_nDim; j++) {
	if ((x[j]>bi->xVarLoM3s[j] && x[j]<bi->xVarLoP3s[j]) && x[j]<(bi->xVarLo[j]+bi->xVarHi[j])/2.) { 
	  inShell = kTRUE;
	} else if ((x[j]>bi->xVarHiM3s[j] && x[j]<bi->xVarHiP3s[j]) && x[j]>(bi->xVarLo[j]+bi->xVarHi[j])/2.) { 
	  inShell = kTRUE;
	}
      }
      if (inShell) bi->sIdcs.push_back(i); // needed for normalization
      else { 
	bi->bmsIdcs.push_back(i);          // idem
      }
    }
  }

  if (_verbose)
    cout << "RooKeysPdf::calculateShell() : " 
	 << "\n Events in shell " << bi->sIdcs.size() 
	 << "\n Events in box " << bi->bIdcs.size() 
	 << "\n Events in box and shell " << bi->bpsIdcs.size() 
	 << endl;
}


void
RooKeysPdf::calculatePreNorm(BoxInfo* bi) const
{
  //bi->nEventsBMSW=0.;
  //bi->nEventsBW=0.;

  // box minus shell
  for (Int_t i=0; i<Int_t(bi->bmsIdcs.size()); i++) 
    bi->nEventsBMSW += _wMap[bi->bmsIdcs[i]];

  // box
  for (Int_t i=0; i<Int_t(bi->bIdcs.size()); i++) 
    bi->nEventsBW += _wMap[bi->bIdcs[i]];

  if (_verbose)
    cout << "RooKeysPdf::calculatePreNorm() : " 
	 << "\n nEventsBMSW " << bi->nEventsBMSW 
	 << "\n nEventsBW " << bi->nEventsBW 
	 << endl;
}


void
RooKeysPdf::sortDataIndices(BoxInfo* bi) const
{
  // sort entries, as needed for loopRange()

  /*
  iiVec itrVec;
  vector<vector<Double_t> >::iterator dpItr = _dataPts.begin();
  for (Int_t i=0; dpItr!=_dataPts.end(); ++dpItr, ++i) {
    if (bi) {
      if (bi->bpsIdcs.find(i)!=bi->bpsIdcs.end()) 
      //if (_wMap.find(i)!=_wMap.end()) 
	itrVec.push_back(iiPair(i,dpItr));
    } else itrVec.push_back(iiPair(i,dpItr));
  }

  for (Int_t j=0; j<_nDim; j++) { 
    _sortIdcs[j].clear();
    sort(itrVec.begin(),itrVec.end(),SorterL2H(j));
    _sortIdcs[j] = itrVec;
  }
  */

  itVec itrVecR;
  vector<TVectorD>::iterator dpRItr = _dataPtsR.begin();
  for (Int_t i=0; dpRItr!=_dataPtsR.end(); ++dpRItr, ++i) {
    if (bi) {
      if (bi->bpsIdcs.find(i)!=bi->bpsIdcs.end()) 
      //if (_wMap.find(i)!=_wMap.end()) 
	itrVecR.push_back(itPair(i,dpRItr));
    } else itrVecR.push_back(itPair(i,dpRItr));
  }

  for (Int_t j=0; j<_nDim; j++) { 
    _sortTVIdcs[j].clear();
    sort(itrVecR.begin(),itrVecR.end(),SorterTV_L2H(j));
    _sortTVIdcs[j] = itrVecR;
  }

  if (_verbose) {
    for (Int_t j=0; j<_nDim; j++) { 
      cout << "RooKeysPdf::sortDataIndices() : Number of sorted events : " << _sortTVIdcs[j].size() << endl; 
    }
  }
}


void
RooKeysPdf::calculateBandWidth() const
{
  if(_verbose) { cout << "RooKeysPdf::calculateBandWidth()" << endl; }

  // non-adaptive bandwidth 
  // (default, and needed to calculate adaptive bandwidth)

  if(!_options.Contains("a")) 
    if (_verbose)
      cout << "RooKeysPdf::calculateBandWidth() Using static bandwidth." << endl;

  for (Int_t i=0; i<_nEvents; i++) {
    //map<Int_t,Bool_t>::iterator bpsItr; 
    //for (bpsItr=bi->bpsIdcs.begin(); bpsItr!=bi->bpsIdcs.end(); ++bpsItr) {
    //Int_t i = (*bpsItr).first;
    //if (i>=_nEvents) continue;

    vector<Double_t>& weight = _weights0[i];
    for (Int_t j=0; j<_nDim; j++) { weight[j] = _rho[j] * _n * (*_sigmaR)[j]; }
  }

  // adaptive width
  if(_options.Contains("a")) {
    if (_verbose)
      cout << "RooKeysPdf::calculateBandWidth() Using adaptive bandwidth." << endl;

    vector<Double_t> dummy(_nDim,0.);
    //_weights1.clear();
    _weights1.resize(_nEvents,dummy);

    for(Int_t i=0; i<_nEvents; ++i) {
      //for (bpsItr=bi->bpsIdcs.begin(); bpsItr!=bi->bpsIdcs.end(); ++bpsItr) {
      //Int_t i = (*bpsItr).first;
      //if (i>=_nEvents) continue;

      vector<Double_t>& x = _dataPts[i];
      Double_t f =  TMath::Power( gauss(x,_weights0)/_nEventsW , -1./(2.*_d) ) ;

      vector<Double_t>& weight = _weights1[i];
      for (Int_t j=0; j<_nDim; j++) {
	Double_t norm = ((_rho[j]*_n*(*_sigmaR)[j])/sqrt(_sigmaAvgR)) ; 
	weight[j] = norm * f / sqrt(12.);
      }
    }
    _weights = &_weights1;
  } 
}


Double_t
RooKeysPdf::gauss(vector<Double_t>& x, vector<vector<Double_t> >& weights) const 
{
  // loop over all closest point to x, as determined by loopRange()

  if(_nEvents==0) return 0.;

  Double_t z=0.;
  map<Int_t,Bool_t> ibMap;
  ibMap.clear();

  // determine loop range for event x
  loopRange(x,ibMap);

  map<Int_t,Bool_t>::iterator ibMapItr = ibMap.begin();

  for (; ibMapItr!=ibMap.end(); ++ibMapItr) {
    Int_t i = (*ibMapItr).first;

    Double_t g(1.);

    const vector<Double_t>& point  = _dataPts[i];
    const vector<Double_t>& weight = weights[_idx[i]];

    for (Int_t j=0; j<_nDim; j++) { (*_dx)[j] = x[j]-point[j]; }
    *_dx *= *_rotMat; // rotate to decorrelated frame!

    for (Int_t j=0; j<_nDim; j++) {
      Double_t r = (*_dx)[j];  //x[j] - point[j];
      Double_t c = 1./(2.*weight[j]*weight[j]);

      g *= exp( -c*r*r );
      g *= 1./(_sqrt2pi*weight[j]);
      g *= _wMap[_idx[i]];
    }
    z += g;
  }

  return z;
}


void
RooKeysPdf::loopRange(vector<Double_t>& x, map<Int_t,Bool_t>& ibMap) const
{
  // determine closest points to x, to loop over in evaluate()
  /*
  vector<Double_t> xm(_nDim);
  vector<Double_t> xp(_nDim);

  for (Int_t j=0; j<_nDim; j++) {
    xRm[j] = xRp[j] = x[j];
    xm[j] = x[j] - _nSigma * (_rho[j] * _n * _sigma[j]);
    xp[j] = x[j] + _nSigma * (_rho[j] * _n * _sigma[j]);
  }
  vector<vector<Double_t> > xvecm(1,xm);
  vector<vector<Double_t> > xvecp(1,xp);
  */

  TVectorD xRm(_nDim);
  TVectorD xRp(_nDim);

  for (Int_t j=0; j<_nDim; j++) { xRm[j] = xRp[j] = x[j]; }

  xRm *= *_rotMat;
  xRp *= *_rotMat;
  for (Int_t j=0; j<_nDim; j++) {
    xRm[j] -= _nSigma * (_rho[j] * _n * (*_sigmaR)[j]);
    xRp[j] += _nSigma * (_rho[j] * _n * (*_sigmaR)[j]);
  }

  vector<TVectorD> xvecRm(1,xRm);
  vector<TVectorD> xvecRp(1,xRp);

  /*
  map<Int_t,Bool_t> ibMapT;

  for (Int_t j=0; j<_nDim; j++) {
    ibMap.clear();
    iiVec::iterator lo = lower_bound(_sortIdcs[j].begin(), _sortIdcs[j].end(),
				     iiPair(0,xvecm.begin()), SorterL2H(j));
    iiVec::iterator hi = upper_bound(_sortIdcs[j].begin(),_sortIdcs[j].end(),
				     iiPair(0,xvecp.begin()), SorterL2H(j));
    iiVec::iterator it=lo;
    if (j==0) {
      if (_nDim==1) { for (it=lo; it!=hi; ++it) ibMap[(*it).first] = kTRUE; }
      else { for (it=lo; it!=hi; ++it) ibMapT[(*it).first] = kTRUE; }
      continue;
    }

    for (it=lo; it!=hi; ++it) 
      if (ibMapT.find((*it).first)!=ibMapT.end()) { ibMap[(*it).first] = kTRUE; }

    ibMapT.clear();
    if (j!=_nDim-1) { ibMapT = ibMap; }
  }
  */


  map<Int_t,Bool_t> ibMapRT;

  for (Int_t j=0; j<_nDim; j++) {
    ibMap.clear();
    itVec::iterator lo = lower_bound(_sortTVIdcs[j].begin(), _sortTVIdcs[j].end(),
				     itPair(0,xvecRm.begin()), SorterTV_L2H(j));
    itVec::iterator hi = upper_bound(_sortTVIdcs[j].begin(), _sortTVIdcs[j].end(),
				     itPair(0,xvecRp.begin()), SorterTV_L2H(j));
    itVec::iterator it=lo;
    if (j==0) {
      if (_nDim==1) { for (it=lo; it!=hi; ++it) ibMap[(*it).first] = kTRUE; }
      else { for (it=lo; it!=hi; ++it) ibMapRT[(*it).first] = kTRUE; }
      continue;
    }

    for (it=lo; it!=hi; ++it) 
      if (ibMapRT.find((*it).first)!=ibMapRT.end()) { ibMap[(*it).first] = kTRUE; }

    ibMapRT.clear();
    if (j!=_nDim-1) { ibMapRT = ibMap; }
  }
}


void
RooKeysPdf::boxInfoInit(BoxInfo* bi, const char* rangeName, Int_t /*code*/) const
{
  vector<Bool_t> doInt(_nDim,kTRUE);
//   vector<Bool_t> doInt(_nDim,kFALSE);
//   for (Int_t j=0; j<_nDim; j++) {
//     Int_t shiftj = 1 << j;
//     if (code&shiftj) { 
//       doInt[j] = kTRUE; 
//     } 
//   }

  bi->filled = kFALSE;

  bi->xVarLo.resize(_nDim,0.);
  bi->xVarHi.resize(_nDim,0.);
  bi->xVarLoM3s.resize(_nDim,0.);
  bi->xVarLoP3s.resize(_nDim,0.);
  bi->xVarHiM3s.resize(_nDim,0.);
  bi->xVarHiP3s.resize(_nDim,0.);

  bi->netFluxZ = kTRUE;
  bi->bpsIdcs.clear();
  bi->bIdcs.clear();
  bi->sIdcs.clear();
  bi->bmsIdcs.clear();

  bi->nEventsBMSW=0.;
  bi->nEventsBW=0.;

  _varItr->Reset() ;
  RooRealVar* var ;
  for(Int_t j=0; (var=(RooRealVar*)_varItr->Next()); ++j) {
    if (doInt[j]) {
      bi->xVarLo[j] = var->getMin(rangeName);
      bi->xVarHi[j] = var->getMax(rangeName);
    } else {
      bi->xVarLo[j] = var->getVal() ;
      bi->xVarHi[j] = var->getVal() ;
    }
  }
}


Double_t 
RooKeysPdf::evaluate() const 
{
  if (!_weightDep) {
    TString name = Form("%s_params",GetName()) ;
    _weightDep = new RooFormulaVar(name,name,"1",_weightParams) ;
  }

  if (_weightDep->isValueDirty() && !_fixedShape) {
    cout << "RooKeysPdf::evaluate(" << GetName() << ") one of the weight parameters has changed, need to recalculate" << endl ;
    // Clear dirty flag
    _weightDep->getVal() ;
    // recalc pdf
    createPdf(kFALSE);
  }

  _varItr->Reset() ;
  RooAbsReal* var ;
  const RooArgSet* nset = _varList.nset() ;
  for(Int_t j=0; (var=(RooAbsReal*)_varItr->Next()); ++j) {    
    _x[j] = var->getVal(nset);
  }

  Double_t val = gauss(_x,*_weights);

  if (val>=1E-20)
    return val ;
  else 
    return (1E-20) ;
}


Int_t 
RooKeysPdf::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const
{
  cout << "RooKeysPdf::getAnalyticalIntegral allVars = " << allVars << endl ;

  Int_t code=0;

  if (rangeName) { code=0; }
  if (matchArgs(allVars,analVars,RooArgSet(_varList))) { code=1; }

  return code;

  /*
  analVars.add(allVars) ;

  Int_t code(0) ;
  TIterator* iter = allVars.createIterator() ;
  RooAbsArg* var ;
  while((var=(RooAbsArg*)iter->Next())) {
    Int_t index = _varList.index(var) ;
    if (index>=0) {
      cout << "code |= (1<<" << index << ")" << endl ;
      code |= (1<<index) ;
    }
  }
  delete iter ;

  cout << "code = " << code << endl ;
  return code ;
  */
}


Double_t 
RooKeysPdf::analyticalIntegral(Int_t code, const char* rangeName) const
{
  if (_verbose) {
    cout << "Calling RooKeysPdf::analyticalIntegral(" << GetName() << ") with code " << code 
	 << " and rangeName " << (rangeName?rangeName:"<none>") << endl;
  }

  // determine which observables need to be integrated over ...
  Int_t nComb = 1 << (_nDim); 
  assert(code>=1 && code<nComb) ;

  vector<Bool_t> doInt(_nDim,kTRUE);
//   vector<Bool_t> doInt(_nDim,kFALSE);
//   for (Int_t j=0; j<_nDim; j++) {
//     Int_t shiftj = 1 << j;
//     if (code&shiftj) { 
//       doInt[j] = kTRUE; 
//     } 
//   }

  // get BoxInfo
  BoxInfo* bi(0);

  if (rangeName) {
    bi = _rangeBoxInfo[make_pair(rangeName,code)] ;
    if (!bi) {
      bi = new BoxInfo ;
      _rangeBoxInfo[make_pair(rangeName,code)] = bi ;      
      boxInfoInit(bi,rangeName,code);
    }
  } else bi= &_fullBoxInfo ;

  // have boundaries changed?
  Bool_t newBounds(kFALSE);
  _varItr->Reset() ;
  RooRealVar* var ;
  for(Int_t j=0; (var=(RooRealVar*)_varItr->Next()); ++j) {
    if ((var->getMin(rangeName)-bi->xVarLo[j]!=0) ||
	(var->getMax(rangeName)-bi->xVarHi[j]!=0)) {
      newBounds = kTRUE;
    }
  }

  // reset
  if (newBounds) {
    cout << "RooKeysPdf::analyticalIntegral() : Found new boundaries ... " << (rangeName?rangeName:"<none>") << endl;
    boxInfoInit(bi,rangeName,code);    
  }

  // recalculates netFluxZero and nEventsIR
  if (!bi->filled || newBounds) {
    // Fill box info with contents
    calculateShell(bi);
    calculatePreNorm(bi);
    bi->filled = kTRUE;
    sortDataIndices(bi);    
  }

  // first guess
  Double_t norm=bi->nEventsBW;

  if (_mirror && bi->netFluxZ) {
    // KEYS expression is self-normalized
    if (_verbose)
      cout << "RooKeysPdf::analyticalIntegral() : Using mirrored normalization : " << bi->nEventsBW << endl;
    return bi->nEventsBW;
  } 
  // calculate leakage in and out of variable range box
  else 
  {
    norm = bi->nEventsBMSW; 
    if (norm<0.) norm=0.;
    
    for (Int_t i=0; i<Int_t(bi->sIdcs.size()); ++i) {      
      Double_t prob=1.;
      const vector<Double_t>& x = _dataPts[bi->sIdcs[i]];
      const vector<Double_t>& weight = (*_weights)[_idx[bi->sIdcs[i]]];
      
      vector<Double_t> chi(_nDim,100.);
      
      for (Int_t j=0; j<_nDim; j++) {
	if(!doInt[j]) continue;

	if ((x[j]>bi->xVarLoM3s[j] && x[j]<bi->xVarLoP3s[j]) && x[j]<(bi->xVarLo[j]+bi->xVarHi[j])/2.) 
	  chi[j] = (x[j]-bi->xVarLo[j])/weight[j];
	else if ((x[j]>bi->xVarHiM3s[j] && x[j]<bi->xVarHiP3s[j]) && x[j]>(bi->xVarLo[j]+bi->xVarHi[j])/2.) 
	  chi[j] = (bi->xVarHi[j]-x[j])/weight[j];

	if (chi[j]>0) // inVarRange
	  prob *= (0.5 + TMath::Erf(fabs(chi[j])/sqrt(2.))/2.);
	else // outside Var range
	  prob *= (0.5 - TMath::Erf(fabs(chi[j])/sqrt(2.))/2.);
      }

      norm += prob * _wMap[_idx[bi->sIdcs[i]]];    
    } 
    
    if (_verbose)
      cout << "RooKeysPdf::analyticalIntegral() : Final normalization : " << norm << endl;
    return norm;
  }
}


Bool_t RooKeysPdf::redirectServersHook(const RooAbsCollection& /*newServerList*/, Bool_t /*mustReplaceAll*/, 
 					 Bool_t /*nameChange*/, Bool_t /*isRecursive*/) 
{
  if (_weightDep) {
    delete _weightDep ;
    _weightDep=0 ;
  }
  
  return kFALSE;
}
