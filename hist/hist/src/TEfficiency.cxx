#ifndef ROOT_TEfficiency_cxx
#define ROOT_TEfficiency_cxx

//standard header
#include <vector>
#include <string>
#include <cmath>

//ROOT headers
#include "Math/QuantFuncMathCore.h"
#include "TBinomialEfficiencyFitter.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TList.h"
#include "TMath.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TVirtualPad.h"

//custom headers
#include "TEfficiency.h"

//default values
const Double_t kDefBetaAlpha = 1;
const Double_t kDefBetaBeta = 1;
const Double_t kDefConfLevel = 0.95;
const Int_t kDefStatOpt = TEfficiency::kFCP;
const Double_t kDefWeight = 1;

ClassImp(TEfficiency)

//______________________________________________________________________________
// Begin_Html <center><h2>TEfficiency - a class to handle efficiency
// histograms</h2></center>
// <ol style="list-style-type: upper-roman;">
// <li><a href="#over">Overview</a></li>
// <li><a href="#create">Creating a TEfficiency object</a></li>
// <li><a href="#fill">Fill in events</a></li>
// <li><a href="#stat">Statistic options</a></li>
// <ol><li><a href="#compare">Coverage probabilities for different methods</a></li></ol>
// <li><a href="#cm">Merging and combining TEfficiency objects</a></li>
//  <ol>
//   <li><a href="#merge">When should I use merging?</a></li>
//   <li><a href="#comb">When should I use combining?</a></li>
//  </ol>
// <li><a href="#other">Further operations</a>
//  <ol>
//   <li><a href="#histo">Information about the internal histograms</a></li>
//   <li><a href="#fit">Fitting</a></li>
//   <li><a href="#draw">Draw a TEfficiency object</a></li>
//  </ol>
// <li><a href="#class">TEfficiency class</a></li>
// </ol>
//
// <h3><a name="over">I. Overview</a></h3>
// This class handles the calculation of efficiencies and their uncertainties. It
// provides several statistical methods for calculating frequentist and bayesian
// confidence intervals as well as a function for combining several efficiencies.
// <br />
// Efficiencies have a lot of applications and meanings but in principle they can
// be described by the fraction of good/passed events k out of sample containing
// N events. One is usually interested in the dependency of the efficiency on other
// (binned) variables. The number of passed and total events is therefore stored
// internally in two histograms (<a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:fTotalHistogram">fTotalHistogram</a> and
// <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:fPassedHistogram">fPassedHistogram</a>). Then the
// efficiency as well as its upper and lower error an be calculated for each bin
// individually.<br />
// As the efficiency can be regarded as a parameter of a binomial distribution, the
// number of pass ed and total events must always be integer numbers. Therefore a
// filling with weights is not possible however you can assign a global weight to each
// TEfficiency object (<a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:SetWeight">SetWeight</a>). It is necessary to create one TEfficiency object
// for each weight if you investigate a process involving different weights. This
// procedure needs more effort but enables you to re-use the filled object in cases
// where you want to change one or more weights. This would not be possible if all
// events with different weights were filled in the same histogram.
// 
// <h3><a name="create">II. Creating a TEfficiency object</a></h3>
// If you start a new analysis, it is highly recommended to use the TEfficiency class
// from the beginning. You can then use one of the constructors for fixed or
// variable bin size and your desired dimension. These constructors append the
// created TEfficiency object to the current directory. So it will be written
// automatically to a file during the next <a href="http://root.cern.ch/root/html/TFile.html#TFile:Write">TFile::Write</a> command.
// <div class="code"><pre>
// <b>Example:</b>
//create a twodimensional TEfficiency object with
//- name = "eff"
//- title = "my efficiency"
//- axistitles: x, y and LaTeX formated epsilon as label for Z axis
//- 10 bins with constant bin width (= 1) along X axis starting at 0 (lower edge
//  from first bin) upto 10 (upper edge of last bin)
//- 20 bins with constant bin width (= 0.5) along Y axis starting at -5 (lower
//  edge from first bin) upto 5 (upper edge of last bin)
// TEfficiency* pEff = new TEfficiency("eff","my efficiency;x;y;#epsilon",10,0,10,20,-5,5);
// </pre></div><div class="clear" />
// If you already have two histograms filled with the number of passed and total
// events, you will use the constructor <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:TEfficiency%1">TEfficiency(const TH1& passed,const TH1& total)</a>
// to construct the TEfficiency object. The histograms "passed" and "total" have
// to fullfill the conditions  mentioned in <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:CheckConsistency">CheckConsistency</a>, otherwise the construction will fail.
// As the histograms already exist, the new TEfficiency is by default <b>not</b> attached
// to the current directory to avoid duplication of data. If you want to store the
// new object anyway, you can either write it directly by calling <a href="http://root.cern.ch/root/html/TObject.html#TObject:Write">Write</a> or attach it to a directory using <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:SetDirectory">SetDirectory</a>.
// This also applies for TEfficiency objects created by the copy constructor <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:TEfficiency%8">TEfficiency(const TEfficiency& rEff)</a>.
// <div class="code">
// <pre>
// <b>Example 1:</b>
// TEfficiency* pEff = 0;
// TFile* pFile = new TFile("myfile.root","recreate");
// 
// //h_pass and h_total are valid and consistent histograms
// if(TEfficiency::CheckConsistency(h_pass,h_total))
// {
//   pEff = new TEfficiency(h_pass,h_total);
//   // this will write the TEfficiency object to "myfile.root"
//   // AND pEff will be attached to the current directory
//   pEff->Write();
// }
// 
// <b>Example 2:</b>
// TEfficiency* pEff = 0;
// TFile* pFile = new TFile("myfile.root","recreate");
// 
// //h_pass and h_total are valid and consistent histograms
// if(TEfficiency::CheckConsistency(h_pass,h_total))
// {
//   pEff = new TEfficiency(h_pass,h_total);
//   //this will attach the TEfficiency object to the current directory
//   pEff->SetDirectory(gDirectory);
//   //now all objects in gDirectory will be written to "myfile.root"
//   pFile->Write();
// }
// </pre>
// </div><div class="clear" />
// In the case that you already have two filled histograms and you only want to
// plot them as a graph, you should rather use <a href="http://root.cern.ch/root/html/TGraphAsymmErrors.html#TGraphAsymmErrors:TGraphAsymmErrors%8">TGraphAsymmErrors::TGraphAsymmErrors(const TH1* pass,const TH1* total,Option_t* opt)</a>
// to create a graph object.
//    
// <h3><a name="fill">III. Filling with events</a></h3>
//    You can fill the TEfficiency object by calling the <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:Fill">Fill(Bool_t bPassed,Double_t x,Double_t y,Double_t z)</a> method.
//    The boolean flag "bPassed" indicates whether the current event is a good
//    (both histograms are filled) or not (only <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:fTotalHistogram">fTotalHistogram</a> is filled).
//    The variables x,y and z determine the bin which is filled. For lower
//    dimensions the z- or even the y-value may be omitted.
//    End_Html
//    Begin_Macro(source)
// {
//    //canvas only needed for this documentation
//    TCanvas* c1 = new TCanvas("example","",600,400);
//    c1->SetFillStyle(1001);
//    c1->SetFillColor(kWhite);
//    
//    //create one-dimensional TEfficiency object with fixed bin size
//    TEfficiency* pEff = new TEfficiency("eff","my efficiency;x;#epsilon",20,0,10);
//    TRandom3 rand;
// 
//    bool bPassed;
//    double x;
//    for(int i=0; i<10000; ++i)
//    {
//       //simulate events with variable under investigation
//       x = rand.Uniform(10);
//       //check selection: bPassed = DoesEventPassSelection(x)
//       bPassed = rand.Rndm() < TMath::Gaus(x,5,4);
//       pEff->Fill(bPassed,x);
//    }
// 
//    pEff->Draw("AP");
// 
//    //only for this documentation
//    return c1;
// }
//    End_Macro
//    Begin_Html
//    You can also set the number of passed or total events for a bin directly by
//    using the <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:SetPassedEvents">SetPassedEvents</a> or <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:SetTotalEvents">SetTotalEvents</a> method.
//    
// <h3><a name="stat">IV. Statistic options</a></h3>
//    The calculation of the estimated efficiency depends on the chosen statistic
//    option. Let k denotes the number of passed events and N the number of total
//    events.<br />
//    <b>Frequentist methods</b><br />
//    The expectation value of the number of passed events is given by the true
//    efficiency times the total number of events. One can estimate the efficiency
//    by replacing the expected number of passed events by the observed number of
//    passed events. End_Html
//    Begin_Latex
// #LT k #GT = #epsilon #times N    #Rightarrow    #hat{#varepsilon} = #frac{k}{N}
//    End_Latex
//    Begin_Html
//    <b>Bayesian methods</b><br />
//    In bayesian statistics a likelihood-function (how probable is it to get the
//    observed data assuming a true efficiency) and a prior probability (what is the
//    probability that a certain true efficiency is actually realised) are used to
//    determine a posterior probability by using Bayes theorem. At the moment, only
//    beta distributions (have 2 free parameters) are supported as prior
//    probabilities.
//    End_Html
//    Begin_Latex(separator='=',align='rl')
//       P(#epsilon | k ; N) = #frac{1}{norm} #times P(k | #epsilon ; N) #times Prior(#epsilon)
//       P(k | #epsilon ; N) = Binomial(N,k) #times #epsilon^{k} #times (1 - #epsilon)^{N - k} ... binomial distribution
//       Prior(#epsilon) = #frac{1}{B(#alpha,#beta)} #times #epsilon ^{#alpha - 1} #times (1 - #epsilon)^{#beta - 1} #equiv Beta(#epsilon; #alpha,#beta)
// #Rightarrow P(#epsilon | k ; N) = #frac{1}{norm'} #times #epsilon^{k + #alpha - 1} #times (1 - #epsilon)^{N - k + #beta - 1} #equiv Beta(#epsilon; k + #alpha, N - k + #beta)
//    End_Latex
//    Begin_Html
//    The expectation value of this posterior distribution is used as estimator for the efficiency:
//    End_Html
//    Begin_Latex
// #hat{#varepsilon} = #frac{k + #alpha}{N + #alpha + #beta}
//    End_Latex
//    Begin_Html   
// The statistic options also specifiy which confidence interval is used for calculating
// the uncertainties of the efficiency. The following properties define the error
// calculation:
// <ul>
// <li><b>fConfLevel:</b> desired confidence level: 0 < fConfLevel < 1 (<a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:GetConfidenceLevel">GetConfidenceLevel</a> / <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:SetConfidenceLevel">SetConfidenceLevel</a>)</li>
// <li><b>fStatisticOption:</b> defines which method is used to calculate the boundaries of the confidence interval (<a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:SetStatisticOption">SetStatisticOption</a>)</li>
// <li><b>fBeta_alpha, fBeta_beta:</b> parameters for the prior distribution which is only used in the bayesian case (<a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:GetBetaAlpha">GetBetaAlpha</a> / <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:GetBetaBeta">GetBetaBeta</a> / <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:SetBetaAlpha">SetBetaAlpha</a> / <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:SetBetaBeta">SetBetaBeta</a>)</li>
// <li><b>kIsBayesian:</b> flag whether bayesian statistics are used or not (<a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:UsesBayesianStat">UsesBayesianStat</a>)</li>
// <li><b>fWeight:</b> global weight for this TEfficiency object which is used during combining or merging with other TEfficiency objects(<a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:GetWeight">GetWeight</a> / <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:SetWeight">SetWeight</a>)</li>
// </ul>
//    In the following table the implemented confidence intervals are listed
//    with their corresponding statistic option. For more details on the calculation,
//    please have a look at the the mentioned functions.<br /><br />
// <table align="center" border="1" cellpadding="5" rules="rows" vspace="10">
//    <caption align="bottom">implemented confidence intervals and their options</caption>
//    <tr>
//    <th>name</th><th>statistic option</th><th>function</th><th>kIsBayesian</th><th>parameters</th>
//    </tr>
//    <tr>
//    <td>Clopper-Pearson</td><td>kFCP</td>
//    <td>
//     <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:ClopperPearson">ClopperPearson</a>
//    </td>
//    <td>false</td>
//    <td>
//      <ul><li>total events</li><li>passed events</li><li>confidence level</li></ul>
//    </td>
//    </tr>
//    <tr>
//    <td>normal approximation</td><td>kFNormal</td>
//    <td>
//     <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:Normal">Normal</a>
//    </td>
//    <td>false</td>
//    <td>
//      <ul><li>total events</li><li>passed events</li><li>confidence level</li></ul>
//    </td>
//    </tr>
//    <tr>
//    <td>Wilson</td><td>kFWilson</td>
//    <td>
//     <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:Wilson">Wilson</a>
//    </td>
//    <td>false</td>
//    <td>
//      <ul><li>total events</li><li>passed events</li><li>confidence level</li></ul>
//    </td>
//    </tr>
//    <tr>
//    <td>Agresti-Coull</td><td>kFAC</td>
//    <td>
//     <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:AgrestiCoull">AgrestiCoull</a>
//    </td>
//    <td>false</td>
//    <td>
//      <ul><li>total events</li><li>passed events</li><li>confidence level</li></ul>
//    </td>
//    </tr>
//    <tr>
//    <td>Jeffrey</td><td>kBJeffrey</td>
//    <td>
//     <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:Bayesian">Bayesian</a>
//    </td>
//    <td>true</td>
//    <td>
//      <ul><li>total events</li><li>passed events</li><li>confidence level</li><li>fBeta_alpha = 0.5</li><li>fBeta_beta = 0.5</li></ul>
//    </td>
//    </tr>
//    <td>Uniform prior</td><td>kBUniform</td>
//    <td>
//     <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:Bayesian">Bayesian</a>
//    </td>
//    <td>true</td>
//    <td>
//      <ul><li>total events</li><li>passed events</li><li>confidence level</li><li>fBeta_alpha = 1</li><li>fBeta_beta = 1</li></ul>
//    </td>
//    </tr>
//    <td>custom prior</td><td>kBBayesian</td>
//    <td>
//     <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:Bayesian">Bayesian</a>
//    </td>
//    <td>true</td>
//    <td>
//      <ul><li>total events</li><li>passed events</li><li>confidence level</li><li>fBeta_alpha</li><li>fBeta_beta</li></ul>
//    </td>
//    </tr>
//    </table>
//    <br />
//    The following example demonstrates the effect of different statistic options and
//    confidence levels.
//    End_Html
//    Begin_Macro(source)
// {
//    //canvas only needed for the documentation
//    TCanvas* c1 = new TCanvas("c1","",600,400);
//    c1->Divide(2);
//    c1->SetFillStyle(1001);
//    c1->SetFillColor(kWhite);
// 
//    //create one-dimensional TEfficiency object with fixed bin size
//    TEfficiency* pEff = new TEfficiency("eff","different confidence levels;x;#epsilon",20,0,10);
//    TRandom3 rand;
// 
//    bool bPassed;
//    double x;
//    for(int i=0; i<1000; ++i)
//    {
//       //simulate events with variable under investigation
//       x = rand.Uniform(10);
//       //check selection: bPassed = DoesEventPassSelection(x)
//       bPassed = rand.Rndm() < TMath::Gaus(x,5,4);
//       pEff->Fill(bPassed,x);
//    }
// 
//    //set style attributes
//    pEff->SetFillStyle(3004);
//    pEff->SetFillColor(kRed);
// 
//    //copy current TEfficiency object and set new confidence level
//    TEfficiency* pCopy = new TEfficiency(*pEff);
//    pCopy->SetConfidenceLevel(0.683);
// 
//    //set style attributes
//    pCopy->SetFillStyle(3005);
//    pCopy->SetFillColor(kBlue);
// 
//    c1->cd(1);
//  
//    //add legend
//    TLegend* leg1 = new TLegend(0.3,0.1,0.7,0.5);
//    leg1->AddEntry(pEff,"95%","F");
//    leg1->AddEntry(pCopy,"68.3%","F");
//    
//    pEff->Draw("A4");
//    pCopy->Draw("same4");
//    leg1->Draw("same");
// 
//    //use same confidence level but different statistic methods
//    TEfficiency* pEff2 = new TEfficiency(*pEff);
//    TEfficiency* pCopy2 = new TEfficiency(*pEff);
//    
//    pEff2->SetStatisticOption(TEfficiency::kFNormal);
//    pCopy2->SetStatisticOption(TEfficiency::kFAC);
// 
//    pEff2->SetTitle("different statistic options;x;#epsilon");
// 
//    //set style attributes
//    pCopy2->SetFillStyle(3005);
//    pCopy2->SetFillColor(kBlue);
// 
//    c1->cd(2);
//  
//    //add legend
//    TLegend* leg2 = new TLegend(0.3,0.1,0.7,0.5);
//    leg2->AddEntry(pEff2,"kFNormal","F");
//    leg2->AddEntry(pCopy2,"kFAC","F");
//    
//    pEff2->Draw("a4");
//    pCopy2->Draw("same4");
//    leg2->Draw("same");
// 
//    //only for this documentation
//    c1->cd(0);
//    return c1;
// }
//    End_Macro
//    Begin_Html
//    The prior probability of the efficiency in bayesian statistics can be given
//    in terms of a beta distribution. The beta distribution has to positive shape
//    parameters. The resulting priors for different combinations of these shape
//    parameters are shown in the plot below.
//    End_Html
//    Begin_Macro(source)
//    {
//       //canvas only needed for the documentation
//       TCanvas* c1 = new TCanvas("c1","",600,400);
//       c1->SetFillStyle(1001);
//       c1->SetFillColor(kWhite);
// 
//       //create different beta distributions
//       TF1* f1 = new TF1("f1","TMath::BetaDist(x,1,1)",0,1);
//       f1->SetLineColor(kBlue);
//       TF1* f2 = new TF1("f2","TMath::BetaDist(x,0.5,0.5)",0,1);
//       f2->SetLineColor(kRed);
//       TF1* f3 = new TF1("f3","TMath::BetaDist(x,1,5)",0,1);
//       f3->SetLineColor(kGreen+3);
//       f3->SetTitle("Beta distributions as priors;#epsilon;P(#epsilon)");
//       TF1* f4 = new TF1("f4","TMath::BetaDist(x,4,3)",0,1);
//       f4->SetLineColor(kViolet);
// 
//       //add legend
//       TLegend* leg = new TLegend(0.25,0.5,0.85,0.89);
//       leg->SetFillColor(kWhite);
//       leg->SetFillStyle(1001);
//       leg->AddEntry(f1,"a=1, b=1","L");
//       leg->AddEntry(f2,"a=0.5, b=0.5","L");
//       leg->AddEntry(f3,"a=1, b=5","L");
//       leg->AddEntry(f4,"a=4, b=3","L");
//       
//       f3->Draw();
//       f1->Draw("same");
//       f2->Draw("Same");
//       f4->Draw("same");
//       leg->Draw("same");
// 
//       //only for this documentation
//       return c1;
//    }
//    End_Macro      
//    Begin_Html
//
//    <h4><a name="compare">IV.1 Coverage probabilities for different methods</a></h4>
//    The following pictures illustrate the actual coverage probability for the
//    different values of the true efficiency and the total number of events when a
//    confidence level of 95% is desired.
//    <p><img src="normal95.gif" alt="normal approximation" width="600" height="400" /></p>
//    <p><img src="wilson95.gif" alt="wilson" width="600" height="400" /></p>
//    <p><img src="ac95.gif" alt="agresti coull" width="600" height="400" /></p>
//    <p><img src="cp95.gif" alt="clopper pearson" width="600" height="400" /></p>
//    <p><img src="uni95.gif" alt="uniform prior" width="600" height="400" /></p>
//    <p><img src="jeffrey95.gif" alt="jeffrey prior" width="600" height="400" /></p>
//
//    The average (over all possible true efficiencies) coverage probability for
//    different number of total events is shown in the next picture.
//    <p><img src="av_cov.png" alt="average coverage" width="600" height="400" /></p>
//    <h3><a name="cm">V. Merging and combining TEfficiency objects</a></h3>
//    In many applications the efficiency should be calculated for an inhomogenous
//    sample in the sense that it contains events with different weights. In order
//    to be able to determine the correct overall efficiency, it is necessary to
//    use for each subsample (= all events with the same weight) a different
//    TEfficiency object. After finsihing your analysis you can then construct the
//    overall efficiency with its uncertainty.<br />
//    This procedure has the advantage that you can change the weight of one
//    subsample easily without rerunning the whole analysis. On the other hand more
//    efford is needed to handle several TEfficiency objects instead of one
//    histogram. In the case of many different or even continuously distributed
//    weights this approach becomes cumbersome. One possibility to overcome this
//    problem is the usage of binned weights.<br /><br />
//    <b>Example</b><br />
//    In high particle physics weights arises from the fact that you want to
//    normalise your results to a certain reference value. A very common formula for
//    calculating weights is End_Html
//    Begin_Latex(separator='-')
//    w = #frac{#sigma L}{N_{gen} #epsilon_{trig}} - #sigma ... cross section
//    - L ... luminosity
//    - N_{gen} ... number of generated events
//    - #epsilon_{trig} ... (known) trigger efficiency
//    End_Latex
//    Begin_Html
//    The reason for different weights can therefore be:<ul>
//    <li>different processes</li>
//    <li>other integrated luminosity</li>
//    <li>varying trigger efficiency</li>
//    <li>different sample sizes</li>
//    <li>...</li>
//    <li>or even combination of them</li>
//    </ul>
//    Depending on the actual meaning of different weights in your case, you
//    should either merge or combine them to get the overall efficiency.
// 						      
//    <h4><a name="merge">V.1 When should I use merging?</a></h4>
//    If the weights are artificial and do not represent real alternative hypotheses,
//    you should merge the different TEfficiency objects. That means especially for
//    the bayesian case that the prior probability should be the same for all merged
//    TEfficiency objects. The merging can be done by invoking one of the following
//    operations:
//    <ul>
//    <li> <b>eff1</b>.<a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:Add">Add</a>(eff2)</li>
//    <li> <b>eff1</b> <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:operator+=">+=</a> eff2</li>
//    <li> <b>eff</b> = eff1 + eff2</li>
//    </ul>
//    The result of the merging is stored in the TEfficiency object which is marked
//    bold above. The contents of the internal histograms of both TEfficiency
//    objects are added and a new weight is assigned. The statistic options are not
//    changed.
//    End_Html
//    Begin_Latex #frac{1}{w_{new}} = #frac{1}{w_{1}} + #frac{1}{w_{2}}End_Latex
//    Begin_Html
//    <b>Example:</b><br />
//    If you use two samples with different numbers of generated events for the same
//    process and you want to normalise both to the same integrated luminosity and
//    trigger efficiency, the different weights then arise just from the fact that
//    you have different numbers of events. The TEfficiency objects should be merged
//    because the samples do not represent true alternatives. You expect the same
//    result as if you would have a big sample with all events in it.
//    End_Html
//    Begin_Latex
//    w_{1} = #frac{#sigma L}{#epsilon N_{1}}, w_{2} = #frac{#sigma L}{#epsilon N_{2}} #Rightarrow w_{new} = #frac{#sigma L}{#epsilon (N_{1} + N_{2})} = #frac{1}{#frac{1}{w_{1}} + #frac{1}{w_{2}}}
//    End_Latex							     
//    Begin_Html							     
// 			  
//    <h4><a name="comb">V.2 When should I use combining?</a></h4>
// You should combine TEfficiency objects whenever the weights represent
// alternatives processes for the efficiency. As the combination of two TEfficiency
// objects is not always consistent with the representation by two internal
// histograms, the result is not stored in a TEfficiency object but a TGraphAsymmErrors
// is returned which shows the estimated combined efficiency and its uncertainty
// for each bin. At the moment the combination method <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:Combine">Combine </a>only supports combination of 1-dimensional efficiencies in a bayesian approach.<br />
// For calculating the combined efficiency and its uncertainty for each bin, a weighted average posterior distribution is constructed. An equal-tailed confidence interval is constructed which might be not the shortest one.
// End_Html
// Begin_Latex(separator='=',align='rl')
// P_{comb}(#epsilon | {k_{i}} , {N_{i}}) = norm #times #sum_{j} p_{j} #times P_{j}(#epsilon | k_{j} , N_{j})
// p_{j} = probability of an event coming from sub sample j
// p_{j} = w_{j} #times N_{j} is used if no probabilites are given
// #hat{#varepsilon} = #int_{0}^{1} #epsilon #times P_{comb}(#epsilon | {k_{i}} , {N_{i}}) d#epsilon
// confidence level = 1 - #alpha
// #frac{#alpha}{2} = #int_{0}^{#epsilon_{low}} P_{comb}(#epsilon | {k_{i}} , {N_{i}}) d#epsilon ... defines lower boundary
// 1- #frac{#alpha}{2} = #int_{0}^{#epsilon_{up}} P_{comb}(#epsilon | {k_{i}} , {N_{i}}) d#epsilon ... defines upper boundary
// End_Latex
// Begin_Html
// <b>Example:</b><br />
// If you use cuts to select electrons which can originate from two different
// processes, you can determine the selection efficiency for each process. The
// overall selection efficiency is then the combined efficiency. The weights for
// the individual posterior distributions should be the probability that an
// electron comes from the corresponding process.
// End_Html
// Begin_Latex
// p_{1} = #frac{#sigma_{1}}{#sigma_{1} + #sigma_{2}} = #frac{N_{1}w_{1}}{N_{1}w_{1} + N_{2}w_{2}}
// p_{2} = #frac{#sigma_{2}}{#sigma_{1} + #sigma{2}} = #frac{N_{2}w_{2}}{N_{1}w_{1} + N_{2}w_{2}}
// P_{ges}(#epsilon | k_{1}, k_{2}; N_{1}, N_{2}) = p_{1} #times P_{1}(#epsilon | k_{1}; N_{1}) + p_{2} #times P_{2}(#epsilon | k_{2}; N_{2})
// End_Latex
// Begin_Html
// <h3><a name="other">VI. Further operations</a></h3>
// 
// <h4><a name="histo">VI.1 Information about the internal histograms</a></h4>
// The methods <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:GetPassedHistogram">GetPassedHistogram</a> and <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:GetTotalHistogram">GetTotalHistogram</a>
// return a constant pointer to the internal histograms. They can be used to
// obtain information about the internal histograms (e.g. the binning, number of passed / total events in a bin, mean values...).
// One can obtain a clone of the internal histograms by calling <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:GetCopyPassedHisto">GetCopyPassedHisto</a> or <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:GetCopyTotalHisto">GetCopyTotalHisto</a>.
// The returned histograms are completely independent from the current
// TEfficiency object. By default, they are not attached to a directory to
// avoid the duplication of data and the user is responsible for deleting them.
// <div class="code">
// <pre>
// <b>Example:</b>
// //open a root file which contains a TEfficiency object
// TFile* pFile = new TFile("myfile.root","update");
// 
// //get TEfficiency object with name "my_eff"
// TEfficiency* pEff = (TEfficiency*)pFile->Get("my_eff");
// 
// //get clone of total histogram
// TH1* clone = pEff->GetCopyTotalHisto();
// 
// //change clone...
// //save changes of clone directly
// clone->Write();
// //or append it to the current directoy and write the file
// //clone->SetDirectory(gDirectory);
// //pFile->Wrtie();
// 
// //delete histogram object
// delete clone;
// clone = 0;
// </pre>
// </div><div class="clear" />
// It is also possible to set the internal total or passed histogram by using the
// methods <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:SetPassedHistogram">SetPassedHistogram</a> or
// <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:SetTotalHistogram">SetTotalHistogram</a>.
// In order to ensure the validity of the TEfficiency object, the consistency of the
// new histogram and the stored histogram is checked. It sometimes might be
// impossible to change the histograms in a consistent way. Therefore one can force
// the replacement by passing the option "f". Then the user has to ensure that the
// other internal histogram is replaced as well and that the TEfficiency object is
// in a valid state.
// 
//    <h4><a name="fit">VI.2 Fitting</a></h4>
//    The efficiency can be fitted using the <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:Fit">Fit</a> function which uses internally the <a href="http://root.cern.ch/root/html/TBinomialEfficiencyFitter.html#TBinomialEfficiencyFitter:Fit">TBinomialEfficiencyFitter::Fit</a> method.
//    As this method is using a maximum-likelihood-fit, it is necessary to initialise
//    the given fit function with reasonable start values.
//    The resulting fit function is attached to the list of associated functions and
//    will be drawn automatically during the next <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:Draw">Draw</a> command.
//    The list of associated function can be modified by using the pointer returned
//    by <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:GetListOfFunctions">GetListOfFunctions</a>.
//    End_Html
//    Begin_Macro(source)
// {
//    //canvas only needed for this documentation
//    TCanvas* c1 = new TCanvas("example","",600,400);
//    c1->SetFillStyle(1001);
//    c1->SetFillColor(kWhite);
//    
//    //create one-dimensional TEfficiency object with fixed bin size
//    TEfficiency* pEff = new TEfficiency("eff","my efficiency;x;#epsilon",20,0,10);
//    TRandom3 rand;
// 
//    bool bPassed;
//    double x;
//    for(int i=0; i<10000; ++i)
//    {
//       //simulate events with variable under investigation
//       x = rand.Uniform(10);
//       //check selection: bPassed = DoesEventPassSelection(x)
//       bPassed = rand.Rndm() < TMath::Gaus(x,5,4);
//       pEff->Fill(bPassed,x);
//    }
// 
//    //create a function for fitting and do the fit
//    TF1* f1 = new TF1("f1","gaus",0,10);
//    f1->SetParameters(1,5,2);
//    pEff->Fit(f1);
// 
//    //create a threshold function
//    TF1* f2 = new TF1("thres","0.8",0,10);
//    f2->SetLineColor(kRed);
//    //add it to the list of functions
//    //use add first because the parameters of the last function will be displayed
//    pEff->GetListOfFunctions()->AddFirst(f2);
//    
//    pEff->Draw("AP");
// 
//    //only for this documentation
//    return c1;
// }
//    End_Macro
//    Begin_Html
//    
//    <h4><a name="draw">VI.3 Draw a TEfficiency object</a></h4>
//    A TEfficiency object can be drawn by calling the usual <a href="http://root.cern.ch/root/html/TEfficiency.html#TEfficiency:Draw">Draw</a> method.
//    At the moment drawing is only supported for 1- and 2-dimensional TEfficiency
//    objects. In the 1-dimensional case you can use the same options as for the <br />
//    <a href="http://root.cern.ch/root/html/TGraph.html#TGraph:Draw">TGraphAsymmErrors::Draw</a> 
//    method. For 2-dimensional TEfficiency objects you can pass the same options as
//    for a <a href="http://root.cern.ch/root/html/TH1.html#TH1:Draw">TH2::Draw</a> object.
// 
//    <h3 style="margin-bottom:-3em;"><a name="class">VII. TEfficiency class</a></h3>
//    
//End_Html
//______________________________________________________________________________

