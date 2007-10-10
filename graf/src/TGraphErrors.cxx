// @(#)root/graf:$Id$
// Author: Rene Brun   15/09/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include <string.h>

#include "Riostream.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TMath.h"
#include "TArrow.h"
#include "TBox.h"
#include "TVirtualPad.h"
#include "TH1.h"
#include "TF1.h"
#include "TVector.h"
#include "TVectorD.h"
#include "TStyle.h"
#include "TClass.h"

ClassImp(TGraphErrors)


//______________________________________________________________________________
/* Begin_Html
<center><h2>TGraphErrors class</h2></center>
A TGraphErrors is a TGraph with error bars.
The various format options to draw a TGraphErrors are explained in
<tt>TGraphErrors::Paint</tt>.
<p>
The picture below gives an example:
End_Html
Begin_Macro(source)
{
   c1 = new TCanvas("c1","A Simple Graph with error bars",200,10,700,500);
   c1->SetFillColor(42);
   c1->SetGrid();
   c1->GetFrame()->SetFillColor(21);
   c1->GetFrame()->SetBorderSize(12);
   Int_t n = 10;
   Double_t x[n]  = {-0.22, 0.05, 0.25, 0.35, 0.5, 0.61,0.7,0.85,0.89,0.95};
   Double_t y[n]  = {1,2.9,5.6,7.4,9,9.6,8.7,6.3,4.5,1};
   Double_t ex[n] = {.05,.1,.07,.07,.04,.05,.06,.07,.08,.05};
   Double_t ey[n] = {.8,.7,.6,.5,.4,.4,.5,.6,.7,.8};
   gr = new TGraphErrors(n,x,y,ex,ey);
   gr->SetTitle("TGraphErrors Example");
   gr->SetMarkerColor(4);
   gr->SetMarkerStyle(21);
   gr->Draw("ALP");
   return c1;
}
End_Macro */


//______________________________________________________________________________
TGraphErrors::TGraphErrors(): TGraph()
{
   // TGraphErrors default constructor.

   CtorAllocate();
}


//______________________________________________________________________________
TGraphErrors::TGraphErrors(Int_t n)
             : TGraph(n)
{
   // TGraphErrors normal constructor.
   //
   //  the arrays are preset to zero

   if (!CtorAllocate()) return;
   FillZero(0, fNpoints);
}


//______________________________________________________________________________
TGraphErrors::TGraphErrors(Int_t n, const Float_t *x, const Float_t *y, const Float_t *ex, const Float_t *ey)
       : TGraph(n,x,y)
{
   // TGraphErrors normal constructor.
   //
   //  if ex or ey are null, the corresponding arrays are preset to zero

   if (!CtorAllocate()) return;

   for (Int_t i=0;i<n;i++) {
      if (ex) fEX[i] = ex[i];
      else    fEX[i] = 0;
      if (ey) fEY[i] = ey[i];
      else    fEY[i] = 0;
   }
}


//______________________________________________________________________________
TGraphErrors::TGraphErrors(Int_t n, const Double_t *x, const Double_t *y, const Double_t *ex, const Double_t *ey)
       : TGraph(n,x,y)
{
   // TGraphErrors normal constructor.
   //
   //  if ex or ey are null, the corresponding arrays are preset to zero

   if (!CtorAllocate()) return;

   n = sizeof(Double_t)*fNpoints;
   if (ex) memcpy(fEX, ex, n);
   else    memset(fEX, 0, n);
   if (ey) memcpy(fEY, ey, n);
   else    memset(fEY, 0, n);
}


//______________________________________________________________________________
TGraphErrors::TGraphErrors(const TVectorF &vx, const TVectorF &vy, const TVectorF &vex, const TVectorF &vey)
             :TGraph()
{
   // constructor with four vectors of floats in input
   // A grapherrors is built with the X coordinates taken from vx and Y coord from vy
   // and the errors from vectors vex and vey.
   // The number of points in the graph is the minimum of number of points
   // in vx and vy.

   fNpoints = TMath::Min(vx.GetNrows(), vy.GetNrows());
   if (!TGraph::CtorAllocate()) return;
   if (!CtorAllocate()) return;
   Int_t ivxlow  = vx.GetLwb();
   Int_t ivylow  = vy.GetLwb();
   Int_t ivexlow = vex.GetLwb();
   Int_t iveylow = vey.GetLwb();
      for (Int_t i=0;i<fNpoints;i++) {
      fX[i]   = vx(i+ivxlow);
      fY[i]   = vy(i+ivylow);
      fEX[i]  = vex(i+ivexlow);
      fEY[i]  = vey(i+iveylow);
   }
}


