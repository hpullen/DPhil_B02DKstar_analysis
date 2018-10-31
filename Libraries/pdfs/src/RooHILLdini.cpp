
#include "RooFit.h"
#include <iostream>
#include <math.h>
#include "TMath.h"

#include "RooHILLdini.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"



  RooHILLdini::RooHILLdini(const char *name, const char *title,
				   RooAbsReal& _m, RooAbsReal& _a, RooAbsReal& _b, RooAbsReal& _csi, RooAbsReal& _shift, RooAbsReal& _sigma, RooAbsReal& _ratio_sigma, RooAbsReal& _fraction_sigma ) :


  RooAbsPdf(name, title),
  m("m", "Dependent", this, _m),
  a("a", "a", this, _a),
  b("b", "b", this, _b),
  csi("csi", "csi", this, _csi),
  shift("shift", "shift", this, _shift),
  sigma("sigma", "sigma", this, _sigma),
  ratio_sigma("ratio_sigma", "ratio_sigma", this, _ratio_sigma),
  fraction_sigma("fraction_sigma", "fraction_sigma", this, _fraction_sigma)
{
}


RooHILLdini::RooHILLdini(const RooHILLdini& other, const char* name) :
  RooAbsPdf(other, name), m("m", this, other.m), a("a", this, other.a), b("b", this, other.b), csi("csi", this, other.csi), shift("shift", this, other.shift), sigma("sigma", this, other.sigma),  ratio_sigma("ratio_sigma", this, other.ratio_sigma), fraction_sigma("fraction_sigma", this, other.fraction_sigma)
{
}





Double_t RooHILLdini::evaluate() const 
{

  ///////////////////////////////
  double a_new  = a;
  double b_new  = b;
  double sigma2 = sigma * ratio_sigma;
  ///////////////////////////////


  double dm = m-shift;
  double sigma_sq = sigma*sigma;
  double sigma2_sq = sigma2*sigma2;
  double sqrt_2pi = sqrt(2*TMath::Pi());

  ///////////////////

  double ea = TMath::Erf((-a_new+(dm))/(sqrt(2)*sigma));
  double eb = TMath::Erf((-b_new+(dm))/(sqrt(2)*sigma));
  double erf_term = ea - eb;

  double x1 = 2*exp(-((a_new-(dm))*(a_new-(dm))/(2*(sigma_sq))))*sigma*( b_new-(dm));
  double x2 = 2*exp(-((b_new-(dm))*(b_new-(dm))/(2*(sigma_sq))))*sigma*(-a_new+(dm));
  double x12 = sqrt_2pi*(a_new*b_new+(sigma_sq)-(a_new+b_new)*(dm)+((dm)*(dm)));

  double firstG1  = 1/(2*sqrt_2pi)*(x1+x2-x12*erf_term);
  double  CURVEG1 =    fabs(   (1-csi)/(b_new-a_new)*(m)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG1); 
  ///////////////////



  ///////////////////

  ea = TMath::Erf((-a_new+(dm))/(sqrt(2)*sigma2));
  eb = TMath::Erf((-b_new+(dm))/(sqrt(2)*sigma2));
  erf_term = ea - eb;

  x1 = 2*exp(-((a_new-(dm))*(a_new-(dm))/(2*(sigma2_sq))))*sigma2*( b_new-(dm));
  x2 = 2*exp(-((b_new-(dm))*(b_new-(dm))/(2*(sigma2_sq))))*sigma2*(-a_new+(dm));
  x12 = sqrt_2pi*(a_new*b_new+(sigma2_sq)-(a_new+b_new)*(dm)+((dm)*(dm)));

  double firstG2  = 1/(2*sqrt_2pi)*(x1+x2-x12*erf_term);
  double  CURVEG2 =    fabs(   (1-csi)/(b_new-a_new)*(m)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG2);
  ///////////////////



  // ///////////////////
  // double firstG1  = (2*exp(-((a_new-(m-shift))*(a_new-(m-shift))/(2*(sigma*sigma))))*sigma*(b_new-(m-shift))+2*exp(-((b_new-(m-shift))*(b_new-(m-shift))/(2*(sigma*sigma))))*sigma*(-a_new+(m-shift))-sqrt(2*TMath::Pi())*(a_new*b_new+(sigma*sigma)-(a_new+b_new)*(m-shift)+((m-shift)*(m-shift)))*TMath::Erf((-a_new+(m-shift))/(sqrt(2)*sigma))+sqrt(2*TMath::Pi())*(a_new*b_new+(sigma*sigma)-(a_new+b_new)*(m-shift)+((m-shift)*(m-shift)))*TMath::Erf((-b_new+(m-shift))/(sqrt(2)*sigma)))/(2*sqrt(2*TMath::Pi()));
  // //double  CURVEG1 =    fabs(   (1-csi)/(b_new-a_new)*(m-shift)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG1);
  // double  CURVEG1 =    fabs(   (1-csi)/(b_new-a_new)*(m)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG1); 
  // ///////////////////



  // ///////////////////
  // double firstG2  = (2*exp(-((a_new-(m-shift))*(a_new-(m-shift))/(2*(sigma2*sigma2))))*sigma2*(b_new-(m-shift))+2*exp(-((b_new-(m-shift))*(b_new-(m-shift))/(2*(sigma2*sigma2))))*sigma2*(-a_new+(m-shift))-sqrt(2*TMath::Pi())*(a_new*b_new+(sigma2*sigma2)-(a_new+b_new)*(m-shift)+((m-shift)*(m-shift)))*TMath::Erf((-a_new+(m-shift))/(sqrt(2)*sigma2))+sqrt(2*TMath::Pi())*(a_new*b_new+(sigma2*sigma2)-(a_new+b_new)*(m-shift)+((m-shift)*(m-shift)))*TMath::Erf((-b_new+(m-shift))/(sqrt(2)*sigma2)))/(2*sqrt(2*TMath::Pi()));
  // //double  CURVEG2 =    fabs(   (1-csi)/(b_new-a_new)*(m-shift)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG2);
  // double  CURVEG2 =    fabs(   (1-csi)/(b_new-a_new)*(m)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG2);
  // ///////////////////


  return fabs(fraction_sigma*CURVEG1 + (1-fraction_sigma)*CURVEG2);

  

}
