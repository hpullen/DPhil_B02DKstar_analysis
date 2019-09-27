// -- CLASS DESCRIPTION [PDF] --

#include "RooFit.h"
#include <iostream>
#include <math.h>
#include "TMath.h"

#include "RooHILLdini_misID.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"

RooHILLdini_misID::RooHILLdini_misID(const char *name, const char *title, RooAbsReal& _m, RooAbsReal& _a, RooAbsReal& _b, RooAbsReal& _csi, RooAbsReal& _m1, RooAbsReal& _s1, RooAbsReal& _m2, RooAbsReal& _s2, RooAbsReal& _m3, RooAbsReal& _s3, RooAbsReal& _m4, RooAbsReal& _s4 ,RooAbsReal& _f1, RooAbsReal& _f2, RooAbsReal& _f3 ) :

  RooAbsPdf(name, title),
  m("m", "Dependent", this, _m),
  a("a", "a", this, _a),
  b("b", "b", this, _b),
  csi("csi", "csi", this, _csi),
  m1("m1", "m1", this, _m1),
  s1("s1", "s1", this, _s1),
  m2("m2", "m2", this, _m2),
  s2("s2", "s2", this, _s2),
  m3("m3", "m3", this, _m3),
  s3("s3", "s3", this, _s3),
  m4("m4", "m4", this, _m4),
  s4("s4", "s4", this, _s4),
  f1("f1", "f1", this, _f1),
  f2("f2", "f2", this, _f2),
  f3("f3", "f3", this, _f3)
{
}


RooHILLdini_misID::RooHILLdini_misID(const RooHILLdini_misID& other, const char* name) :
RooAbsPdf(other, name), m("m", this, other.m), a("a", this, other.a), b("b", this, other.b), csi("csi", this, other.csi), m1("m1", this, other.m1), s1("s1", this, other.s1), m2("m2", this, other.m2), s2("s2", this, other.s2), m3("m3", this, other.m3), s3("s3", this, other.s3), m4("m4", this, other.m4), s4("s4", this, other.s4), f1("f1", this, other.f1), f2("f2", this, other.f2), f3("f3", this, other.f3)
{
}