//______________________________________________________________________________
TGraphErrors::TGraphErrors(const TVectorD  &vx, const TVectorD  &vy, const TVectorD  &vex, const TVectorD  &vey)
             :TGraph()
{
   // constructor with four vectors of doubles in input
   // A grapherrors is built with the X coordinates taken from vx and Y coord from vy
   // and the errors from vectors vex and vey.
   // The number of points in the graph is the minimum of number of points
   // in vx and vy.

   fNpoints = TMath::Min(vx.GetNrows(), vy.GetNrows());
   if (!TGraph::CtorAllocate()) return;
   if (!CtorAllocate()) return;
   Int_t ivxlow  = vx.GetLwb();
   Int_t ivylow  = vy.GetLwb();
   Int_t ivexlow = vex.GetLwb();
   Int_t iveylow = vey.GetLwb();
      for (Int_t i=0;i<fNpoints;i++) {
      fX[i]   = vx(i+ivxlow);
      fY[i]   = vy(i+ivylow);
      fEX[i]  = vex(i+ivexlow);
      fEY[i]  = vey(i+iveylow);
   }
}


//______________________________________________________________________________
TGraphErrors::TGraphErrors(const TGraphErrors &gr)
       : TGraph(gr)
{
   // TGraphErrors copy constructor

   if (!CtorAllocate()) return;

   Int_t n = sizeof(Double_t)*fNpoints;
   memcpy(fEX, gr.fEX, n);
   memcpy(fEY, gr.fEY, n);
}


//______________________________________________________________________________
TGraphErrors& TGraphErrors::operator=(const TGraphErrors &gr)
{
   // TGraphErrors assignment operator

   if(this!=&gr) {
      TGraph::operator=(gr);
      if (!CtorAllocate()) return *this;

      Int_t n = sizeof(Double_t)*fNpoints;
      memcpy(fEX, gr.fEX, n);
      memcpy(fEY, gr.fEY, n);
   }
   return *this;
}


//______________________________________________________________________________
TGraphErrors::TGraphErrors(const TH1 *h)
       : TGraph(h)
{
   // TGraphErrors constructor importing its parameters from the TH1 object passed as argument

   if (!CtorAllocate()) return;

   for (Int_t i=0;i<fNpoints;i++) {
      fEX[i] = h->GetBinWidth(i+1)*gStyle->GetErrorX();
      fEY[i] = h->GetBinError(i+1);
   }
}


//______________________________________________________________________________
TGraphErrors::TGraphErrors(const char *filename, const char *format, Option_t *)
       : TGraph(100)
{
   // GraphErrors constructor reading input from filename
   // filename is assumed to contain at least 3 columns of numbers
   // convention for format (default="%lg %lg %lg %lg)
   //  format = "%lg %lg"         read only 2 first columns into X,Y
   //  format = "%lg %lg %lg"     read only 3 first columns into X,Y and EY
   //  format = "%lg %lg %lg %lg" read only 4 first columns into X,Y,EX,EY

   CtorAllocate();
   Double_t x,y,ex,ey;
   ifstream infile(filename);
   if(!infile.good()){
      MakeZombie();
      Error("TGrapherrors", "Cannot open file: %s, TGraphErrors is Zombie",filename);
      fNpoints = 0;
      return;
   }
   // count number of columns in format
   Int_t ncol = CalculateScanfFields(format);
   Int_t np = 0;

   std::string line;
   while(std::getline(infile,line,'\n')){
      ex=ey=0;
      Int_t res;
      if (ncol < 3) {
         res = sscanf(line.c_str(),format,&x, &y);
      } else if(ncol <4) {
         res = sscanf(line.c_str(),format,&x, &y, &ey);
      } else {
         res = sscanf(line.c_str(),format,&x, &y, &ex, &ey);
      }
      if (res < 2) {
         // not a data line
         continue;
      }
      SetPoint(np,x,y);
      SetPointError(np,ex,ey);
      np++;
   }
   Set(np);
}