//______________________________________________________________________________
TEfficiency::TEfficiency():
   fBeta_alpha(kDefBetaAlpha),
   fBeta_beta(kDefBetaBeta),
   fBoundary(0),
   fConfLevel(kDefConfLevel),
   fDirectory(0),
   fFunctions(0),
   fPaintGraph(0),
   fPaintHisto(0),
   fPassedHistogram(0),
   fTotalHistogram(0),
   fWeight(kDefWeight)
{
   //default constructor
   //
   //should not be used explicitly
   
   SetStatisticOption(kDefStatOpt);
}

//______________________________________________________________________________
TEfficiency::TEfficiency(const TH1& passed,const TH1& total):
   fBeta_alpha(kDefBetaAlpha),
   fBeta_beta(kDefBetaBeta),
   fConfLevel(kDefConfLevel),
   fDirectory(0),
   fFunctions(0),
   fPaintGraph(0),
   fPaintHisto(0),
   fWeight(kDefWeight)
{
   //constructor using two existing histograms as input
   //
   //Input: passed - contains the events fullfilling some criteria
   //       total  - contains all investigated events
   //
   //Notes: - both histograms have to fullfill the conditions of CheckConsistency
   //       - dimension of the resulating efficiency object depends
   //         on the dimension of the given histograms
   //       - Clones of both histograms are stored internally
   //       - The function SetName(total.GetName() + "_clone") is called to set
   //         the names of the new object and the internal histograms..
   //       - The created TEfficiency object is NOT appended to a directory. It
   //         will not be written to disk during the next TFile::Write() command
   //         in order to prevent duplication of data. If you want to save this
   //         TEfficiency object anyway, you can either append it to a
   //         directory by calling SetDirectory(TDirectory*) or write it
   //         explicitly to disk by calling Write().

   //check consistency of histograms
   if(CheckConsistency(passed,total)) {
       Bool_t bStatus = TH1::AddDirectoryStatus();
       TH1::AddDirectory(kFALSE);
       fTotalHistogram = (TH1*)total.Clone();
       fPassedHistogram = (TH1*)passed.Clone();
       TH1::AddDirectory(bStatus);

       char buffer[100];
       strcpy(buffer,total.GetName());
       strcat(buffer,"_clone");
       SetName(buffer);
   }
   else {
      Error("TEfficiency(const TH1&,const TH1&)","histograms are not consistent -> results are useless");
      Warning("TEfficiency(const TH1&,const TH1&)","using two empty TH1D('h1','h1',10,0,10)");

      Bool_t bStatus = TH1::AddDirectoryStatus();
      TH1::AddDirectory(kFALSE);
      fTotalHistogram = new TH1D("h1_total","h1 (total)",10,0,10);
      fPassedHistogram = new TH1D("h1_passed","h1 (passed)",10,0,10);
      TH1::AddDirectory(bStatus);
   }   

   SetStatisticOption(kDefStatOpt);
   SetDirectory(0);
}

