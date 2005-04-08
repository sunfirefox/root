// @(#)root/foam:$Name:$:$Id:$
// Authors: S. Jadach and P.Sawicki

#ifndef ROOT_TFoamIntegrand
#define ROOT_TFoamIntegrand

//_________________________________________
// Class TFoamIntegrand
// =====================
// Abstract class representing n-dimensional real positive integrand function

#ifndef ROOT_TObject
#include "TObject.h"
#endif

class TFoamIntegrand : public TObject  {
public:
   TFoamIntegrand() { };
   virtual ~TFoamIntegrand() { };
   virtual Double_t Density(Int_t ndim, Double_t *) = 0;

   ClassDef(TFoamIntegrand,1); //n-dimensional real positive integrand of FOAM
};

#endif