//______________________________________________________________________________
TGraphErrors::~TGraphErrors()
{
   // TGraphErrors default destructor.

   delete [] fEX;
   delete [] fEY;
}


//______________________________________________________________________________
void TGraphErrors::Apply(TF1 *f)
{
   // apply function to all the data points
   // y = f(x,y)
   //
   // The error is calculated as ey=(f(x,y+ey)-f(x,y-ey))/2
   // This is the same as error(fy) = df/dy * ey for small errors
   //
   // For generic functions the symmetric errors might become non-symmetric
   // and are averaged here. Use TGraphAsymmErrors if desired.
   //
   // error on x doesn't change
   // function suggested/implemented by Miroslav Helbich <helbich@mail.desy.de>

   Double_t x,y,ex,ey;

   for (Int_t i=0;i<GetN();i++) {
      GetPoint(i,x,y);
      ex=GetErrorX(i);
      ey=GetErrorY(i);

      SetPoint(i,x,f->Eval(x,y));
      SetPointError(i,ex,TMath::Abs(f->Eval(x,y+ey) - f->Eval(x,y-ey))/2.);
   }
}


//______________________________________________________________________________
Int_t TGraphErrors::CalculateScanfFields(const char *fmt)
{
   // Calculate scan fields.

   Int_t fields = 0;
   while ((fmt = strchr(fmt, '%'))) {
      Bool_t skip = kFALSE;
      while (*(++fmt)) {
         if ('[' == *fmt) {
            if (*++fmt && '^' == *fmt) ++fmt; // "%[^]a]"
            if (*++fmt && ']' == *fmt) ++fmt; // "%[]a]" or "%[^]a]"
            while (*fmt && *fmt != ']')
               ++fmt;
            if (!skip) ++fields;
            break;
         }
         if ('%' == *fmt) break; // %% literal %
         if ('*' == *fmt) {
            skip = kTRUE; // %*d -- skip a number
         } else if (strchr("dDiouxXxfegEscpn", *fmt)) {
            if (!skip) ++fields;
            break;
         }
         // skip modifiers & field width
      }
   }
   return fields;
}


//______________________________________________________________________________
void TGraphErrors::ComputeRange(Double_t &xmin, Double_t &ymin, Double_t &xmax, Double_t &ymax) const
{
   // Compute range.

   for (Int_t i=0;i<fNpoints;i++) {
      if (fX[i] -fEX[i] < xmin) {
         if (gPad && gPad->GetLogx()) {
            if (fEX[i] < fX[i]) xmin = fX[i]-fEX[i];
            else                xmin = TMath::Min(xmin,fX[i]/3);
         } else {
            xmin = fX[i]-fEX[i];
         }
      }
      if (fX[i] +fEX[i] > xmax) xmax = fX[i]+fEX[i];
      if (fY[i] -fEY[i] < ymin) {
         if (gPad && gPad->GetLogy()) {
            if (fEY[i] < fY[i]) ymin = fY[i]-fEY[i];
            else                ymin = TMath::Min(ymin,fY[i]/3);
         } else {
            ymin = fY[i]-fEY[i];
         }
      }
      if (fY[i] +fEY[i] > ymax) ymax = fY[i]+fEY[i];
   }
}


//______________________________________________________________________________
void TGraphErrors::CopyAndRelease(Double_t **newarrays,
                                  Int_t ibegin, Int_t iend, Int_t obegin)
{
   // Copy and release.

   CopyPoints(newarrays, ibegin, iend, obegin);
   if (newarrays) {
      delete[] fX;
      fX = newarrays[2];
      delete[] fY;
      fY = newarrays[3];
      delete[] fEX;
      fEX = newarrays[0];
      delete[] fEY;
      fEY = newarrays[1];
      delete[] newarrays;
   }
}


//______________________________________________________________________________
Bool_t TGraphErrors::CopyPoints(Double_t **arrays, Int_t ibegin, Int_t iend,
                                Int_t obegin)
{
   // Copy errors from fEX and fEY to arrays[0] and arrays[1]
   // or to fX and fY. Copy points.

   if (TGraph::CopyPoints(arrays ? arrays+2 : 0, ibegin, iend, obegin)) {
      Int_t n = (iend - ibegin)*sizeof(Double_t);
      if (arrays) {
         memmove(&arrays[0][obegin], &fEX[ibegin], n);
         memmove(&arrays[1][obegin], &fEY[ibegin], n);
      } else {
         memmove(&fEX[obegin], &fEX[ibegin], n);
         memmove(&fEY[obegin], &fEY[ibegin], n);
      }
      return kTRUE;
   } else {
      return kFALSE;
   }
}