//______________________________________________________________________________
TEfficiency::TEfficiency(const char* name,const char* title,Int_t nbins,
				const Double_t* xbins):
   fBeta_alpha(kDefBetaAlpha),
   fBeta_beta(kDefBetaBeta),
   fConfLevel(kDefConfLevel),
   fDirectory(0),
   fFunctions(0),
   fPaintGraph(0),
   fPaintHisto(0),
   fWeight(kDefWeight)
{
   //create 1-dimensional TEfficiency object with variable bin size
   //
   //constructor creates two new and empty histograms with a given binning
   //
   // Input: name   - the common part of the name for both histograms (no blanks)
   //                 fTotalHistogram has name: name + "_total"
   //                 fPassedHistogram has name: name + "_passed"
   //        title  - the common part of the title for both histogram
   //                 fTotalHistogram has title: title + " (total)"
   //                 fPassedHistogram has title: title + " (passed)"
   //                 It is possible to label the axis by passing a title with
   //                 the following format: "title;xlabel;ylabel".
   //        nbins  - number of bins on the x-axis
   //        xbins  - array of length (nbins + 1) with low-edges for each bin
   //                 xbins[nbinsx] ... lower edge for overflow bin

   Bool_t bStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   fTotalHistogram = new TH1D("total","total",nbins,xbins);
   fPassedHistogram = new TH1D("passed","passed",nbins,xbins);
   TH1::AddDirectory(bStatus);

   Build(name,title);
}

//______________________________________________________________________________
TEfficiency::TEfficiency(const char* name,const char* title,Int_t nbinsx,
				Double_t xlow,Double_t xup):
   fBeta_alpha(kDefBetaAlpha),
   fBeta_beta(kDefBetaBeta),
   fConfLevel(kDefConfLevel),
   fDirectory(0),
   fFunctions(0),
   fPaintGraph(0),
   fPaintHisto(0),
   fWeight(kDefWeight)
{
   //create 1-dimensional TEfficiency object with fixed bins isze
   //
   //constructor creates two new and empty histograms with a fixed binning
   //
   // Input: name   - the common part of the name for both histograms(no blanks)
   //                 fTotalHistogram has name: name + "_total"
   //                 fPassedHistogram has name: name + "_passed"
   //        title  - the common part of the title for both histogram
   //                 fTotalHistogram has title: title + " (total)"
   //                 fPassedHistogram has title: title + " (passed)"
   //                 It is possible to label the axis by passing a title with
   //                 the following format: "title;xlabel;ylabel".
   //        nbinsx - number of bins on the x-axis
   //        xlow   - lower edge of first bin
   //        xup    - upper edge of last bin

   Bool_t bStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   fTotalHistogram = new TH1D("total","total",nbinsx,xlow,xup);
   fPassedHistogram = new TH1D("passed","passed",nbinsx,xlow,xup);
   TH1::AddDirectory(bStatus);

   Build(name,title);
}

//______________________________________________________________________________
TEfficiency::TEfficiency(const char* name,const char* title,Int_t nbinsx,
			   Double_t xlow,Double_t xup,Int_t nbinsy,
			   Double_t ylow,Double_t yup):
   fBeta_alpha(kDefBetaAlpha),
   fBeta_beta(kDefBetaBeta),
   fConfLevel(kDefConfLevel),
   fDirectory(0),
   fFunctions(0),
   fPaintGraph(0),
   fPaintHisto(0),
   fWeight(kDefWeight)
{
   //create 2-dimensional TEfficiency object with fixed bin size
   //
   //constructor creates two new and empty histograms with a fixed binning
   //
   // Input: name   - the common part of the name for both histograms(no blanks)
   //                 fTotalHistogram has name: name + "_total"
   //                 fPassedHistogram has name: name + "_passed"
   //        title  - the common part of the title for both histogram
   //                 fTotalHistogram has title: title + " (total)"
   //                 fPassedHistogram has title: title + " (passed)"
   //                 It is possible to label the axis by passing a title with
   //                 the following format: "title;xlabel;ylabel;zlabel".
   //        nbinsx - number of bins on the x-axis
   //        xlow   - lower edge of first x-bin
   //        xup    - upper edge of last x-bin
   //        nbinsy - number of bins on the y-axis
   //        ylow   - lower edge of first y-bin
   //        yup    - upper edge of last y-bin

   Bool_t bStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   fTotalHistogram = new TH2D("total","total",nbinsx,xlow,xup,nbinsy,ylow,yup);
   fPassedHistogram = new TH2D("passed","passed",nbinsx,xlow,xup,nbinsy,ylow,yup);
   TH1::AddDirectory(bStatus);

   Build(name,title);
}

