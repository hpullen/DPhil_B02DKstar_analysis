
#include "RooFit.h"
#include <iostream>
#include <math.h>
#include "TMath.h"

#include "RooHORNSdini.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"



  RooHORNSdini::RooHORNSdini(const char *name, const char *title,
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


RooHORNSdini::RooHORNSdini(const RooHORNSdini& other, const char* name) :
  RooAbsPdf(other, name), m("m", this, other.m), a("a", this, other.a), b("b", this, other.b), csi("csi", this, other.csi), shift("shift", this, other.shift), sigma("sigma", this, other.sigma),  ratio_sigma("ratio_sigma", this, other.ratio_sigma), fraction_sigma("fraction_sigma", this, other.fraction_sigma)
{
}


Double_t RooHORNSdini::evaluate() const 
{

  ///////////////////////////////
  double a_new  = a;
  double b_new  = b;
  double B_NEW  = (a_new+b_new)/2;
  double sigma2 = sigma * ratio_sigma;
  ///////////////////////////////

  ///////////////////////////////////////////////////////////////////////
  //mult = ((1-csi)/(b_new-a_new)*m + (b_new*csi - a_new)/(b_new-a_new));
  ///////////////////////////////////////////////////////////////////////



  ///////////////////////////////////////////////////////////////////////
  // double firstG1 = ((2*(a_new-2*B_NEW+(m-shift))*sigma)/exp((a_new-(m-shift))*(a_new-(m-shift))/(2*sigma*sigma)) - (2*(b_new-2*B_NEW+(m-shift))*sigma)/exp((b_new-(m-shift))*(b_new-(m-shift))/(2*sigma*sigma))+ sqrt(2*TMath::Pi())*((B_NEW-(m-shift))*(B_NEW-(m-shift)) + sigma*sigma)*TMath::Erf((-a_new+(m-shift))/(sqrt(2)*sigma))  - sqrt(2*TMath::Pi())*((B_NEW-(m-shift))*(B_NEW-(m-shift)) + sigma*sigma) * TMath::Erf((-b_new+(m-shift))/(sqrt(2)*sigma)))/(2*sqrt(2*TMath::Pi()));
  // double secondG1 = (((2*sigma*(a_new*a_new + B_NEW*B_NEW + a_new*(m-shift) + (m-shift)*(m-shift) - 2*B_NEW*(a_new+(m-shift)) + 2*(sigma*sigma)))/exp((a_new-(m-shift))*(a_new-(m-shift))/(2*(sigma*sigma))) - (2*sigma*(b_new*b_new + B_NEW*B_NEW + b_new*(m-shift) + (m-shift)*(m-shift) - 2*B_NEW*(b_new + (m-shift)) + 2*(sigma*sigma)))/exp((b_new - (m-shift))*(b_new - (m-shift))/(2*(sigma*sigma))) - sqrt(2*TMath::Pi())*(-((B_NEW - (m-shift))*(B_NEW - (m-shift)) *(m-shift)) + (2*B_NEW - 3*(m-shift))*(sigma*sigma))*TMath::Erf((-a_new + (m-shift))/(sqrt(2)*sigma)) + sqrt(2*TMath::Pi())* (-((B_NEW - (m-shift))*(B_NEW - (m-shift))*(m-shift)) + (2*B_NEW - 3*(m-shift))*(sigma*sigma)) *TMath::Erf((-b_new + (m-shift))/(sqrt(2)*sigma)))/(2 *sqrt(2*TMath::Pi())));
  // double CURVEG1 = fabs((1-csi)*secondG1 + (b_new*csi - a_new)*firstG1);
  ///////////////////////////////////////////////////////////////////////



  ///////////////////////////////////////////////////////////////////////
  // double firstG2 = ((2*(a_new-2*B_NEW+(m-shift))*sigma2)/exp((a_new-(m-shift))*(a_new-(m-shift))/(2*sigma2*sigma2)) - (2*(b_new-2*B_NEW+(m-shift))*sigma2)/exp((b_new-(m-shift))*(b_new-(m-shift))/(2*sigma2*sigma2))+ sqrt(2*TMath::Pi())*((B_NEW-(m-shift))*(B_NEW-(m-shift)) + sigma2*sigma2)*TMath::Erf((-a_new+(m-shift))/(sqrt(2)*sigma2))  - sqrt(2*TMath::Pi())*((B_NEW-(m-shift))*(B_NEW-(m-shift)) + sigma2*sigma2) * TMath::Erf((-b_new+(m-shift))/(sqrt(2)*sigma2)))/(2*sqrt(2*TMath::Pi()));
  // double secondG2 = (((2*sigma2*(a_new*a_new + B_NEW*B_NEW + a_new*(m-shift) + (m-shift)*(m-shift) - 2*B_NEW*(a_new+(m-shift)) + 2*(sigma2*sigma2)))/exp((a_new-(m-shift))*(a_new-(m-shift))/(2*(sigma2*sigma2))) - (2*sigma2*(b_new*b_new + B_NEW*B_NEW + b_new*(m-shift) + (m-shift)*(m-shift) - 2*B_NEW*(b_new + (m-shift)) + 2*(sigma2*sigma2)))/exp((b_new - (m-shift))*(b_new - (m-shift))/(2*(sigma2*sigma2))) - sqrt(2*TMath::Pi())*(-((B_NEW - (m-shift))*(B_NEW - (m-shift)) *(m-shift)) + (2*B_NEW - 3*(m-shift))*(sigma2*sigma2))*TMath::Erf((-a_new + (m-shift))/(sqrt(2)*sigma2)) + sqrt(2*TMath::Pi())* (-((B_NEW - (m-shift))*(B_NEW - (m-shift))*(m-shift)) + (2*B_NEW - 3*(m-shift))*(sigma2*sigma2)) *TMath::Erf((-b_new + (m-shift))/(sqrt(2)*sigma2)))/(2 *sqrt(2*TMath::Pi())));
  // double CURVEG2 = fabs((1-csi)*secondG2 + (b_new*csi - a_new)*firstG2);
  ///////////////////////////////////////////////////////////////////////



  // Calculate a lot of const we need
  double dm = m-shift;
  double sigma_sq = sigma*sigma;
  double sigma2_sq = sigma2*sigma2;
  double sqrt_2pi = sqrt(2*TMath::Pi());

  ///////////////////////////////////////////////////////////////////////
  double ea = TMath::Erf((-a_new+(dm))/(sqrt(2)*sigma));
  double eb = TMath::Erf((-b_new+(dm))/(sqrt(2)*sigma));
  double erf_term = ea - eb;

  double x1 = (2*(a_new-2*B_NEW+(dm))*sigma) * exp(-(a_new-(dm))*(a_new-(dm))/(2*sigma_sq));
  double x2 = (2*(b_new-2*B_NEW+(dm))*sigma) * exp(-(b_new-(dm))*(b_new-(dm))/(2*sigma_sq));
  double x12 = sqrt_2pi*((B_NEW-(dm))*(B_NEW-(dm)) + sigma_sq);
  double firstG1 = 1/(2*sqrt_2pi)*( x1 - x2 + x12*erf_term);
  

  double y1 = (2*sigma*(a_new*a_new + B_NEW*B_NEW + a_new*(dm) + (dm)*(dm) - 2*B_NEW*(a_new+(dm)) + 2*(sigma_sq)))/exp((a_new-(dm))*(a_new-(dm))/(2*(sigma_sq)));
  double y2 = (2*sigma*(b_new*b_new + B_NEW*B_NEW + b_new*(dm) + (dm)*(dm) - 2*B_NEW*(b_new+(dm)) + 2*(sigma_sq)))/exp((b_new-(dm))*(b_new-(dm))/(2*(sigma_sq)));
  double y12 = sqrt_2pi*(-((B_NEW - (dm))*(B_NEW - (dm)) *(dm)) + (2*B_NEW - 3*(dm))*(sigma_sq));
  double secondG1 = 1/(2 *sqrt_2pi)*( y1 -  y2 - y12*erf_term);
  double CURVEG1 = fabs((1-csi)*secondG1 + (b_new*csi - a_new)*firstG1);
  ///////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////

  ea = TMath::Erf((-a_new+(dm))/(sqrt(2)*sigma2));
  eb = TMath::Erf((-b_new+(dm))/(sqrt(2)*sigma2));
  erf_term = ea - eb;

  x1 = (2*(a_new-2*B_NEW+(dm))*sigma2)/exp((a_new-(dm))*(a_new-(dm))/(2*sigma2_sq));
  x2 = (2*(b_new-2*B_NEW+(dm))*sigma2)/exp((b_new-(dm))*(b_new-(dm))/(2*sigma2_sq));
  x12 = sqrt_2pi*((B_NEW-(dm))*(B_NEW-(dm)) + sigma2_sq);
  double firstG2 = 1/(2*sqrt_2pi)*(x1 - x2 + x12*erf_term);
  
  y1 = (2*sigma2*(a_new*a_new + B_NEW*B_NEW + a_new*(dm) + (dm)*(dm) - 2*B_NEW*(a_new+(dm)) + 2*(sigma2_sq)))/exp((a_new-(dm))*(a_new-(dm))/(2*(sigma2_sq)));
  y2 = (2*sigma2*(b_new*b_new + B_NEW*B_NEW + b_new*(dm) + (dm)*(dm) - 2*B_NEW*(b_new+(dm)) + 2*(sigma2_sq)))/exp((b_new-(dm))*(b_new-(dm))/(2*(sigma2_sq)));
  y12 = sqrt_2pi*(-((B_NEW - (dm))*(B_NEW - (dm)) *(dm)) + (2*B_NEW - 3*(dm))*(sigma2_sq));
  double secondG2 = 1/(2 *sqrt_2pi)*( y1 -  y2 - y12*erf_term);
  double CURVEG2 = fabs((1-csi)*secondG2 + (b_new*csi - a_new)*firstG2);
  ////////

  return fraction_sigma*CURVEG1 + (1-fraction_sigma)*CURVEG2;


}