//______________________________________________________________________________
Bool_t TGraphErrors::CtorAllocate()
{
   // Constructor allocate.

   if (!fNpoints) {
      fEX = fEY = 0;
      return kFALSE;
   } else {
      fEX = new Double_t[fMaxSize];
      fEY = new Double_t[fMaxSize];
   }
   return kTRUE;
}


//______________________________________________________________________________
void TGraphErrors::FillZero(Int_t begin, Int_t end, Bool_t from_ctor)
{
   // Set zero values for point arrays in the range [begin, end]

   if (!from_ctor) {
      TGraph::FillZero(begin, end, from_ctor);
   }
   Int_t n = (end - begin)*sizeof(Double_t);
   memset(fEX + begin, 0, n);
   memset(fEY + begin, 0, n);
}


//______________________________________________________________________________
Double_t TGraphErrors::GetErrorX(Int_t i) const
{
   // This function is called by GraphFitChisquare.
   // It returns the error along X at point i.

   if (i < 0 || i >= fNpoints) return -1;
   if (fEX) return fEX[i];
   return -1;
}


//______________________________________________________________________________
Double_t TGraphErrors::GetErrorY(Int_t i) const
{
   // This function is called by GraphFitChisquare.
   // It returns the error along Y at point i.

   if (i < 0 || i >= fNpoints) return -1;
   if (fEY) return fEY[i];
   return -1;
}


//______________________________________________________________________________
Double_t TGraphErrors::GetErrorXhigh(Int_t i) const
{
   // This function is called by GraphFitChisquare.
   // It returns the error along X at point i.

   if (i < 0 || i >= fNpoints) return -1;
   if (fEX) return fEX[i];
   return -1;
}


//______________________________________________________________________________
Double_t TGraphErrors::GetErrorXlow(Int_t i) const
{
   // This function is called by GraphFitChisquare.
   // It returns the error along X at point i.

   if (i < 0 || i >= fNpoints) return -1;
   if (fEX) return fEX[i];
   return -1;
}


//______________________________________________________________________________
Double_t TGraphErrors::GetErrorYhigh(Int_t i) const
{
   // This function is called by GraphFitChisquare.
   // It returns the error along X at point i.

   if (i < 0 || i >= fNpoints) return -1;
   if (fEY) return fEY[i];
   return -1;
}


//______________________________________________________________________________
Double_t TGraphErrors::GetErrorYlow(Int_t i) const
{
   // This function is called by GraphFitChisquare.
   // It returns the error along X at point i.

   if (i < 0 || i >= fNpoints) return -1;
   if (fEY) return fEY[i];
   return -1;
}