//______________________________________________________________________________
TEfficiency::TEfficiency(const char* name,const char* title,Int_t nbinsx,
			   const Double_t* xbins,Int_t nbinsy,
			   const Double_t* ybins):
   fBeta_alpha(kDefBetaAlpha),
   fBeta_beta(kDefBetaBeta),
   fConfLevel(kDefConfLevel),
   fDirectory(0),
   fFunctions(0),
   fPaintGraph(0),
   fPaintHisto(0),
   fWeight(kDefWeight)
{
   //create 2-dimensional TEfficiency object with variable bin size
   //
   //constructor creates two new and empty histograms with a given binning
   //
   // Input: name   - the common part of the name for both histograms(no blanks)
   //                 fTotalHistogram has name: name + "_total"
   //                 fPassedHistogram has name: name + "_passed"
   //        title  - the common part of the title for both histogram
   //                 fTotalHistogram has title: title + " (total)"
   //                 fPassedHistogram has title: title + " (passed)"
   //                 It is possible to label the axis by passing a title with
   //                 the following format: "title;xlabel;ylabel;zlabel".
   //        nbinsx - number of bins on the x-axis
   //        xbins  - array of length (nbins + 1) with low-edges for each bin
   //                 xbins[nbinsx] ... lower edge for overflow x-bin
   //        nbinsy - number of bins on the y-axis
   //        ybins  - array of length (nbins + 1) with low-edges for each bin
   //                 ybins[nbinsy] ... lower edge for overflow y-bin

   Bool_t bStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   fTotalHistogram = new TH2D("total","total",nbinsx,xbins,nbinsy,ybins);
   fPassedHistogram = new TH2D("passed","passed",nbinsx,xbins,nbinsy,ybins);
   TH1::AddDirectory(bStatus);

   Build(name,title);
}

//______________________________________________________________________________
TEfficiency::TEfficiency(const char* name,const char* title,Int_t nbinsx,
			   Double_t xlow,Double_t xup,Int_t nbinsy,
			   Double_t ylow,Double_t yup,Int_t nbinsz,
			   Double_t zlow,Double_t zup):
   fBeta_alpha(kDefBetaAlpha),
   fBeta_beta(kDefBetaBeta),
   fConfLevel(kDefConfLevel),
   fDirectory(0),
   fFunctions(0),
   fPaintGraph(0),
   fPaintHisto(0),
   fWeight(kDefWeight)
{
   //create 3-dimensional TEfficiency object with fixed bin size
   //
   //constructor creates two new and empty histograms with a fixed binning
   //
   // Input: name   - the common part of the name for both histograms(no blanks)
   //                 fTotalHistogram has name: name + "_total"
   //                 fPassedHistogram has name: name + "_passed"
   //        title  - the common part of the title for both histogram
   //                 fTotalHistogram has title: title + " (total)"
   //                 fPassedHistogram has title: title + " (passed)"
   //                 It is possible to label the axis by passing a title with
   //                 the following format: "title;xlabel;ylabel;zlabel".
   //        nbinsx - number of bins on the x-axis
   //        xlow   - lower edge of first x-bin
   //        xup    - upper edge of last x-bin
   //        nbinsy - number of bins on the y-axis
   //        ylow   - lower edge of first y-bin
   //        yup    - upper edge of last y-bin
   //        nbinsz - number of bins on the z-axis
   //        zlow   - lower edge of first z-bin
   //        zup    - upper edge of last z-bin

   Bool_t bStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   fTotalHistogram = new TH3D("total","total",nbinsx,xlow,xup,nbinsy,ylow,yup,nbinsz,zlow,zup);
   fPassedHistogram = new TH3D("passed","passed",nbinsx,xlow,xup,nbinsy,ylow,yup,nbinsz,zlow,zup);
   TH1::AddDirectory(bStatus);

   Build(name,title);
}

//______________________________________________________________________________
TEfficiency::TEfficiency(const char* name,const char* title,Int_t nbinsx,
			   const Double_t* xbins,Int_t nbinsy,
			   const Double_t* ybins,Int_t nbinsz,
			   const Double_t* zbins):
   fBeta_alpha(kDefBetaAlpha),
   fBeta_beta(kDefBetaBeta),
   fConfLevel(kDefConfLevel),
   fDirectory(0),
   fFunctions(0),
   fPaintGraph(0),
   fPaintHisto(0),
   fWeight(kDefWeight)
{
   //create 3-dimensional TEfficiency object with variable bin size
   //
   //constructor creates two new and empty histograms with a given binning
   //
   // Input: name   - the common part of the name for both histograms(no blanks)
   //                 fTotalHistogram has name: name + "_total"
   //                 fPassedHistogram has name: name + "_passed"
   //        title  - the common part of the title for both histogram
   //                 fTotalHistogram has title: title + " (total)"
   //                 fPassedHistogram has title: title + " (passed)"
   //                 It is possible to label the axis by passing a title with
   //                 the following format: "title;xlabel;ylabel;zlabel".
   //        nbinsx - number of bins on the x-axis
   //        xbins  - array of length (nbins + 1) with low-edges for each bin
   //                 xbins[nbinsx] ... lower edge for overflow x-bin
   //        nbinsy - number of bins on the y-axis
   //        ybins  - array of length (nbins + 1) with low-edges for each bin
   //                 xbins[nbinsx] ... lower edge for overflow y-bin
   //        nbinsz - number of bins on the z-axis
   //        zbins  - array of length (nbins + 1) with low-edges for each bin
   //                 xbins[nbinsx] ... lower edge for overflow z-bin

   Bool_t bStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   fTotalHistogram = new TH3D("total","total",nbinsx,xbins,nbinsy,ybins,nbinsz,zbins);
   fPassedHistogram = new TH3D("passed","passed",nbinsx,xbins,nbinsy,ybins,nbinsz,zbins);
   TH1::AddDirectory(bStatus);

   Build(name,title);
}

//______________________________________________________________________________
TEfficiency::TEfficiency(const TEfficiency& rEff):
   TNamed(),
   TAttLine(),
   TAttFill(),
   TAttMarker(),
   fBeta_alpha(rEff.fBeta_alpha),
   fBeta_beta(rEff.fBeta_beta),
   fConfLevel(rEff.fConfLevel),
   fDirectory(0),
   fFunctions(0),
   fPaintGraph(0),
   fPaintHisto(0),
   fWeight(rEff.fWeight)
{
   //copy constructor
   //
   //The list of associated objects (e.g. fitted functions) is not copied.
   //
   //Note: - SetName(rEff.GetName() + "_copy") is called to set the names of the
   //        object and the histograms.
   //      - The titles are set by calling SetTitle("[copy] " + rEff.GetTitle()).
   //      - The copied TEfficiency object is NOT appended to a directory. It
   //        will not be written to disk during the next TFile::Write() command
   //        in order to prevent duplication of data. If you want to save this
   //        TEfficiency object anyway, you can either append it to a directory
   //        by calling SetDirectory(TDirectory*) or write it explicitly to disk
   //        by calling Write().

   Bool_t bStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   fTotalHistogram = (TH1*)((rEff.fTotalHistogram)->Clone());
   fPassedHistogram = (TH1*)((rEff.fPassedHistogram)->Clone());
   TH1::AddDirectory(bStatus);

   char buffer[100];
   strcpy(buffer,rEff.GetName());
   strcat(buffer,"_copy");
   SetName(buffer);
   strcpy(buffer,"[copy] ");
   strcat(buffer,rEff.GetTitle());
   SetTitle(buffer);
   
   SetStatisticOption(rEff.GetStatisticOption());

   SetDirectory(0);

   //copy style
   rEff.TAttLine::Copy(*this);
   rEff.TAttFill::Copy(*this);
   rEff.TAttMarker::Copy(*this);
}

//______________________________________________________________________________
TEfficiency::~TEfficiency()
{
   //default destructor

   //delete all function in fFunctions
   if(fFunctions) {
      
      TIter next(fFunctions);
      TObject* obj = 0;
      while((obj = next())) {
	 delete obj;
      }

      fFunctions->Delete();
   }

   if(fDirectory)
      fDirectory->Remove(this);
   
   delete fFunctions;
   delete fTotalHistogram;
   delete fPassedHistogram;
   delete fPaintGraph;
   delete fPaintHisto;
}

//______________________________________________________________________________
Double_t TEfficiency::AgrestiCoull(Int_t total,Int_t passed,Double_t level,Bool_t bUpper)
{
   //calculates the boundaries for the frequentist Agresti-Coull interval
   //
   //Input: - total : number of total events
   //       - passed: 0 <= number of passed events <= total
   //       - level : confidence level
   //       - bUpper: true  - upper boundary is returned
   //                 false - lower boundary is returned
   //
   //calculation:
   //Begin_Latex(separator='=',align='rl')
   // #alpha = 1 - #frac{level}{2}
   // #kappa = #Phi^{-1}(1 - #alpha,1) ... normal quantile function
   // mode = #frac{passed + #frac{#kappa^{2}}{2}}{total + #kappa^{2}}
   // #Delta = #kappa * #sqrt{#frac{mode * (1 - mode)}{total + #kappa^{2}}}
   // return =  max(0,mode - #Delta) or min(1,mode + #Delta)
   //End_Latex
   
   Double_t alpha = (1.0 - level)/2;
   Double_t kappa = ROOT::Math::normal_quantile(1 - alpha,1);

   Double_t mode = (passed + 0.5 * kappa * kappa) / (total + kappa * kappa);
   Double_t delta = kappa * std::sqrt(mode * (1 - mode) / (total + kappa * kappa));

   if(bUpper)
      return ((mode + delta) > 1) ? 1.0 : (mode + delta);
   else
      return ((mode - delta) < 0) ? 0.0 : (mode - delta);
}

//______________________________________________________________________________
Double_t TEfficiency::Bayesian(Int_t total,Int_t passed,Double_t level,Double_t alpha,Double_t beta,Bool_t bUpper)
{
   //calculates the boundaries for a baysian confidence interval
   //
   //Input: - total : number of total events
   //       - passed: 0 <= number of passed events <= total
   //       - level : confidence level
   //       - alpha : shape parameter > 0 for the prior distribution (fBeta_alpha)
   //       - beta  : shape parameter > 0 for the prior distribution (fBeta_beta)
   //       - bUpper: true  - upper boundary is returned
   //                 false - lower boundary is returned
   //
   //Note: The equal-tailed confidence interval is calculated which might be not
   //      the shortest interval containing the desired coverage probability.
   //
   //Calculation:
   //
   //The posterior probability in bayesian statistics is given by:
   //Begin_Latex P(#varepsilon |k,N) #propto L(#varepsilon|k,N) #times Prior(#varepsilon)End_Latex
   //As an efficiency can be interpreted as probability of a positive outcome of
   //a Bernoullli trial the likelihood function is given by the binomial
   //distribution:
   //Begin_Latex L(#varepsilon|k,N) = Binomial(N,k) #varepsilon ^{k} (1 - #varepsilon)^{N-k}End_Latex
   //At the moment only beta distributions are supported as prior probabilities
   //of the efficiency (Begin_Latex #scale[0.8]{B(#alpha,#beta)}End_Latex is the beta function):
   //Begin_Latex Prior(#varepsilon) = #frac{1}{B(#alpha,#beta)} #varepsilon ^{#alpha - 1} (1 - #varepsilon)^{#beta - 1}End_Latex
   //The posterior probability is therefore again given by a beta distribution:
   //Begin_Latex P(#varepsilon |k,N) #propto #varepsilon ^{k + #alpha - 1} (1 - #varepsilon)^{N - k + #beta - 1} End_Latex
   //The lower boundary for the equal-tailed confidence interval is given by the
   //inverse cumulative (= quantile) function for the quantile Begin_Latex #frac{1 - level}{2} End_Latex.
   //The upper boundary for the equal-tailed confidence interval is given by the
   //inverse cumulative (= quantile) function for the quantile Begin_Latex #frac{1 + level}{2} End_Latex.
   //Hence it is the solution Begin_Latex #varepsilon End_Latex of the following equation:
   //Begin_Latex I_{#varepsilon}(k + #alpha,N - k + #beta) = #frac{1}{norm} #int_{0}^{#varepsilon} dt t^{k + #alpha - 1} (1 - t)^{N - k + #beta - 1} =  #frac{1 #pm level}{2} End_Latex

   if(bUpper) {
      if((alpha > 0) && (beta > 0))
	 return (passed == total)? 1.0 : ROOT::Math::beta_quantile((1+level)/2,passed+alpha,total-passed+beta);
      else
	 return 1;
   }
   else {
      if((alpha > 0) && (beta > 0))
	 return (passed == 0)? 0.0 : ROOT::Math::beta_quantile((1-level)/2,passed+alpha,total-passed+beta);
      else
	 return 0;
   }
}

