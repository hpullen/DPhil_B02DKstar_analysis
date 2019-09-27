// -- CLASS DESCRIPTION [PDF] --

#include "RooFit.h"
#include <iostream>
#include <math.h>
#include "TMath.h"

#include "RooHORNSdini_misID.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"


RooHORNSdini_misID::RooHORNSdini_misID(const char *name, const char *title, RooAbsReal& _m, RooAbsReal& _a, RooAbsReal& _b, RooAbsReal& _csi, RooAbsReal& _m1, RooAbsReal& _s1, RooAbsReal& _m2, RooAbsReal& _s2, RooAbsReal& _m3, RooAbsReal& _s3, RooAbsReal& _m4, RooAbsReal& _s4 ,RooAbsReal& _f1, RooAbsReal& _f2, RooAbsReal& _f3 ) :

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


RooHORNSdini_misID::RooHORNSdini_misID(const RooHORNSdini_misID& other, const char* name) :
RooAbsPdf(other, name), m("m", this, other.m), a("a", this, other.a), b("b", this, other.b), csi("csi", this, other.csi), m1("m1", this, other.m1), s1("s1", this, other.s1), m2("m2", this, other.m2), s2("s2", this, other.s2), m3("m3", this, other.m3), s3("s3", this, other.s3), m4("m4", this, other.m4), s4("s4", this, other.s4), f1("f1", this, other.f1), f2("f2", this, other.f2), f3("f3", this, other.f3)
{
}





Double_t RooHORNSdini_misID::evaluate() const 
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

  double one_over_sqrt_pi = 1/sqrt(TMath::Pi());
  double one_over_sqrt_2 = 1/sqrt(2);

  double dm, dma, dmb, dmc;
  double one_over_sig, sigma_value, sqrt2_sig, sigma_sq;
  double ea, eb, erf_term;
  double exp_da_sq, exp_db_sq;

  auto set_vals = [&](double m_i, double sigma_val){
      sigma_value = sigma_val;
      one_over_sig = one_over_sqrt_2 / sigma_val;
      sqrt2_sig = 1/one_over_sig;

      dm = m - m_i;
      dma = one_over_sig * (dm - a_new);
      dmb = one_over_sig * (b_new - dm);
      dmc = one_over_sig * (B_NEW - dm);

      sigma_sq = sigma_val*sigma_val;

      ea = TMath::Erf(dma);
      eb = TMath::Erf(dmb);
      erf_term = ea + eb;

      exp_da_sq = exp(-dma*dma);
      exp_db_sq = exp(-dmb*dmb);
  };

  auto curve = [&](){
      // Calculate the expression for a convolution with kernel width sigma_val
      double x1  = -1*one_over_sqrt_pi *sigma_sq * exp_da_sq * dmb;
      double x2  = -1*one_over_sqrt_pi *sigma_sq * exp_db_sq * dma;
      double x12 = sigma_sq*(dmc*dmc + 0.5);
      return (x1 + x2 + x12*erf_term);
  };

  auto curve_pt2 = [&](){
    // Calculate the expression for a convolution with kernel width sigma_val
    double y1 = 0.25*one_over_sqrt_pi*sqrt2_sig*(0.5*(b_new-a_new)*(b_new-a_new) + 2*dm*dm - 2*b_new*dm + 4*sigma_sq)*exp_da_sq;
    double y2 = 0.25*one_over_sqrt_pi*sqrt2_sig*(0.5*(b_new-a_new)*(b_new-a_new) + 2*dm*dm - 2*a_new*dm + 4*sigma_sq)*exp_db_sq;
    double y12 = sigma_sq*(dmc*dmc *dm - 0.5*(a+b-3*dm));
    return ( y1 -  y2 + y12*erf_term);
  };


  
  set_vals(m1, s1);
  double firstG1 = curve();
  double secondG1 = curve_pt2();
  double CURVEG1 = fabs((1-csi)*secondG1 + (b_new*csi - a_new)*firstG1);

  set_vals(m2, s2);
  double firstG2 = curve();
  double secondG2 = curve_pt2();
  double CURVEG2 = fabs((1-csi)*secondG2 + (b_new*csi - a_new)*firstG2);

  set_vals(m3, s3);
  double firstG3 = curve();
  double secondG3 = curve_pt2();
  double CURVEG3 = fabs((1-csi)*secondG3 + (b_new*csi - a_new)*firstG3);

  set_vals(m4, s4);
  double firstG4 = curve();
  double secondG4 = curve_pt2();
  double CURVEG4 = fabs((1-csi)*secondG4 + (b_new*csi - a_new)*firstG4);

  ///////////////////////////////////////////////////////////////////////  

  return fabs(f1*CURVEG1)+ fabs(f2*CURVEG2)+  fabs(f3*CURVEG3) + fabs((1-f1-f2-f3)*CURVEG4);

}