//______________________________________________________________________________
void TGraphErrors::Paint(Option_t *option)
{
   // Paint this TGraphErrors with its current attributes
   //
   // by default horizonthal and vertical small lines are drawn at
   // the end of the error bars. if option "z" or "Z" is specified,
   // these lines are not drawn.
   //
   // if option contains ">" an arrow is drawn at the end of the error bars
   // if option contains "|>" a full arrow is drawn at the end of the error bars
   // the size of the arrow is set to 2/3 of the marker size.
   //
   // By default, error bars are drawn. If option "X" is specified,
   // the errors are not drawn (TGraph::Paint equivalent).
   //
   // if option "[]" is specified only the end vertical/horizonthal lines
   // of the error bars are drawn. This option is interesting to superimpose
   // systematic errors on top of a graph with statistical errors.
   //
   // if option "2" is specified error rectangles are drawn.
   //
   // if option "3" is specified a filled area is drawn through the end points of
   // the vertical error bars.
   //
   // if option "4" is specified a smoothed filled area is drawn through the end
   // points of the vertical error bars.
   //
   // Use gStyle->SetErrorX(dx) to control the size of the error along x.
   // set dx = 0 to suppress the error along x.
   //
   // Use gStyle->SetEndErrorSize(np) to control the size of the lines
   // at the end of the error bars (when option 1 is used).
   // By default np=1. (np represents the number of pixels).

   Double_t *xline = 0;
   Double_t *yline = 0;
   Int_t if1 = 0;
   Int_t if2 = 0;

   const Int_t kBASEMARKER=8;
   Double_t s2x, s2y, symbolsize, sbase;
   Double_t x, y, ex, ey, xl1, xl2, xr1, xr2, yup1, yup2, ylow1, ylow2, tx, ty;
   static Float_t cxx[11] = {1,1,0.6,0.6,1,1,0.6,0.5,1,0.6,0.6};
   static Float_t cyy[11] = {1,1,1,1,1,1,1,1,1,0.5,0.6};

   if (strchr(option,'X') || strchr(option,'x')) {TGraph::Paint(option); return;}
   Bool_t brackets = kFALSE;
   if (strstr(option,"[]")) brackets = kTRUE;
   Bool_t endLines = kTRUE;
   if (strchr(option,'z')) endLines = kFALSE;
   if (strchr(option,'Z')) endLines = kFALSE;
   const char *arrowOpt = 0;
   if (strchr(option,'>'))  arrowOpt = ">";
   if (strstr(option,"|>")) arrowOpt = "|>";

   Bool_t axis = kFALSE;
   if (strchr(option,'a')) axis = kTRUE;
   if (strchr(option,'A')) axis = kTRUE;
   if (axis) TGraph::Paint(option);

   Bool_t option2 = kFALSE;
   Bool_t option3 = kFALSE;
   Bool_t option4 = kFALSE;
   if (strchr(option,'2')) option2 = kTRUE;
   if (strchr(option,'3')) option3 = kTRUE;
   if (strchr(option,'4')) {option3 = kTRUE; option4 = kTRUE;}

   if (option3) {
      xline = new Double_t[2*fNpoints];
      yline = new Double_t[2*fNpoints];
      if (!xline || !yline) {
         Error("Paint", "too many points, out of memory");
         return;
      }
      if1 = 1;
      if2 = 2*fNpoints;
   }

   TAttLine::Modify();

   TArrow arrow;
   arrow.SetLineWidth(GetLineWidth());
   arrow.SetLineColor(GetLineColor());
   arrow.SetFillColor(GetFillColor());

   TBox box;
   box.SetLineWidth(GetLineWidth());
   box.SetLineColor(GetLineColor());
   box.SetFillColor(GetFillColor());
   box.SetFillStyle(GetFillStyle());

   symbolsize  = GetMarkerSize();
   sbase       = symbolsize*kBASEMARKER;
   Int_t mark  = GetMarkerStyle();
   Double_t cx  = 0;
   Double_t cy  = 0;
   if (mark >= 20 && mark < 31) {
      cx = cxx[mark-20];
      cy = cyy[mark-20];
   }

   //      define the offset of the error bars due to the symbol size
   s2x  = gPad->PixeltoX(Int_t(0.5*sbase)) - gPad->PixeltoX(0);
   s2y  =-gPad->PixeltoY(Int_t(0.5*sbase)) + gPad->PixeltoY(0);
   Int_t dxend = Int_t(gStyle->GetEndErrorSize());
   tx    = gPad->PixeltoX(dxend) - gPad->PixeltoX(0);
   ty    =-gPad->PixeltoY(dxend) + gPad->PixeltoY(0);
   Float_t asize = 0.6*symbolsize*kBASEMARKER/gPad->GetWh();

   gPad->SetBit(kClipFrame, TestBit(kClipFrame));
   for (Int_t i=0;i<fNpoints;i++) {
      x  = gPad->XtoPad(fX[i]);
      y  = gPad->YtoPad(fY[i]);
      if (option3) {
         if (x < gPad->GetUxmin()) x = gPad->GetUxmin();
         if (x > gPad->GetUxmax()) x = gPad->GetUxmax();
         if (y < gPad->GetUymin()) y = gPad->GetUymin();
         if (y > gPad->GetUymax()) y = gPad->GetUymax();
      } else {
         if (x < gPad->GetUxmin()) continue;
         if (x > gPad->GetUxmax()) continue;
         if (y < gPad->GetUymin()) continue;
         if (y > gPad->GetUymax()) continue;
      }
      ex = fEX[i];
      ex = fEX[i];
      ey = fEY[i];

      //  draw the error rectangles
      if (option2) {
         box.PaintBox(gPad->XtoPad(fX[i] - ex),
                      gPad->YtoPad(fY[i] - ey),
                      gPad->XtoPad(fX[i] + ex),
                      gPad->YtoPad(fY[i] + ey));
         continue;
      }

      //  keep points for fill area drawing
      if (option3) {
         xline[if1-1] = x;
         xline[if2-1] = x;
         yline[if1-1] = gPad->YtoPad(fY[i] + ey);
         yline[if2-1] = gPad->YtoPad(fY[i] - ey);
         if1++;
         if2--;
         continue;
      }

      xl1 = x - s2x*cx;
      xl2 = gPad->XtoPad(fX[i] - ex);
      if (xl1 > xl2) {
         if (arrowOpt) {
            arrow.PaintArrow(xl1,y,xl2,y,asize,arrowOpt);
         } else {
            if (!brackets) gPad->PaintLine(xl1,y,xl2,y);
            if (endLines)  gPad->PaintLine(xl2,y-ty,xl2,y+ty);
         }
      }
      xr1 = x + s2x*cx;
      xr2 = gPad->XtoPad(fX[i] + ex);
      if (xr1 < xr2) {
         if (arrowOpt) {
            arrow.PaintArrow(xr1,y,xr2,y,asize,arrowOpt);
         } else {
            if (!brackets) gPad->PaintLine(xr1,y,xr2,y);
            if (endLines)  gPad->PaintLine(xr2,y-ty,xr2,y+ty);
         }
      }
      yup1 = y + s2y*cy;
      yup2 = gPad->YtoPad(fY[i] + ey);
      if (yup2 > gPad->GetUymax()) yup2 =  gPad->GetUymax();
      if (yup2 > yup1) {
         if (arrowOpt) {
            arrow.PaintArrow(x,yup1,x,yup2,asize,arrowOpt);
         } else {
            if (!brackets) gPad->PaintLine(x,yup1,x,yup2);
            if (endLines)  gPad->PaintLine(x-tx,yup2,x+tx,yup2);
         }
      }
      ylow1 = y - s2y*cy;
      ylow2 = gPad->YtoPad(fY[i] - ey);
      if (ylow2 < gPad->GetUymin()) ylow2 =  gPad->GetUymin();
      if (ylow2 < ylow1) {
         if (arrowOpt) {
            arrow.PaintArrow(x,ylow1,x,ylow2,asize,arrowOpt);
         } else {
            if (!brackets) gPad->PaintLine(x,ylow1,x,ylow2);
            if (endLines)  gPad->PaintLine(x-tx,ylow2,x+tx,ylow2);
         }
      }
   }
   if (!brackets && !axis) TGraph::Paint(option);
   gPad->ResetBit(kClipFrame);

   if (option3) {
      Int_t logx = gPad->GetLogx();
      Int_t logy = gPad->GetLogy();
      gPad->SetLogx(0);
      gPad->SetLogy(0);

      if (option4) PaintGraph(2*fNpoints, xline, yline,"FC");
      else         PaintGraph(2*fNpoints, xline, yline,"F");

      gPad->SetLogx(logx);
      gPad->SetLogy(logy);
      delete [] xline;
      delete [] yline;
   }
}