//______________________________________________________________________________
void TEfficiency::Build(const char* name,const char* title)
{
   //building standard data structure of a TEfficiency object
   //
   //Notes: - calls: SetName(name), SetTitle(title)
   //       - set the statistic option to the default (kFCP)
   //       - appends this object to the current directory
   //         SetDirectory(gDirectory)
   
   SetName(name);
   SetTitle(title);

   SetStatisticOption(kDefStatOpt);
   SetDirectory(gDirectory);

   //set normalisation factors to 0, otherwise the += may not work properly
   fPassedHistogram->SetNormFactor(0);
   fTotalHistogram->SetNormFactor(0);
}

//______________________________________________________________________________
Bool_t TEfficiency::CheckBinning(const TH1& pass,const TH1& total)
{
   //checks binning for each axis
   //
   //It is assumed that the passed histograms have the same dimension.
   
   TAxis* ax1 = 0;
   TAxis* ax2 = 0;
   
   //check binning along x axis
   for(Int_t j = 0; j < pass.GetDimension(); ++j) {
      switch(j) {
      case 0:
	 ax1 = pass.GetXaxis();
	 ax2 = total.GetXaxis();
	 break;
      case 1:
	 ax1 = pass.GetYaxis();
	 ax2 = total.GetYaxis();
	 break;
      case 2:
	 ax1 = pass.GetZaxis();
	 ax2 = total.GetZaxis();
	 break;
      }
      
      if(ax1->GetNbins() != ax2->GetNbins())
	 return false;
      if(ax1->GetXmin() != ax2->GetXmin())
	 return false;
      if(ax1->GetXmax() != ax2->GetXmax())
	 return false;

      for(Int_t i = 1; i <= ax1->GetNbins(); ++i)
	 if(ax1->GetBinLowEdge(i) != ax2->GetBinLowEdge(i))
	    return false;
   }

   return true;
}

//______________________________________________________________________________
Bool_t TEfficiency::CheckConsistency(const TH1& pass,const TH1& total,Option_t* opt)
{
   //checks the consistence of the given histograms
   //
   //The histograms are considered as consistent if:
   //- both have the same dimension
   //- both have the same binning
   //- pass.GetBinContent(i) <= total.GetBinContent(i) for each bin i
   //
   //Option: - w: The check for unit weights is skipped and therefore histograms
   //             filled with weights are accepted.
   
   if(pass.GetDimension() != total.GetDimension())
      return false;

   if(!CheckBinning(pass,total))
      return false;

   if(!CheckEntries(pass,total,opt))
      return false;

   return true;
}

//______________________________________________________________________________
Bool_t TEfficiency::CheckEntries(const TH1& pass,const TH1& total,Option_t* opt)
{
   //checks whether bin contents are compatible with binomial statistics
   //
   //The following inequality has to be valid for each bin i:
   // total.GetBinContent(i) >= pass.GetBinContent(i)
   //
   //and the histogram have to be filled with unit weights.
   //
   //Option: - w: Do not check for unit weights -> accept histograms filled with
   //             weights
   //
   //Note: - It is assumed that both histograms have the same dimension and
   //        binning.

   TString option = opt;
   option.ToLower();

   //check for unit weights
   if(!option.Contains("w")) {      
      Double_t statpass[10];
      Double_t stattotal[10];

      pass.GetStats(statpass);
      total.GetStats(stattotal);

      //require: sum of weights == sum of weights^2
      if((TMath::Abs(statpass[0]-statpass[1]) > 1e-5) ||
	 (TMath::Abs(stattotal[0]-stattotal[1]) > 1e-5))
	 return false;
   }
   
   //check: pass <= total
   Int_t nbinsx, nbinsy, nbinsz, nbins;

   nbinsx = pass.GetNbinsX();
   nbinsy = pass.GetNbinsY();
   nbinsz = pass.GetNbinsZ();

   switch(pass.GetDimension()) {
   case 1: nbins = nbinsx + 2; break;
   case 2: nbins = (nbinsx + 2) * (nbinsy + 2); break;
   case 3: nbins = (nbinsx + 2) * (nbinsy + 2) * (nbinsz + 2); break;
   default: nbins = 0;
   }
   
   for(Int_t i = 0; i < nbins; ++i) {
      if(pass.GetBinContent(i) > total.GetBinContent(i))
	 return false;
   }

   return true;
}

//______________________________________________________________________________
Double_t TEfficiency::ClopperPearson(Int_t total,Int_t passed,Double_t level,Bool_t bUpper)
{
   //calculates the boundaries for the frequentist Clopper-Pearson interval
   //
   //This interval is recommended by the PDG.
   //
   //Input: - total : number of total events
   //       - passed: 0 <= number of passed events <= total
   //       - level : confidence level
   //       - bUpper: true  - upper boundary is returned
   //                 false - lower boundary is returned
   //
   //calculation:
   //
   //The lower boundary of the Clopper-Pearson interval is the "exact" inversion
   //of the test:
   //Begin_Latex(separator='=',align='rl')
   //P(x #geq passed; total) = #frac{1 - level}{2}
   //P(x #geq passed; total) = 1 - P(x #leq passed - 1; total)
   // = 1 - #frac{1}{norm} * #int_{0}^{1 - #varepsilon} t^{total - passed} (1 - t)^{passed - 1} dt
   // = 1 - #frac{1}{norm} * #int_{#varepsilon}^{1} t^{passed - 1} (1 - t)^{total - passed} dt
   // = #frac{1}{norm} * #int_{0}^{#varepsilon} t^{passed - 1} (1 - t)^{total - passed} dt
   // = I_{#varepsilon}(passed,total - passed + 1)
   //End_Latex
   //The lower boundary is therfore given by the Begin_Latex #frac{1 - level}{2}End_Latex quantile
   //of the beta distribution.
   //
   //The upper boundary of the Clopper-Pearson interval is the "exact" inversion
   //of the test:
   //Begin_Latex(separator='=',align='rl')
   //P(x #leq passed; total) = #frac{1 - level}{2}
   //P(x #leq passed; total) = #frac{1}{norm} * #int_{0}^{1 - #varepsilon} t^{total - passed - 1} (1 - t)^{passed} dt
   // = #frac{1}{norm} * #int_{#varepsilon}^{1} t^{passed} (1 - t)^{total - passed - 1} dt
   // = 1 - #frac{1}{norm} * #int_{0}^{#varepsilon} t^{passed} (1 - t)^{total - passed - 1} dt
   // #Rightarrow 1 - #frac{1 - level}{2} = #frac{1}{norm} * #int_{0}^{#varepsilon} t^{passed} (1 - t)^{total - passed -1} dt
   // #frac{1 + level}{2} = I_{#varepsilon}(passed + 1,total - passed)
   //End_Latex
   //The upper boundary is therfore given by the Begin_Latex #frac{1 + level}{2}End_Latex quantile
   //of the beta distribution.
   //
   //Note: The connection between the binomial distribution and the regularized
   //      incomplete beta function Begin_Latex I_{#varepsilon}(#alpha,#beta)End_Latex has been used.
   
   Double_t alpha = (1.0 - level) / 2;
   if(bUpper)
      return ((passed == total) ? 1.0 : ROOT::Math::beta_quantile(1 - alpha,passed + 1,total-passed));
   else
      return ((passed == 0) ? 0.0 : ROOT::Math::beta_quantile(alpha,passed,total-passed+1.0));
}