Int_t RooHORNSdini_misID::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const  
 { 
   // LIST HERE OVER WHICH VARIABLES ANALYTICAL INTEGRATION IS SUPPORTED, 
   // ASSIGN A NUMERIC CODE FOR EACH SUPPORTED (SET OF) PARAMETERS 
   // THE EXAMPLE BELOW ASSIGNS CODE 1 TO INTEGRATION OVER VARIABLE m
   // YOU CAN ALSO IMPLEMENT MORE THAN ONE ANALYTICAL INTEGRAL BY REPEATING THE matchArgs 
   // EXPRESSION MULTIPLE TIMES

   if (matchArgs(allVars,analVars,m)) return 1 ; 
   return 0 ; 
 } 

  Double_t RooHORNSdini_misID::analyticalIntegral(Int_t code, const char* rangeName) const  
 { 
   // RETURN ANALYTICAL INTEGRAL DEFINED BY RETURN CODE ASSIGNED BY getAnalyticalIntegral
   // THE MEMBER FUNCTION x.min(rangeName) AND x.max(rangeName) WILL RETURN THE INTEGRATION
   // BOUNDARIES FOR EACH OBSERVABLE x

   if (!(code==1)) { return 0 ; } 


   // we are integrating over m!
   double m1_lim = m.min(rangeName);
   double m2_lim = m.max(rangeName);

  double a_new  = a;
  double b_new  = b;
  double c_new = 0.5*(a_new+b_new);

  double dm1_lim, dm2_lim, shift;

  double _sqrt_pi = sqrt(TMath::Pi());
  double _one_over_sqrt_pi = 1/sqrt(TMath::Pi());
  double _one_over_sqrt_2 = 1/sqrt(2);

  double one_over_sig, sqrt2_sig, sigma_sq, sq2_sigma_sq;
  double dma1, dma2, dmb1, dmb2;
  double ea1, eb1, ea2, eb2;
  double exp_dma1_sq, exp_dma2_sq, exp_dmb1_sq, exp_dmb2_sq;

  auto set_vals = [&](double m_i, double sigma_val){

    dm1_lim = m1_lim-m_i;
    dm2_lim = m2_lim-m_i;    
    shift = m_i;


    one_over_sig = _one_over_sqrt_2 / sigma_val;
    sqrt2_sig = 1/one_over_sig;
    dma1 = one_over_sig * (dm1_lim - a_new);
    dmb1 = one_over_sig * (b_new - dm1_lim);
    dma2 = one_over_sig * (dm2_lim - a_new);
    dmb2 = one_over_sig * (b_new - dm2_lim);

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

  // std::cout << "\n  ---  m in " << m1_lim << "  -  " << m2_lim << "----\n";

  // Integrate the curve() calculated terms - that are NOT multiplied with m is csi part
  // Set values using set_vals(sigma<2>)
  auto curve_int = [&](){
    
  
    double x1_int  = -0.5*_one_over_sqrt_pi *sigma_sq *(
      sqrt2_sig*(-exp_dma1_sq+exp_dma2_sq)
      +_sqrt_pi*(a-b)*(ea1-ea2));  
    // std::cout << "x1: " << x1_int/(-_one_over_sqrt_pi *sigma_sq) << std::endl;

    double x2_int  = -0.5*_one_over_sqrt_pi *sigma_sq *(
      sqrt2_sig*(exp_dmb1_sq-exp_dmb2_sq)
      +_sqrt_pi*(a-b)*(eb2-eb1));
    // std::cout << "x2: " << x2_int/(-_one_over_sqrt_pi *sigma_sq) << std::endl;

    double erf_term_simple_a_int = 0.5 * sigma_sq * (
      sqrt2_sig/sqrt(TMath::Pi())*(exp_dma2_sq-exp_dma1_sq)
      +(a-m1_lim+shift)*ea1-(a-m2_lim+shift)*ea2
      );
    // std::cout << "erf term simple a: " << erf_term_simple_a_int/(0.5 * sigma_sq) << std::endl;

    double erf_term_simple_b_int = 0.5 * sigma_sq * (
      sqrt2_sig/sqrt(TMath::Pi())*(-exp_dmb2_sq+exp_dmb1_sq)
      +(b-m1_lim+shift)*eb1-(b-m2_lim+shift)*eb2
      );
    // std::cout << "erf term simple b: " << erf_term_simple_b_int/(0.5 * sigma_sq) << std::endl;

    auto erf_const = [&](double ab, double mn){
      return (ab*ab + 3*c_new*c_new + (mn - shift)*(mn - shift) + ab*(-3*c_new + mn - shift) 
        + 3*c_new*(-mn + shift));
    };

    double erf_term_dm_a_int = sigma_sq  *
    ( 1/(6*_sqrt_pi*sigma_sq)*(
      sqrt2_sig*(
        +exp_dma2_sq*(erf_const(a_new, m2_lim)+2*sigma_sq) 
        -exp_dma1_sq*(erf_const(a_new, m1_lim)+2*sigma_sq))
      +_sqrt_pi*(
          ( (a - m1_lim + shift)*erf_const(a_new, m1_lim) + 3*(a_new - c_new)*sigma_sq)*ea1 -
          ( (a - m2_lim + shift)*erf_const(a_new, m2_lim) + 3*(a_new - c_new)*sigma_sq)*ea2 
        )
      )
    );
    // std::cout << "erf term dmc^2 a: " << erf_term_dm_a_int/(sigma_sq) << std::endl;

    double erf_term_dm_b_int = sigma_sq  *
    ( 1/(6*_sqrt_pi*sigma_sq)*(
      sqrt2_sig*(
         exp_dmb1_sq*( erf_const(b_new, m1_lim)+2*sigma_sq)
        -exp_dmb2_sq*( erf_const(b_new, m2_lim)+2*sigma_sq)) 
      +_sqrt_pi*(
          ( (b - m1_lim + shift)*erf_const(b_new, m1_lim) + 3*(b_new - c_new)*sigma_sq)*eb1 -
          ( (b - m2_lim + shift)*erf_const(b_new, m2_lim) + 3*(b_new - c_new)*sigma_sq)*eb2
        )
      )
    );
    // std::cout << "erf term dmc^2 b: " << erf_term_dm_b_int/(sigma_sq) << std::endl;

    return x1_int + x2_int + erf_term_simple_a_int + erf_term_simple_b_int + erf_term_dm_a_int + erf_term_dm_b_int;

  };

  auto curve_pt2_int = [&](){

    double y1_int = 0.25*_one_over_sqrt_pi*sqrt2_sig*(0.5*_sqrt_pi*sqrt2_sig)*(
      (0.5*(b_new-a_new)*(b_new-a_new) + 4*sigma_sq)*(ea2-ea1) // constant term 
      - 2*b_new * ( _one_over_sqrt_pi*sqrt2_sig*(exp_dma1_sq-exp_dma2_sq) +  a_new*(ea2-ea1)) // prop to dm
      +2*(
        _one_over_sqrt_pi*sqrt2_sig*(exp_dma1_sq*(a+m1_lim-shift)-exp_dma2_sq*(a+m2_lim-shift)) // prop to dm^2
        +(a*a+sigma_sq)*(ea2-ea1)
        )
      );

    double y2_int = 0.25*_one_over_sqrt_pi*sqrt2_sig*(0.5*_sqrt_pi*sqrt2_sig)*(
      (0.5*(b_new-a_new)*(b_new-a_new) + 4*sigma_sq)*(eb1-eb2)
      - 2*a_new * ( _one_over_sqrt_pi*sqrt2_sig*(exp_dmb1_sq-exp_dmb2_sq) +  b_new*(eb1-eb2))
      +2*(
        _one_over_sqrt_pi*sqrt2_sig*(exp_dmb1_sq*(b+m1_lim-shift)-exp_dmb2_sq*(b+m2_lim-shift))
        +(b*b+sigma_sq)*(eb1-eb2)
        )
      );

    double erf_term_simple_a_int = -0.5*sigma_sq*(
        (a+b)*(sqrt2_sig*_one_over_sqrt_pi*(exp_dma2_sq-exp_dma1_sq)+(a-m1_lim+shift)*ea1-(a-m2_lim+shift)*ea2) // constant term
      );

    double erf_term_simple_a_int_m = -0.5*sigma_sq*(
        -3*0.5*(
          sqrt2_sig*_one_over_sqrt_pi * ( -(a+m1_lim-shift)*exp_dma1_sq + (a+m2_lim-shift)*exp_dma2_sq)
          +(a*a-(m1_lim-shift)*(m1_lim-shift)+sigma_sq)*ea1 - (a*a-(m2_lim-shift)*(m2_lim-shift)+sigma_sq)*ea2
          ) // dm term
      );

    auto exp_const = [&](double ab, double mn){
      return -3*ab*ab*ab - 3*(mn - shift)*(mn - shift)*(mn - shift) + 6*c_new*c_new* (-mn + shift) + ab*ab* (8*c_new - 3*mn + 3*shift) + 
      9*(-mn + shift) *sigma_sq + 8*c_new*((mn - shift)*(mn - shift) + 2*sigma_sq) - 
      ab*(6*c_new*c_new + 8*c_new*(-mn + shift) + 3*((mn - shift)*(mn - shift) + 5 * sigma_sq));
    };

    auto erf_const = [&](double ab, double mn){
      return (-3*pow(ab,4) + 8*pow(ab,3)* c_new - 8*c_new* pow(mn - shift,3) + 3*pow(mn - shift,4) + 
        24*ab*c_new*sigma_sq - 9*pow(sigma_sq, 2) + 
        6*c_new*c_new * (pow(mn - shift,2) - sigma_sq) - 6*ab*ab*(c_new*c_new + 3*sigma_sq));
    };

    double erf_term_simple_a_int_mm = sigma_sq*(
        1/(24*sigma_sq)*(
        sqrt2_sig*_one_over_sqrt_pi*(   exp_const(a_new, m1_lim)*exp_dma1_sq -exp_const(a_new, m2_lim)*exp_dma2_sq)
        +(-erf_const(a_new, m1_lim)*ea1+erf_const(a_new, m2_lim)*ea2) // dm term
        )
      );
    
    double erf_term_simple_b_int = -0.5*sigma_sq*(
        (a+b)*(sqrt2_sig*_one_over_sqrt_pi*(exp_dmb1_sq-exp_dmb2_sq)+(b-m1_lim+shift)*eb1-(b-m2_lim+shift)*eb2) // constant term
      );

    double erf_term_simple_b_int_m = -0.5*sigma_sq*(
        -3*0.5*(
          sqrt2_sig*_one_over_sqrt_pi * ( (b+m1_lim-shift)*exp_dmb1_sq - (b+m2_lim-shift)*exp_dmb2_sq)
          +(b*b-(m1_lim-shift)*(m1_lim-shift)+sigma_sq)*eb1 - (b*b-(m2_lim-shift)*(m2_lim-shift)+sigma_sq)*eb2
          ) // dm term
      );    

    double erf_term_simple_b_int_mm = sigma_sq*(
        1/(24*sigma_sq)*(
        sqrt2_sig*_one_over_sqrt_pi*(  -exp_const(b_new, m1_lim)*exp_dmb1_sq +exp_const(b_new, m2_lim)*exp_dmb2_sq)
        +(-erf_const(b_new, m1_lim)*eb1+erf_const(b_new, m2_lim)*eb2) // dm term
        )
      );

    return y1_int - y2_int 
        + erf_term_simple_a_int + erf_term_simple_a_int_m + erf_term_simple_a_int_mm
        + erf_term_simple_b_int + erf_term_simple_b_int_m + erf_term_simple_b_int_mm;


  };

  set_vals(m1, s1);
  double firstG1 = curve_int();
  double secondG1 = curve_pt2_int();
  double CURVEG1 = fabs((1-csi)*secondG1 + (b_new*csi - a_new)*firstG1);


  set_vals(m2, s2);
  double firstG2 = curve_int();
  double secondG2 = curve_pt2_int();
  double CURVEG2 = fabs((1-csi)*secondG2 + (b_new*csi - a_new)*firstG2);

  set_vals(m3, s3);
  double firstG3 = curve_int();
  double secondG3 = curve_pt2_int();
  double CURVEG3 = fabs((1-csi)*secondG3 + (b_new*csi - a_new)*firstG3);

  set_vals(m4, s4);
  double firstG4 = curve_int();
  double secondG4 = curve_pt2_int();
  double CURVEG4 = fabs((1-csi)*secondG4 + (b_new*csi - a_new)*firstG4);

  ////////

  // std::cout << "-\n";

  return fabs(f1*CURVEG1)+ fabs(f2*CURVEG2)+  fabs(f3*CURVEG3) + fabs((1-f1-f2-f3)*CURVEG4);
 }