//______________________________________________________________________________
void TGraphErrors::Print(Option_t *) const
{
   // Print graph and errors values.

   for (Int_t i=0;i<fNpoints;i++) {
      printf("x[%d]=%g, y[%d]=%g, ex[%d]=%g, ey[%d]=%g\n",i,fX[i],i,fY[i],i,fEX[i],i,fEY[i]);
   }
}


//______________________________________________________________________________
void TGraphErrors::SavePrimitive(ostream &out, Option_t *option /*= ""*/)
{
   // Save primitive as a C++ statement(s) on output stream out

   char quote = '"';
   out<<"   "<<endl;
   if (gROOT->ClassSaved(TGraphErrors::Class())) {
      out<<"   ";
   } else {
      out<<"   TGraphErrors *";
   }
   out<<"gre = new TGraphErrors("<<fNpoints<<");"<<endl;
   out<<"   gre->SetName("<<quote<<GetName()<<quote<<");"<<endl;
   out<<"   gre->SetTitle("<<quote<<GetTitle()<<quote<<");"<<endl;

   SaveFillAttributes(out,"gre",0,1001);
   SaveLineAttributes(out,"gre",1,1,1);
   SaveMarkerAttributes(out,"gre",1,1,1);

   for (Int_t i=0;i<fNpoints;i++) {
      out<<"   gre->SetPoint("<<i<<","<<fX[i]<<","<<fY[i]<<");"<<endl;
      out<<"   gre->SetPointError("<<i<<","<<fEX[i]<<","<<fEY[i]<<");"<<endl;
   }

   static Int_t frameNumber = 0;
   if (fHistogram) {
      frameNumber++;
      TString hname = fHistogram->GetName();
      hname += frameNumber;
      fHistogram->SetName(hname.Data());
      fHistogram->SavePrimitive(out,"nodraw");
      out<<"   gre->SetHistogram("<<fHistogram->GetName()<<");"<<endl;
      out<<"   "<<endl;
   }

   // save list of functions
   TIter next(fFunctions);
   TObject *obj;
   while ((obj=next())) {
      obj->SavePrimitive(out,"nodraw");
      if (obj->InheritsFrom("TPaveStats")) {
         out<<"   gre->GetListOfFunctions()->Add(ptstats);"<<endl;
         out<<"   ptstats->SetParent(gre->GetListOfFunctions());"<<endl;
      } else {
         out<<"   gre->GetListOfFunctions()->Add("<<obj->GetName()<<");"<<endl;
      }
   }

   char *l = strstr(option,"multigraph");
   if (l) {
      out<<"   multigraph->Add(gre,"<<quote<<l+10<<quote<<");"<<endl;
   } else {
      out<<"   gre->Draw("<<quote<<option<<quote<<");"<<endl;
   }
}