//______________________________________________________________________________
TGraphAsymmErrors* TEfficiency::Combine(TCollection* pList,Option_t* option,
					 Int_t n,Double_t* p)
{
   //combines a list of 1-dimensional TEfficiency objects
   //
   //A TGraphAsymmErrors object is returned which contains the estimated
   //efficiency and its uncertainty for each bin.
   //If the combination fails, a zero pointer is returned.
   //
   //At the moment the combining is only implemented for bayesian statistics.
   //
   //Input:
   //- pList  :
   //- options  
   // + s     : strict combining; only TEfficiency objects with the same beta
   //           prior and the flag kIsBayesian == true are combined
   // + v     : verbose mode; print information about combining
   // + cl=x  : set confidence level (0 < cl < 1). If not specified, the
   //           confidence level of the first TEfficiency object is used.
   //- n      : number of weights (has to be the number of TEfficiency objects
   //           in pList)
   //           If no weights are passed, each TEfficiency object is weighted by
   //           Begin_Latex GetWeight() #times GetTotalEvents(bin) End_Latex
   //           Therefore the weights can differ from bin to bin.
   //- p      : array of length n with weights for each TEfficiency object in
   //           pList (p[0] correspond to pList->First ... p[n-1] -> pList->Last)
   //           The weights do not have to be normalised. They are global for each
   //           bin in the corresponding TEfficiency object.
   //
   //For each bin j the following calculation is done:
   //Begin_Html
   //Notation:
   //<ul>
   //<li>k = passed events</li>
   //<li>N = total evens</li>
   //<li>n = number of combined TEfficiency objects</li>
   //<li>i = index for numbering TEfficiency objects</li>
   //<li>w = global weight of TEfficiency object</li>
   //<li>j = index of currently handled bin</li>
   //</ul>
   //calculation:
   //<ol>
   //<li>The combined posterior distributions is calculated</li>
   //End_Html
   //Begin_Latex(separator='=',align='rl')
   //P_{comb,j}(#epsilon |{k_{i,j}}; {N_{i,j}}) = #frac{1}{sum p_{i}} #times #sum_{i} p_{i} #times P_{i,j}(#epsilon | k_{i,j}; N_{i,j})
   //p_{i} = p[i] if weights are passed, otherwise w_{i} #times N_{i,j}
   //End_Latex
   //Begin_Html
   //<li>The estimated efficiency is given by the mean value of the resulting
   //combined posterior distribution.</li>
   //<li>The boundaries of the confidence interval for a confidence level (1 - a)
   //are given by the a/2 and 1-a/2 quantiles of the resulting cumulative
   //distribution.</li>
   //</ol>
   //End_Html
   //Example (uniform prior distribution):
   //Begin_Macro
   //{
   //  TCanvas* c1 = new TCanvas("c1","",600,800);
   //  c1->Divide(1,2);
   //  c1->SetFillStyle(1001);
   //  c1->SetFillColor(kWhite);
   //
   //  TF1* p1 = new TF1("p1","TMath::BetaDist(x,18,8)",0,1);
   //  TF1* p2 = new TF1("p2","TMath::BetaDist(x,3,7)",0,1);
   //  TF1* comb = new TF1("comb","0.6*p1 + 0.4*p2",0,1);
   //  TF1* const1 = new TF1("const1","0.05",0,1);
   //  TF1* const2 = new TF1("const2","0.95",0,1);
   //
   //  p1->SetLineColor(kRed);
   //  p1->SetTitle("combined posteriors;#epsilon;P(#epsilon|k,N)");
   //  p2->SetLineColor(kBlue);
   //  comb->SetLineColor(kGreen+2);
   //
   //  TLegend* leg1 = new TLegend(0.2,0.65,0.6,0.85);
   //  leg1->AddEntry(p1,"k1 = 18, N1 = 26","l");
   //  leg1->AddEntry(p2,"k2 = 3, N2 = 10","l");
   //  leg1->AddEntry(comb,"combined: p1 = 0.6, p2=0.4","l");
   
   //  c1->cd(1);
   //  p1->Draw();
   //  p2->Draw("same");
   //  comb->Draw("same");
   //  leg1->Draw("same");
   //  c1->cd(2);
   //  const1->SetLineWidth(1);
   //  const2->SetLineWidth(1);
   //  TGraph* gr = (TGraph*)comb->DrawIntegral();
   //  gr->SetTitle("cumulative function of combined posterior with boundaries for cl = 95%;#epsilon;CDF");   
   //  const1->Draw("same");
   //  const2->Draw("same");   
   //
   //  c1->cd(0);
   //  return c1;
   //}
   //End_Macro
   
   TString opt = option;
   opt.ToLower();

   //parameter of prior distribution, confidence level and normalisation factor
   Double_t alpha = 0;
   Double_t beta = 0;
   Double_t level = 0;
   
   //flags for combining
   Bool_t bStrict = false;
   Bool_t bOutput = false;
   Bool_t bWeights = false;
   //list of all information needed to weight and combine efficiencies
   std::vector<TH1*> vTotal;
   std::vector<TH1*> vPassed;
   std::vector<Double_t> vWeights;
   std::vector<Double_t> vAlpha;
   std::vector<Double_t> vBeta;

   if(opt.Contains("s"))
      bStrict = true;

   if(opt.Contains("v"))
      bOutput = true;

   if(opt.Contains("cl=")) {
      sscanf(strstr(opt.Data(),"cl="),"cl=%lf",&level);
      if((level <= 0) && (level >= 1))
	 level = 0;
   }

   //are weights explicitly given
   if(n && p) {
      bWeights = true;
      for(Int_t k = 0; k < n; ++k) {
	 if(p[k] > 0)
	    vWeights.push_back(p[k]);
	 else {
	    gROOT->Error("TEfficiency::Combine","invalid custom weight found w = %.2lf",p[k]);
	    gROOT->Info("TEfficiency::Combine","stop combining");
	    return 0;
	 }
      }
   }
   
   TIter next(pList);
   TObject* obj = 0;
   TEfficiency* pEff = 0;
   while((obj = next())) {
      pEff = dynamic_cast<TEfficiency*>(obj);
      //is object a TEfficiency object?
      if(pEff) {
	 if(pEff->GetDimension() > 1)
	    continue;
	 if(!level) level = pEff->GetConfidenceLevel();
	 
	 //if strict combining, check priors, confidence level and statistic
	 if(bStrict) {
	    if(!alpha) alpha = pEff->GetBetaAlpha();
	    if(!beta) beta = pEff->GetBetaBeta();
	 
	    if(alpha != pEff->GetBetaAlpha())
	       continue;	    
	    if(beta != pEff->GetBetaBeta())
	       continue;
	    if(!pEff->UsesBayesianStat())
	       continue;
	 }
	
	 vTotal.push_back(pEff->fTotalHistogram);
	 vPassed.push_back(pEff->fPassedHistogram);

	 //no weights given -> use weights of TEfficiency objects
	 if(!bWeights)
	    vWeights.push_back(pEff->fWeight);

	 //strict combining -> using global prior
	 if(bStrict) {
	    vAlpha.push_back(alpha);
	    vBeta.push_back(beta);
	 }
	 else {
	    vAlpha.push_back(pEff->GetBetaAlpha());
	    vBeta.push_back(pEff->GetBetaBeta());
	 }
      }
   }

   //no TEfficiency objects found
   if(vTotal.empty()) {
      gROOT->Error("TEfficiency::Combine","no TEfficiency objects in given list");
      gROOT->Info("TEfficiency::Combine","stop combining");
      return 0;
   }

   //invalid number of custom weights
   if(bWeights && (n != (Int_t)vTotal.size())) {
      gROOT->Error("TEfficiency::Combine","number of weights n=%i differs from number of TEfficiency objects k=%i which should be combined",n,(Int_t)vTotal.size());
      gROOT->Info("TEfficiency::Combine","stop combining");
      return 0;
   }

   Int_t nbins_max = vTotal.at(0)->GetNbinsX();
   //check consistency of all histograms
   for(UInt_t i=0; i<vTotal.size(); ++i) {
      try {
	 TEfficiency::CheckConsistency(*vTotal.at(0),*vTotal.at(i));
      }
      catch(std::exception&) {
	 gROOT->Warning("TEfficiency::Combine","histograms are not consistent -> results may be useless");
      }
      if(vTotal.at(i)->GetNbinsX() < nbins_max) nbins_max = vTotal.at(i)->GetNbinsX();
   }

   //display information about combining
   if(bOutput) {
      gROOT->Info("TEfficiency::Combine","combining %i TEfficiency objects",(Int_t)vTotal.size());
      if(bWeights)
	 gROOT->Info("TEfficiency::Combine","using custom weights");
      if(bStrict) {
	 gROOT->Info("TEfficiency::Combine","using the following prior probability for the efficiency: P(e) ~ Beta(e,%.3lf,%.3lf)",alpha,beta);
      }
      else
	 gROOT->Info("TEfficiency::Combine","using individual priors of each TEfficiency object");
      gROOT->Info("TEfficiency::Combine","confidence level = %.2lf",level);
   }

   //create TGraphAsymmErrors with efficiency
   Double_t* x = new Double_t[nbins_max];
   Double_t* xlow = new Double_t[nbins_max];
   Double_t* xhigh = new Double_t[nbins_max];
   Double_t* eff = new Double_t[nbins_max];
   Double_t* efflow = new Double_t[nbins_max];
   Double_t* effhigh = new Double_t[nbins_max];

   TString formula;
   char* sub = 0;
   TF1* pdf = 0;
   Double_t w = 0;
   Double_t totalweight = 0;

   //loop over all bins
   for(Int_t i=1; i <= nbins_max; ++i) {
      //the binning of the x-axis is taken from the first total histogram
      x[i-1] = vTotal.at(0)->GetBinCenter(i);
      xlow[i-1] = x[i-1] - vTotal.at(0)->GetBinLowEdge(i);
      xhigh[i-1] = vTotal.at(0)->GetBinWidth(i) - xlow[i-1];
      //reset normalisation factor
      totalweight = 0;
      //create formula string:
      //(\sum_i weight_i * Beta(x;passed_i + alpha,total_i - passed_i + beta) )/totalweight
      formula.Clear();
      formula = "( 0 ";
      //add the sum
      for(UInt_t j=0; j < vTotal.size(); ++j) {
	 //use custom weights
	 if(bWeights)
	    w = vWeights.at(j);
	 //use generic weight = weight * N_total
	 else
	    w = vWeights.at(j)*(vTotal.at(j)->GetBinContent(i));
	 
	 totalweight += w;
	 
	 sub = new char[100];
	 sprintf(sub,"+ %lf * TMath::BetaDist(x,%lf,%lf) ",w,
		 vPassed.at(j)->GetBinContent(i)+vAlpha.at(j),vTotal.at(j)->GetBinContent(i)-
		 vPassed.at(j)->GetBinContent(i)+vBeta.at(j));
	 formula.Append(sub);
	 delete sub;
	 sub = 0;
      }
      //divide by total weight
      sub = new char[20];
      sprintf(sub,")/%lf",totalweight);
      formula.Append(sub);
      delete sub;
      sub = 0;
      
      pdf = new TF1("pdf",formula.Data(),0,1);

      //fill efficiency and errors
      Double_t q[2];
      Double_t quantil[2] = {(1-level)/2,(1+level)/2};
      pdf->GetQuantiles(2,q,quantil);
      eff[i-1] = pdf->Mean(0,1);
      efflow[i-1]= eff[i-1] - q[0];
      effhigh[i-1]= q[1] - eff[i-1];

      delete pdf;
      pdf = 0;      
   }//loop over all bins

   TGraphAsymmErrors* gr = new TGraphAsymmErrors(nbins_max,x,eff,xlow,xhigh,efflow,effhigh);

   delete [] x;
   delete [] xlow;
   delete [] xhigh;
   delete [] eff;
   delete [] efflow;
   delete [] effhigh;

   return gr;
}

//______________________________________________________________________________
void TEfficiency::Draw(const Option_t* opt)
{
   //draws the current TEfficiency object
   //
   //options:
   //- 1-dimensional case: same options as TGraphAsymmErrors::Draw()
   //- 2-dimensional case: same options as TH2::Draw()
   //- 3-dimensional case: not yet supported
   
   //check options
   TString option = opt;
   option.ToLower();

   if(gPad && !option.Contains("same"))
      gPad->Clear();

   AppendPad(option.Data());
}

//______________________________________________________________________________
void TEfficiency::Fill(Bool_t bPassed,Double_t x,Double_t y,Double_t z)
{
   //This function is used for filling the two histograms.
   //
   //Input: bPassed - flag whether the current event passed the selection
   //                 true: both histograms are filled
   //                 false: only the total histogram is filled
   //       x       - x value
   //       y       - y value (use default=0 for 1-D efficiencies)
   //       z       - z value (use default=0 for 2-D or 1-D efficiencies)

   switch(GetDimension()) {
   case 1:
      fTotalHistogram->Fill(x);
      if(bPassed)
	 fPassedHistogram->Fill(x);
      break;
   case 2:
      ((TH2*)(fTotalHistogram))->Fill(x,y);
      if(bPassed)
	 ((TH2*)(fPassedHistogram))->Fill(x,y);
      break;
   case 3:
      ((TH3*)(fTotalHistogram))->Fill(x,y,z);
      if(bPassed)
	 ((TH3*)(fPassedHistogram))->Fill(x,y,z);
      break;
   }
}

//______________________________________________________________________________
Int_t TEfficiency::FindFixBin(Double_t x,Double_t y,Double_t z) const
{
   //returns the global bin number containing the given values
   //
   //Note: - values which belong to dimensions higher than the current dimension
   //        of the TEfficiency object are ignored (i.e. for 1-dimensional
   //        efficiencies only the x-value is considered)
   
   Int_t nx = fTotalHistogram->GetXaxis()->FindFixBin(x);
   Int_t ny = 0;
   Int_t nz = 0;
   
   switch(GetDimension()) {
   case 3: nz = fTotalHistogram->GetZaxis()->FindFixBin(z);
   case 2: ny = fTotalHistogram->GetYaxis()->FindFixBin(y);break;
   }

   return GetGlobalBin(nx,ny,nz);
}

//______________________________________________________________________________
Int_t TEfficiency::Fit(TF1* f1,Option_t* opt)
{
   //fits the efficiency using the TBinomialEfficiencyFitter class
   //
   //The resulting fit function is added to the list of associated functions.
   //
   //Options: - "+": previous fitted functions in the list are kept, by default
   //                all functions in the list are deleted
   //         - for more fitting options see TBinomialEfficiencyFitter::Fit

   TString option = opt;
   option.ToLower();

   //replace existing functions in list with same name
   Bool_t bDeleteOld = true;
   if(option.Contains("+")) {
      option.ReplaceAll("+","");
      bDeleteOld = false;
   }
   
   TBinomialEfficiencyFitter Fitter(fPassedHistogram,fTotalHistogram);
   
   Int_t result = Fitter.Fit(f1,option.Data());

   //create copy which is appended to the list
   TF1* pFunc = new TF1(*f1);
   
   if(!fFunctions)
      fFunctions = new TList();

   if(bDeleteOld) {
      TIter next(fFunctions);
      TObject* obj = 0;
      while((obj = next())) {
	 if(obj->InheritsFrom(TF1::Class())) {
	    fFunctions->Remove(obj);
	    delete obj;
	 }
      }      
   }
   
   fFunctions->Add(pFunc);
   
   return result;
}

//______________________________________________________________________________
TH1* TEfficiency::GetCopyPassedHisto() const
{
   //returns a cloned version of fPassedHistogram
   //
   //Notes: - The histogram is filled with unit weights. You might want to scale
   //         it with the global weight GetWeight().
   //       - The returned object is owned by the user who has to care about the
   //         deletion of the new TH1 object.
   //       - This histogram is by default NOT attached to the current directory
   //         to avoid duplication of data. If you want to store it automatically
   //         during the next TFile::Write() command, you have to attach it to
   //         the corresponding directory.
   //Begin_html
   //<div class="code"><pre>
   //  TFile* pFile = new TFile("passed.root","update");
   //  TEfficiency* pEff = (TEfficiency*)gDirectory->Get("my_eff");
   //  TH1* copy = pEff->GetCopyPassedHisto();
   //  copy->SetDirectory(gDirectory);
   //  pFile->Write();
   //</pre></div>
   //<div class="clear"></div>
   //End_Html
    
   Bool_t bStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   TH1* tmp = (TH1*)(fPassedHistogram->Clone());
   TH1::AddDirectory(bStatus);

   return tmp;
}

//______________________________________________________________________________
TH1* TEfficiency::GetCopyTotalHisto() const
{
   //returns a cloned version of fTotalHistogram
   //
   //Notes: - The histogram is filled with unit weights. You might want to scale
   //         it with the global weight GetWeight().
   //       - The returned object is owned by the user who has to care about the
   //         deletion of the new TH1 object.
   //       - This histogram is by default NOT attached to the current directory
   //         to avoid duplication of data. If you want to store it automatically
   //         during the next TFile::Write() command, you have to attach it to
   //         the corresponding directory.
   //Begin_Html
   //<div class="code"><pre>
   //  TFile* pFile = new TFile("total.root","update");
   //  TEfficiency* pEff = (TEfficiency*)gDirectory->Get("my_eff");
   //  TH1* copy = pEff->GetCopyTotalHisto();
   //  copy->SetDirectory(gDirectory);
   //  pFile->Write();
   //</pre></div>
   //<div class="clear"></div>
   //End_Html
   
   Bool_t bStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   TH1* tmp = (TH1*)(fTotalHistogram->Clone());
   TH1::AddDirectory(bStatus);

   return tmp;
}