Double_t RooHILLdini_misID::evaluate() const 
{

  ///////////////////////////////
  //double t = m;
  double a_new  = a;
  double b_new  = b;
  double B_NEW  = (a_new+b_new)/2;
  ///////////////////////////////

  ///////////////////////////////////////////////////////////////////////
  //mult           = ((1-csi)/(b_new-a_new)*m + (b_new*csi - a_new)/(b_new-a_new));
  //double CURVEG1 = fabs((1-csi)*secondG1    + (b_new*csi - a_new)*firstG1);
  ///////////////////////////////////////////////////////////////////////

  double dm1 = m-m1;
  double dm2 = m-m2;
  double dm3 = m-m3;
  double dm4 = m-m4;

  double sqrt_2pi = sqrt(2*TMath::Pi());
  double one_over_sq2pi = 1/sqrt_2pi;
  double one_over_sq2 = 1/sqrt(2);

  auto curve = [&](double m_i, double sigma_val){
      // Calculate the expression for a convolution with kernel width sigma_val
      
      double dm = m - m_i;

      double one_over_sig = one_over_sq2 / sigma_val;
      double dma = one_over_sig * (dm - a_new);
      double dmb = one_over_sig * (b_new - dm);
      double sigma_sq = sqrt(2)*sigma_val*sigma_val;

      double ea = TMath::Erf(dma);
      double eb = TMath::Erf(dmb);
      double erf_term = ea + eb;

      double x1  = one_over_sq2pi * sigma_sq * exp(-dma*dma) * dmb;
      double x2  = one_over_sq2pi * sigma_sq * exp(-dmb*dmb) * dma;
      double x12 = one_over_sq2   * sigma_sq * (dma*dmb - 0.5);

      return (x1 + x2 + x12*erf_term);

  };  
  /////////////////// 
  double firstG1  = curve(m1, s1);
  double  CURVEG1 =    fabs(   (1-csi)/(b_new-a_new)*(dm1)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG1);
  ///////////////////  

  ///////////////////                                                                                                                                                                                                             
  double firstG2  = curve(m2, s2);
  double  CURVEG2 =    fabs(   (1-csi)/(b_new-a_new)*(dm2)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG2);
  ///////////////////                                                                                                    


  ///////////////////                                                                                           
  double firstG3  = curve(m3, s3);
  double  CURVEG3 =    fabs(   (1-csi)/(b_new-a_new)*(dm3)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG3);
  ///////////////////                                                                                                    

  ///////////////////
  double firstG4  = curve(m4, s4);
  double  CURVEG4 =    fabs(   (1-csi)/(b_new-a_new)*(dm4)  + (b_new*csi - a_new)/(b_new-a_new)  )*fabs(firstG4);
  ///////////////////                                                                                                    


  return fabs(f1*CURVEG1)+ fabs(f2*CURVEG2)+  fabs(f3*CURVEG3) + fabs((1-f1-f2-f3)*CURVEG4);
}

 Int_t RooHILLdini_misID::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const  
 { 
   // LIST HERE OVER WHICH VARIABLES ANALYTICAL INTEGRATION IS SUPPORTED, 
   // ASSIGN A NUMERIC CODE FOR EACH SUPPORTED (SET OF) PARAMETERS 
   // THE EXAMPLE BELOW ASSIGNS CODE 1 TO INTEGRATION OVER VARIABLE m
   // YOU CAN ALSO IMPLEMENT MORE THAN ONE ANALYTICAL INTEGRAL BY REPEATING THE matchArgs 
   // EXPRESSION MULTIPLE TIMES

   if (matchArgs(allVars,analVars,m)) return 1 ; 
   return 0 ; 
 } 



 Double_t RooHILLdini_misID::analyticalIntegral(Int_t code, const char* rangeName) const  
 { 
   // RETURN ANALYTICAL INTEGRAL DEFINED BY RETURN CODE ASSIGNED BY getAnalyticalIntegral
   // THE MEMBER FUNCTION x.min(rangeName) AND x.max(rangeName) WILL RETURN THE INTEGRATION
   // BOUNDARIES FOR EACH OBSERVABLE x

   if (!(code==1)) { return 0 ; } 


   // we are integrating over m!
   double m_lim1 = m.min(rangeName);
   double m_lim2 = m.max(rangeName);


  ///////////////////////////////

  double one_over_sq2 = 1/sqrt(2);
  double one_over_sq2pi = one_over_sq2/sqrt(TMath::Pi());

  double _sqrt_pi = sqrt(TMath::Pi());

  double a_new  = a;
  double b_new  = b;
  // double dm = m-shift;

  
  ///////////////////////////////

  double one_over_sig, sqrt2_sig, sigma_sq, sq2_sigma_sq;
  double dm_lim1, dm_lim2, dma1, dma2, dmb1, dmb2, shift;
  double ea1, eb1, ea2, eb2;
  double exp_dma1_sq, exp_dma2_sq, exp_dmb1_sq, exp_dmb2_sq;

  auto set_vals = [&](double m_i, double sigma_val){

    dm_lim1 = m_lim1-m_i;
    dm_lim2 = m_lim2-m_i;   
    shift = m_i; 
    
    one_over_sig = one_over_sq2 / sigma_val;
    sqrt2_sig = 1/one_over_sig;
    dma1 = one_over_sig * (dm_lim1 - a_new);
    dmb1 = one_over_sig * (b_new - dm_lim1);
    dma2 = one_over_sig * (dm_lim2 - a_new);
    dmb2 = one_over_sig * (b_new - dm_lim2);

    sigma_sq = sigma_val*sigma_val;
    sq2_sigma_sq = sqrt(2)*sigma_val*sigma_val;

    // double ea = TMath::Erf(dma);
    // double eb = TMath::Erf(dmb);
    ea1 = TMath::Erf(dma1);
    eb1 = TMath::Erf(dmb1);
    ea2 = TMath::Erf(dma2);
    eb2 = TMath::Erf(dmb2);

    exp_dma1_sq = exp(-dma1*dma1);
    exp_dma2_sq = exp(-dma2*dma2);
    exp_dmb1_sq = exp(-dmb1*dmb1);
    exp_dmb2_sq = exp(-dmb2*dmb2);
  };

  // Integrate the curve terms that are NOT multiplied with m is csi part
  // Set values using set_vals(sigma<2>)
  auto curve_int = [&](){
    
  
    double x1_int  = one_over_sq2pi * sq2_sigma_sq * 0.5*(
      sqrt2_sig*(-exp_dma1_sq+exp_dma2_sq)
      +_sqrt_pi*(a-b)*(ea1-ea2));  


    double x2_int  = one_over_sq2pi * sq2_sigma_sq * 0.5*(
      sqrt2_sig*(exp_dmb1_sq-exp_dmb2_sq)
      +_sqrt_pi*(a-b)*(eb2-eb1));


    double erf_term_simple_a_int = -0.5 * sigma_sq * (
      sqrt2_sig/sqrt(TMath::Pi())*(exp_dma2_sq-exp_dma1_sq)
      +(a-m_lim1+shift)*ea1-(a-m_lim2+shift)*ea2
      );

    double erf_term_simple_b_int = -0.5 * sigma_sq * (
      sqrt2_sig/sqrt(TMath::Pi())*(-exp_dmb2_sq+exp_dmb1_sq)
      +(b-m_lim1+shift)*eb1-(b-m_lim2+shift)*eb2
      );


    double erf_term_dm_a_int = sigma_sq  *
    ( 1/(12*_sqrt_pi*sigma_sq)*(
      sqrt2_sig*(
        exp_dma2_sq*( (a-3*b+2*m_lim2-2*shift)*(a-m_lim2+shift)-4*sigma_sq) +
        exp_dma1_sq*(-(a-3*b+2*m_lim1-2*shift)*(a-m_lim1+shift)+4*sigma_sq))
      -_sqrt_pi*(
          (-(a - 3*b + 2*m_lim1 - 2*shift)*(a - m_lim1 + shift)*(a - m_lim1 + shift) + 3*(a - b)*sigma_sq)*ea1 +
          ( (a - 3*b + 2*m_lim2 - 2*shift)*(a - m_lim2 + shift)*(a - m_lim2 + shift) - 3*(a - b)*sigma_sq)*ea2
        )
      )
    );


    double erf_term_dm_b_int = sigma_sq  *
    ( 1/(12*_sqrt_pi*sigma_sq)*(
      sqrt2_sig*(
        exp_dmb1_sq*( (b-3*a+2*m_lim1-2*shift)*(b-m_lim1+shift)-4*sigma_sq) +
        exp_dmb2_sq*(-(b-3*a+2*m_lim2-2*shift)*(b-m_lim2+shift)+4*sigma_sq))
      +_sqrt_pi*(
          ( (b - 3*a + 2*m_lim1 - 2*shift)*(b - m_lim1 + shift)*(b - m_lim1 + shift) + 3*(a - b)*sigma_sq)*eb1 +
          (-(b - 3*a + 2*m_lim2 - 2*shift)*(b - m_lim2 + shift)*(b - m_lim2 + shift) - 3*(a - b)*sigma_sq)*eb2
        )
      )
    );


    double erf_term_a_int = erf_term_simple_a_int + erf_term_dm_a_int;
    double erf_term_b_int = erf_term_simple_b_int + erf_term_dm_b_int;
    return (x1_int + x2_int + erf_term_a_int + erf_term_b_int);
  };

  // Integrate the curve terms that ARE multiplied with m is csi part
  // Set values using set_vals(sigma<2>)
  auto curve_int_m = [&](){

    double x1_int_m = one_over_sq2pi * sq2_sigma_sq * 0.5*(
      sqrt2_sig*(exp_dma2_sq*(a-b+m_lim2)-exp(-dma1*dma1)*(a-b+m_lim1))
      +_sqrt_pi*((a-b)*(a+shift)+sigma_sq)*(ea1-ea2));


    double x2_int_m = one_over_sq2pi * sq2_sigma_sq * 0.5*(
      sqrt2_sig*(-exp(-dmb2*dmb2)*(b-a+m_lim2)+exp(-dmb1*dmb1)*(b-a+m_lim1))
      +_sqrt_pi*((b-a)*(b+shift)+sigma_sq)*(eb1-eb2));


    double erf_term_simple_a_int_m =  -0.5 * sigma_sq * 0.5/_sqrt_pi*(
      sqrt2_sig*(-exp_dma1_sq*(a+m_lim1+shift)+exp_dma2_sq*(a+m_lim2+shift))
      +_sqrt_pi*(
        (-m_lim1*m_lim1+(a+shift)*(a+shift)+sigma_sq)*ea1 -
        (-m_lim2*m_lim2+(a+shift)*(a+shift)+sigma_sq)*ea2)
      );


    double erf_term_simple_b_int_m =  -0.5 * sigma_sq * 0.5/_sqrt_pi*(
      sqrt2_sig*( exp_dmb1_sq*(b+m_lim1+shift)-exp_dmb2_sq*(b+m_lim2+shift))
      +_sqrt_pi*(
        (-m_lim1*m_lim1+(b+shift)*(b+shift)+sigma_sq)*eb1 -
        (-m_lim2*m_lim2+(b+shift)*(b+shift)+sigma_sq)*eb2)
      );


    double sa1 = a*a + 2*a*(-b + m_lim1) - 2*b*(2*m_lim1 + shift) + (m_lim1 - shift) * (3*m_lim1 + shift);
    double sa2 = a*a + 2*a*(-b + m_lim2) - 2*b*(2*m_lim2 + shift) + (m_lim2 - shift) * (3*m_lim2 + shift);
    double erf_term_dm_a_int_m =  sigma_sq  *1/(12*_sqrt_pi)*(
       exp_dma1_sq*(sa1*dma1-(-7*a+8*b-9*m_lim1+shift)*sqrt2_sig*0.5)
      -exp_dma2_sq*(sa2*dma2-(-7*a+8*b-9*m_lim2+shift)*sqrt2_sig*0.5)
      +_sqrt_pi*(
        -(-sa1*dma1*dma1+3*(a-b)*(a+shift) + 0.5*9*sigma_sq)*ea1
        -( sa2*dma2*dma2-3*(a-b)*(a+shift) - 0.5*9*sigma_sq)*ea2
        )
      );


    double sb1 = b*b + 2*b*(-a + m_lim1) - 2*a*(2*m_lim1 + shift) + (m_lim1 - shift) * (3*m_lim1 + shift);
    double sb2 = b*b + 2*b*(-a + m_lim2) - 2*a*(2*m_lim2 + shift) + (m_lim2 - shift) * (3*m_lim2 + shift);
    double erf_term_dm_b_int_m =  sigma_sq  *1/(12*_sqrt_pi)*(
       exp_dmb1_sq*(sb1*dmb1+(-7*b+8*a-9*m_lim1+shift)*sqrt2_sig*0.5)
      -exp_dmb2_sq*(sb2*dmb2+(-7*b+8*a-9*m_lim2+shift)*sqrt2_sig*0.5)
      +_sqrt_pi*(
        -(-sb1*dmb1*dmb1-3*(a-b)*(b+shift) + 0.5*9*sigma_sq)*eb1
        -( sb2*dmb2*dmb2+3*(a-b)*(b+shift) - 0.5*9*sigma_sq)*eb2
        )
      );


    double erf_term_a_int_m = erf_term_simple_a_int_m + erf_term_dm_a_int_m;
    double erf_term_b_int_m = erf_term_simple_b_int_m + erf_term_dm_b_int_m;

    return (x1_int_m + x2_int_m + erf_term_a_int_m + erf_term_b_int_m);

  };



 
  set_vals(m1, s1);
  double firstG1  = curve_int();
  double firstG1_m = curve_int_m();
  double  CURVEG1 = ((1-csi)/(b_new-a_new))*(firstG1_m-m1*firstG1)
  + ((b_new*csi - a_new)/(b_new-a_new) )*(firstG1); 

  set_vals(m2, s2);
  double firstG2  = curve_int();
  double firstG2_m = curve_int_m();
  double  CURVEG2 = ((1-csi)/(b_new-a_new))*(firstG2_m-m2*firstG2)
  + ((b_new*csi - a_new)/(b_new-a_new) )*(firstG2); 

  set_vals(m3, s3);
  double firstG3  = curve_int();
  double firstG3_m = curve_int_m();
  double  CURVEG3 = ((1-csi)/(b_new-a_new))*(firstG3_m-m3*firstG3)
  + ((b_new*csi - a_new)/(b_new-a_new) )*(firstG3); 

  set_vals(m4, s4);
  double firstG4  = curve_int();
  double firstG4_m = curve_int_m();
  double  CURVEG4 = ((1-csi)/(b_new-a_new))*(firstG4_m-m4*firstG4)
  + ((b_new*csi - a_new)/(b_new-a_new) )*(firstG4); 


  return fabs(f1*CURVEG1)+ fabs(f2*CURVEG2)+  fabs(f3*CURVEG3) + fabs((1-f1-f2-f3)*CURVEG4);

  }