//______________________________________________________________________________
void TGraphErrors::SetPointError(Double_t ex, Double_t ey)
{
   // Set ex and ey values for point pointed by the mouse.

   Int_t px = gPad->GetEventX();
   Int_t py = gPad->GetEventY();

   //localize point to be deleted
   Int_t ipoint = -2;
   Int_t i;
   // start with a small window (in case the mouse is very close to one point)
   for (i=0;i<fNpoints;i++) {
      Int_t dpx = px - gPad->XtoAbsPixel(gPad->XtoPad(fX[i]));
      Int_t dpy = py - gPad->YtoAbsPixel(gPad->YtoPad(fY[i]));
      if (dpx*dpx+dpy*dpy < 25) {ipoint = i; break;}
   }
   if (ipoint == -2) return;

   fEX[ipoint] = ex;
   fEY[ipoint] = ey;
   gPad->Modified();
}


//______________________________________________________________________________
void TGraphErrors::SetPointError(Int_t i, Double_t ex, Double_t ey)
{
   // Set ex and ey values for point number i.

   if (i < 0) return;
   if (i >= fNpoints) {
   // re-allocate the object
      TGraphErrors::SetPoint(i,0,0);
   }
   fEX[i] = ex;
   fEY[i] = ey;
}


//______________________________________________________________________________
void TGraphErrors::Streamer(TBuffer &b)
{
   // Stream an object of class TGraphErrors.

   if (b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = b.ReadVersion(&R__s, &R__c);
      if (R__v > 2) {
         b.ReadClassBuffer(TGraphErrors::Class(), this, R__v, R__s, R__c);
         return;
      }
      //====process old versions before automatic schema evolution
      TGraph::Streamer(b);
      fEX = new Double_t[fNpoints];
      fEY = new Double_t[fNpoints];
      if (R__v < 2) {
         Float_t *ex = new Float_t[fNpoints];
         Float_t *ey = new Float_t[fNpoints];
         b.ReadFastArray(ex,fNpoints);
         b.ReadFastArray(ey,fNpoints);
         for (Int_t i=0;i<fNpoints;i++) {
            fEX[i] = ex[i];
            fEY[i] = ey[i];
         }
         delete [] ey;
         delete [] ex;
      } else {
         b.ReadFastArray(fEX,fNpoints);
         b.ReadFastArray(fEY,fNpoints);
      }
      b.CheckByteCount(R__s, R__c, TGraphErrors::IsA());
      //====end of old versions

   } else {
      b.WriteClassBuffer(TGraphErrors::Class(),this);
   }
}


//______________________________________________________________________________
void TGraphErrors::SwapPoints(Int_t pos1, Int_t pos2)
{
   // Swap points

   SwapValues(fEX, pos1, pos2);
   SwapValues(fEY, pos1, pos2);
   TGraph::SwapPoints(pos1, pos2);
}