//______________________________________________________________________________
Int_t TEfficiency::GetDimension() const
{
   //returns the dimension of the current TEfficiency object

   return fTotalHistogram->GetDimension();
}

//______________________________________________________________________________
Double_t TEfficiency::GetEfficiency(Int_t bin) const
{
   //returns the efficiency in the given global bin
   //
   //Note: - The estimated efficiency depends on the chosen statistic option:
   //        for frequentist ones:
   //        Begin_Latex #hat{#varepsilon} = #frac{passed}{total} End_Latex
   //        for bayesian ones the expectation value of the resulting posterior
   //        distribution is returned:
   //        Begin_Latex #hat{#varepsilon} = #frac{passed + #alpha}{total + #alpha + #beta} End_Latex
   //      - If the denominator is equal to 0, an efficiency of 0 is returned.
   
   Int_t total = (Int_t)fTotalHistogram->GetBinContent(bin);
   Int_t passed = (Int_t)fPassedHistogram->GetBinContent(bin);
      
   if(TestBit(kIsBayesian))
      return (total + fBeta_alpha + fBeta_beta)?
	 (passed + fBeta_alpha)/(total + fBeta_alpha + fBeta_beta): 0;
   else
      return (total)? ((Double_t)passed)/total : 0;
}

//______________________________________________________________________________
Double_t TEfficiency::GetEfficiencyErrorLow(Int_t bin) const
{
   //returns the lower error on the efficiency in the given global bin
   //
   //The result depends on the current confidence level fConfLevel and the
   //chosen statistic option fStatisticOption. See SetStatisticOption(Int_t) for
   //more details.

   Int_t total = (Int_t)fTotalHistogram->GetBinContent(bin);
   Int_t passed = (Int_t)fPassedHistogram->GetBinContent(bin);

   Double_t eff = GetEfficiency(bin);

   if(TestBit(kIsBayesian))
      return (eff - Bayesian(total,passed,fConfLevel,fBeta_alpha,fBeta_beta,false));
   else
      return (eff - fBoundary(total,passed,fConfLevel,false));
}

//______________________________________________________________________________
Double_t TEfficiency::GetEfficiencyErrorUp(Int_t bin) const
{   
   //returns the upper error on the efficiency in the given global bin
   //
   //The result depends on the current confidence level fConfLevel and the
   //chosen statistic option fStatisticOption. See SetStatisticOption(Int_t) for
   //more details.
   
   Int_t total = (Int_t)fTotalHistogram->GetBinContent(bin);
   Int_t passed = (Int_t)fPassedHistogram->GetBinContent(bin);

   Double_t eff = GetEfficiency(bin);

   if(TestBit(kIsBayesian))
      return (Bayesian(total,passed,fConfLevel,fBeta_alpha,fBeta_beta,true) - eff);
   else
      return fBoundary(total,passed,fConfLevel,true) - eff;
}

//______________________________________________________________________________
Int_t TEfficiency::GetGlobalBin(Int_t binx,Int_t biny,Int_t binz) const
{
   //returns the global bin number which can be used as argument for the
   //following functions:
   //
   // - GetEfficiency(bin), GetEfficiencyErrorLow(bin), GetEfficiencyErrorUp(bin)
   // - GetPassedEvents(bin), SetPassedEvents(bin), GetTotalEvents(bin),
   //   SetTotalEvents(bin)
   //
   //see TH1::GetBin() for conventions on numbering bins
   
   return fTotalHistogram->GetBin(binx,biny,binz);
}

//______________________________________________________________________________
void TEfficiency::Merge(TCollection* pList)
{
   //merges the TEfficiency objects in the given list to the given
   //TEfficiency object using the operator+=(TEfficiency&)
   //
   //The merged result is stored in the current object. The statistic options and
   //the confidence level are taken from the current object.
   //
   //This function should be used when all TEfficiency objects correspond to
   //the same process.
   //
   //The new weight is set according to:
   //Begin_Latex #frac{1}{w_{new}} = #sum_{i} \frac{1}{w_{i}}End_Latex 
   
   if(pList->IsEmpty())
      return;
   
   TIter next(pList);
   TObject* obj = 0;
   TEfficiency* pEff = 0;
   while((obj = next())) {
      pEff = dynamic_cast<TEfficiency*>(obj);
      if(pEff)
	 *this += *pEff;
   }
}

//______________________________________________________________________________
Double_t TEfficiency::Normal(Int_t total,Int_t passed,Double_t level,Bool_t bUpper)
{
   //returns the confidence limits for the efficiency supposing that the
   //efficiency follows a normal distribution with the rms below
   //
   //Input: - total : number of total events
   //       - passed: 0 <= number of passed events <= total
   //       - level : confidence level
   //       - bUpper: true  - upper boundary is returned
   //                 false - lower boundary is returned
   //
   //calculation:
   //Begin_Latex(separator='=',align='rl')
   // #hat{#varepsilon} = #frac{passed}{total}
   // #sigma_{#varepsilon} = #sqrt{#frac{#hat{#varepsilon} (1 - #hat{#varepsilon})}{total}}
   // #varepsilon_{low} = #hat{#varepsilon} #pm #Phi^{-1}(#frac{level}{2},#sigma_{#varepsilon})
   //End_Latex

   Double_t alpha = (1.0 - level)/2;
   Double_t average = ((Double_t)passed) / total;
   Double_t sigma = std::sqrt(average * (1 - average) / total);
   Double_t delta = ROOT::Math::normal_quantile(1 - alpha,sigma);

   if(bUpper)
      return ((average + delta) > 1) ? 1.0 : (average + delta);
   else
      return ((average - delta) < 0) ? 0.0 : (average - delta);
}

//______________________________________________________________________________
TEfficiency& TEfficiency::operator+=(const TEfficiency& rhs)
{
   //adds the histograms of another TEfficiency object to current histograms
   //
   //The statistic options and the confidence level remain unchanged.
   //
   //fTotalHistogram += rhs.fTotalHistogram;
   //fPassedHistogram += rhs.fPassedHistogram;
   //
   //calculates a new weight:
   //current weight of this TEfficiency object = Begin_Latex w_{1} End_Latex
   //weight of rhs = Begin_Latex w_{2} End_Latex
   //Begin_Latex w_{new} = \frac{w_{1} \times w_{2}}{w_{1} + w_{2}}End_Latex
   
   fTotalHistogram->ResetBit(TH1::kIsAverage);
   fPassedHistogram->ResetBit(TH1::kIsAverage);

   fTotalHistogram->Add(rhs.fTotalHistogram);
   fPassedHistogram->Add(rhs.fPassedHistogram);

   SetWeight((fWeight * rhs.GetWeight())/(fWeight + rhs.GetWeight()));
   
   return *this;
}

//______________________________________________________________________________
TEfficiency& TEfficiency::operator=(const TEfficiency& rhs)
{
   //assignment operator
   //
   //The histograms, statistic option, confidence level, weight and paint styles
   //of rhs are copied to the this TEfficiency object.
   //
   //Note: - The list of associated functions is not copied. After this
   //        operation the list of associated functions is empty.
   
   if(this != &rhs)
   {
      //statistic options
      SetStatisticOption(rhs.GetStatisticOption());
      SetConfidenceLevel(rhs.GetConfidenceLevel());
      SetBetaAlpha(rhs.GetBetaAlpha());
      SetBetaBeta(rhs.GetBetaBeta());
      SetWeight(rhs.GetWeight());
      
      //associated list of functions
      if(fFunctions)
	 fFunctions->Delete();

      //copy histograms
      delete fTotalHistogram;
      delete fPassedHistogram;

      Bool_t bStatus = TH1::AddDirectoryStatus();
      TH1::AddDirectory(kFALSE);
      fTotalHistogram = (TH1*)(rhs.fTotalHistogram->Clone());
      fPassedHistogram = (TH1*)(rhs.fPassedHistogram->Clone());
      TH1::AddDirectory(bStatus);

      //delete temporary paint objects
      delete fPaintHisto;
      delete fPaintGraph;
      fPaintHisto = 0;
      fPaintGraph = 0;

      //copy style
      rhs.TAttLine::Copy(*this);
      rhs.TAttFill::Copy(*this);
      rhs.TAttMarker::Copy(*this);
   }

   return *this;
}

//______________________________________________________________________________
void TEfficiency::Paint(const Option_t* opt)
{
   //paints this TEfficiency object
   //
   //For details on the possible option see Draw(Option_t*)
   
   if(!gPad)
      return;
   
   TString option = opt;
   option.ToLower();

   //use TGraphAsymmErrors for painting
   if(GetDimension() == 1) {
      Int_t npoints = fTotalHistogram->GetNbinsX();
      if(!fPaintGraph) {
	 fPaintGraph = new TGraphAsymmErrors(npoints);
	 fPaintGraph->SetName("eff_graph");
      }
      //refresh title before painting
      fPaintGraph->SetTitle(GetTitle());

      //errors for points
      Double_t xlow,xup,ylow,yup;
      //point i corresponds to bin i+1 in histogram
      for(Int_t i = 0; i < npoints; ++i) {
	 fPaintGraph->SetPoint(i,fTotalHistogram->GetBinCenter(i+1),GetEfficiency(i+1));
	 xlow = fTotalHistogram->GetBinCenter(i+1) - fTotalHistogram->GetBinLowEdge(i+1);
	 xup = fTotalHistogram->GetBinWidth(i+1) - xlow;
	 ylow = GetEfficiencyErrorLow(i+1);
	 yup = GetEfficiencyErrorUp(i+1);
	 fPaintGraph->SetPointError(i,xlow,xup,ylow,yup);
      }

      //copying style information
      TAttLine::Copy(*fPaintGraph);
      TAttFill::Copy(*fPaintGraph);
      TAttMarker::Copy(*fPaintGraph);
      
      //paint graph      
      fPaintGraph->Paint(option.Data());

      //paint all associated functions
      if(fFunctions) {
	 //paint box with fit parameters
	 gStyle->SetOptFit(1);
	 TIter next(fFunctions);
	 TObject* obj = 0;
	 while((obj = next())) {
	    if(obj->InheritsFrom(TF1::Class())) {
               fPaintGraph->PaintStats((TF1*)obj);
	       ((TF1*)obj)->Paint("sameC");
	    }
	 }
      }
      
      return;
   }

   //use TH2 for painting
   if(GetDimension() == 2) {
      Int_t nbinsx = fTotalHistogram->GetNbinsX();
      Int_t nbinsy = fTotalHistogram->GetNbinsY();
      if(!fPaintHisto) {
	 fPaintHisto = new TH2F("eff_histo",GetTitle(),nbinsx,fTotalHistogram->GetXaxis()->GetXbins()->GetArray(),
				nbinsy,fTotalHistogram->GetYaxis()->GetXbins()->GetArray());
	 fPaintHisto->SetDirectory(0);
      }
      //refresh title before each painting
      fPaintHisto->SetTitle(GetTitle());

      Int_t bin;
      for(Int_t i = 0; i < nbinsx + 2; ++i) {
	 for(Int_t j = 0; j < nbinsy + 2; ++j) {
	    bin = GetGlobalBin(i,j);
	    fPaintHisto->SetBinContent(bin,GetEfficiency(bin));
	 }
      }

      //copying style information
      TAttLine::Copy(*fPaintHisto);
      TAttFill::Copy(*fPaintHisto);
      TAttMarker::Copy(*fPaintHisto);
      fPaintHisto->SetStats(0);

      //paint histogram
      fPaintHisto->Paint(option.Data());
      return;
   }
}

//______________________________________________________________________________
void TEfficiency::SetBetaAlpha(Double_t alpha)
{
   //sets the shape parameter Begin_Latex \alpha End_Latex
   //
   //The prior probability of the efficiency is given by the beta distribution:
   //Begin_Latex
   // f(\varepsilon;\alpha;\beta) = \frac{1}{B(\alpha,\beta)} \varepsilon^{\alpha-1} (1 - \varepsilon)^{\beta-1}
   //End_Latex
   //
   //Note: - both shape parameters have to be positive (i.e. > 0)
   
   if(alpha > 0)
      fBeta_alpha = alpha;
   else
      Warning("SetBetaAlpha(Double_t)","invalid shape parameter %.2lf",alpha);
}

//______________________________________________________________________________
void TEfficiency::SetBetaBeta(Double_t beta)
{   
   //sets the shape parameter Begin_Latex \beta End_Latex
   //
   //The prior probability of the efficiency is given by the beta distribution:
   //Begin_Latex
   // f(\varepsilon;\alpha,\beta) = \frac{1}{B(\alpha,\beta)} \varepsilon^{\alpha-1} (1 - \varepsilon)^{\beta-1}
   //End_Latex
   //
   //Note: - both shape parameters have to be positive (i.e. > 0)
 
   if(beta > 0)
      fBeta_beta = beta;
   else
      Warning("SetBetaBeta(Double_t)","invalid shape parameter %.2lf",beta);
}

//______________________________________________________________________________
void TEfficiency::SetConfidenceLevel(Double_t level)
{
   //sets the confidence level (0 < level < 1)

   if((level > 0) && (level < 1))
      fConfLevel = level;
   else
      Warning("SetConfidenceLevel(Double_t)","invalid confidence level %.2lf",level);
}

//______________________________________________________________________________
void TEfficiency::SetDirectory(TDirectory* dir)
{
   //sets the directory holding this TEfficiency object
   //
   //A reference to this TEfficiency object is removed from the current
   //directory (if it exists) and a new reference to this TEfficiency object is
   //added to the given directory.
   //
   //Notes: - If the given directory is 0, the TEfficiency object does not
   //         belong to any directory and will not be written to file during the
   //         next TFile::Write() command.

   if(fDirectory == dir)
      return;
   if(fDirectory)
      fDirectory->Remove(this);
   fDirectory = dir;
   if(fDirectory)
      fDirectory->Append(this);      
}

//______________________________________________________________________________
void TEfficiency::SetName(const char* name)
{
   //sets the name
   //
   //Note: The names of the internal histograms are set to "name + _total" or
   //      "name + _passed" respectively.
   
   TNamed::SetName(name);
   
   //setting the names (appending the correct ending)
   char* namebuf = new char[strlen(name)+8];
   strcpy(namebuf,name);
   strcat(namebuf,"_total");
   fTotalHistogram->SetName(namebuf);
   strcpy(namebuf,name);
   strcat(namebuf,"_passed");
   fPassedHistogram->SetName(namebuf);
   delete namebuf;
}

//______________________________________________________________________________
Bool_t TEfficiency::SetPassedEvents(Int_t bin,Int_t events)
{
   //sets the number of passed events in the given global bin
   //
   //returns "true" if the number of passed events has been updated
   //otherwise "false" ist returned
   //
   //Note: - requires: 0 <= events <= fTotalHistogram->GetBinContent(bin)

   if(events <= fTotalHistogram->GetBinContent(bin)) {
      fPassedHistogram->SetBinContent(bin,events);
      return true;
   }
   else {
      Error("SetPassedEvents(Int_t,Int_t)","total number of events (%.1lf) in bin %i is less than given number of passed events %i",fTotalHistogram->GetBinContent(bin),bin,events);
      return false;
   }
}

//______________________________________________________________________________
Bool_t TEfficiency::SetPassedHistogram(const TH1& rPassed,Option_t* opt)
{
   //sets the histogram containing the passed events
   //
   //The given histogram is cloned and stored internally as histogram containing
   //the passed events. The given histogram has to be consistent with the current
   //fTotalHistogram (see CheckConsistency(const TH1&,const TH1&)).
   //The method returns whether the fPassedHistogram has been replaced (true) or
   //not (false).
   //
   //Note: The list of associated functions fFunctions is cleared.
   //
   //Option: - "f": force the replacement without checking the consistency
   //               This can lead to inconsistent histograms and useless results
   //               or unexpected behaviour. But sometimes it might be the only
   //               way to change the histograms. If you use this option, you
   //               should ensure that the fTotalHistogram is replaced by a
   //               consistent one (with respect to rPassed) as well.

   TString option = opt;
   option.ToLower();

   Bool_t bReplace = option.Contains("f");

   if(!bReplace)
      bReplace = CheckConsistency(rPassed,*fTotalHistogram);

   if(bReplace) {
      delete fPassedHistogram;
      Bool_t bStatus = TH1::AddDirectoryStatus();
      TH1::AddDirectory(kFALSE);
      fPassedHistogram = (TH1*)(rPassed.Clone());
      fPassedHistogram->SetNormFactor(0);
      TH1::AddDirectory(bStatus);

      if(fFunctions)
	 fFunctions->Delete();

      return true;
   }
   else
      return false;
}

//______________________________________________________________________________
void TEfficiency::SetStatisticOption(Int_t option)
{
   //sets the statistic option which affects the calculation of the confidence interval
   //
   //Options:
   //- kFCP (=0)(default): using the Clopper-Pearson interval (recommended by PDG)
   //                      sets kIsBayesian = false
   //                      see also ClopperPearson
   //- kFNormal   (=1)   : using the normal approximation
   //                      sets kIsBayesian = false
   //                      see also Normal
   //- kFWilson   (=2)   : using the Wilson interval
   //                      sets kIsBayesian = false
   //                      see also Wilson
   //- kFAC       (=3)   : using the Agresti-Coull interval
   //                      sets kIsBayesian = false
   //                      see also AgrestiCoull
   //- kBJeffrey  (=4)   : using the Jeffrey interval
   //                      sets kIsBayesian = true, fBeta_alpha = 0.5 and fBeta_beta = 0.5
   //                      see also Bayesian
   //- kBUniform  (=5)   : using a uniform prior
   //                      sets kIsBayesian = true, fBeta_alpha = 1 and fBeta_beta = 1
   //                      see also Bayesian
   //- kBBayesian (=6)   : using a custom prior defined by fBeta_alpha and fBeta_beta
   //                      sets kIsBayesian = true
   //                      see also Bayesian
   
   fStatisticOption = option;

   switch(option)
   {
   case kFCP:      
      fBoundary = &ClopperPearson;
      SetBit(kIsBayesian,false);
      break;
   case kFNormal:      
      fBoundary = &Normal;
      SetBit(kIsBayesian,false);
      break;
   case kFWilson:      
      fBoundary = &Wilson;
      SetBit(kIsBayesian,false);
      break;
   case kFAC:
      fBoundary = &AgrestiCoull;
      SetBit(kIsBayesian,false);
      break;
   case kBJeffrey:
      fBeta_alpha = 0.5;
      fBeta_beta = 0.5;
      SetBit(kIsBayesian,true);
      break;
   case kBUniform:
      fBeta_alpha = 1;
      fBeta_beta = 1;
      SetBit(kIsBayesian,true);
      break;
   case kBBayesian:
      SetBit(kIsBayesian,true);
      break;
   default:
      fStatisticOption = kFCP;
      fBoundary = &ClopperPearson;
      SetBit(kIsBayesian,false);
   }
}

//______________________________________________________________________________
void TEfficiency::SetTitle(const char* title)
{
   //sets the title
   //
   //Notes: - The titles of the internal histograms are set to "title + (total)"
   //         or "title + (passed)" respectively.
   //       - It is possible to label the axis of the histograms as usual (see
   //         TH1::SetTitle).
   //
   //Example: Setting the title to "My Efficiency" and label the axis
   //Begin_Html
   //<div class="code"><pre>
   //pEff->SetTitle("My Efficiency;x label;eff");
   //</pre></div>
   //<div class="clear"></div>
   //End_Html
   
   TNamed::SetTitle(title);
   
   //setting the titles (looking for the first semicolon and insert the tokens there)
   char* titlebuf = new char[strlen(title)+10];
   strcpy(titlebuf,title);
   char* semicolon = strchr(titlebuf,';');
   if(semicolon != 0)
   {
      char* axistitles = new char[strlen(titlebuf)];
      strcpy(axistitles,semicolon);
      
      strcpy(semicolon," (total)\0");
      strcat(titlebuf,axistitles);
      fTotalHistogram->SetTitle(titlebuf);
      strcpy(semicolon," (passed)\0");
      strcat(titlebuf,axistitles);
      fPassedHistogram->SetTitle(titlebuf);

      delete [] axistitles;
   }
   else
   {
      strcat(titlebuf," (total)");
      fTotalHistogram->SetTitle(titlebuf);
      strcpy(titlebuf,title);
      strcat(titlebuf," (passed)");
      fPassedHistogram->SetTitle(titlebuf);
   }

   delete [] titlebuf;
}

//______________________________________________________________________________
Bool_t TEfficiency::SetTotalEvents(Int_t bin,Int_t events)
{
   //sets the number of total events in the given global bin
   //
   //returns "true" if the number of total events has been updated
   //otherwise "false" ist returned
   //
   //Note: - requires: fPassedHistogram->GetBinContent(bin) <= events

   if(events >= fPassedHistogram->GetBinContent(bin)) {      
      fTotalHistogram->SetBinContent(bin,events);
      return true;
   }
   else {
      Error("SetTotalEvents(Int_t,Int_t)","passed number of events (%.1lf) in bin %i is bigger than given number of total events %i",fPassedHistogram->GetBinContent(bin),bin,events);
      return false;
   }
}

//______________________________________________________________________________
Bool_t TEfficiency::SetTotalHistogram(const TH1& rTotal,Option_t* opt)
{
   //sets the histogram containing all events
   //
   //The given histogram is cloned and stored internally as histogram containing
   //all events. The given histogram has to be consistent with the current
   //fPassedHistogram (see CheckConsistency(const TH1&,const TH1&)).
   //The method returns whether the fTotalHistogram has been replaced (true) or
   //not (false).
   //
   //Note: The list of associated functions fFunctions is cleared.
   //
   //Option: - "f": force the replacement without checking the consistency
   //               This can lead to inconsistent histograms and useless results
   //               or unexpected behaviour. But sometimes it might be the only
   //               way to change the histograms. If you use this option, you
   //               should ensure that the fPassedHistogram is replaced by a
   //               consistent one (with respect to rTotal) as well.

   TString option = opt;
   option.ToLower();

   Bool_t bReplace = option.Contains("f");

   if(!bReplace)
      bReplace = CheckConsistency(*fPassedHistogram,rTotal);

   if(bReplace) {
      delete fTotalHistogram;
      Bool_t bStatus = TH1::AddDirectoryStatus();
      TH1::AddDirectory(kFALSE);
      fTotalHistogram = (TH1*)(rTotal.Clone());
      fTotalHistogram->SetNormFactor(0);
      TH1::AddDirectory(bStatus);

      if(fFunctions)
	 fFunctions->Delete();

      return true;
   }
   else
      return false;
}

//______________________________________________________________________________
void TEfficiency::SetWeight(Double_t weight)
{
   //sets the global weight for this TEfficiency object
   //
   //Note: - weight has to be positive ( > 0)
   
   if(weight > 0)
      fWeight = weight;
   else
      Warning("SetWeight","invalid weight %.2lf",weight);
}

//______________________________________________________________________________
Double_t TEfficiency::Wilson(Int_t total,Int_t passed,Double_t level,Bool_t bUpper)
{
   //calculates the boundaries for the frequentist Wilson interval
   //
   //Input: - total : number of total events
   //       - passed: 0 <= number of passed events <= total
   //       - level : confidence level
   //       - bUpper: true  - upper boundary is returned
   //                 false - lower boundary is returned
   //
   //calculation:
   //Begin_Latex(separator='=',align='rl')
   // #alpha = 1 - #frac{level}{2}
   // #kappa = #Phi^{-1}(1 - #alpha,1) ... normal quantile function
   // mode = #frac{passed + #frac{#kappa^{2}}{2}}{total + #kappa^{2}}
   // #Delta = #frac{#kappa}{total + #kappa^{2}} * #sqrt{passed (1 - #frac{passed}{total}) + #frac{#kappa^{2}}{4}}
   // return = max(0,mode - #Delta) or min(1,mode + #Delta)
   //End_Latex
   
   Double_t alpha = (1.0 - level)/2;
   Double_t average = ((Double_t)passed) / total;
   Double_t kappa = ROOT::Math::normal_quantile(1 - alpha,1);

   Double_t mode = (passed + 0.5 * kappa * kappa) / (total + kappa * kappa);
   Double_t delta = kappa / (total + kappa*kappa) * std::sqrt(total * average
							      * (1 - average) + kappa * kappa / 4);
   if(bUpper)
      return ((mode + delta) > 1) ? 1.0 : (mode + delta);
   else
      return ((mode - delta) < 0) ? 0.0 : (mode - delta);
}

//______________________________________________________________________________
const TEfficiency operator+(const TEfficiency& lhs,const TEfficiency& rhs)
{
   // addition operator
   //
   // adds the corresponding histograms:
   // lhs.GetTotalHistogram() + rhs.GetTotalHistogram()
   // lhs.GetPassedHistogram() + rhs.GetPassedHistogram()
   //
   // the statistic option and the confidence level are taken from lhs
   
   TEfficiency tmp(lhs);
   tmp += rhs;
   return tmp;
}

#endif