#include <map>
#include "RooStats/SPlot.h"

void sig_4pi()
{  
	using namespace std;
	using namespace RooFit;
  gROOT->SetStyle("Plain");
  gStyle->SetStatFont(132);
  gStyle->SetStatFontSize(0.08);
  gStyle->SetTitleFont(132,"XYZ"); 
  gStyle->SetLabelFont(132,"XYZ");
  gStyle->SetTitleSize(0.08,"XYZ"); 
  gStyle->SetLabelSize(0.08,"XYZ");
  gStyle->SetPadTopMargin(0.12);
  gStyle->SetPadBottomMargin(0.2);
  gStyle->SetPadLeftMargin(0.2);
  gStyle->SetPadRightMargin(0.03);
  
  RooMsgService::instance().setGlobalKillBelow(RooFit::ERROR);
	
  TPaveText * lhcbpreliminary = new TPaveText(0.6,0.67,0.85,0.85,"TR NDC");
  lhcbpreliminary->SetBorderSize(0); lhcbpreliminary->SetFillStyle(0);
  lhcbpreliminary->SetTextFont(132); lhcbpreliminary->SetTextSize(0.08); 
  lhcbpreliminary->AddText("LHCb Preliminary");
  lhcbpreliminary->AddText("343 pb^{-1}");

  int n = 0;
  Double_t MIN=0;                         // minimum value of the scanned parameter
  Double_t MAX=1.6;
  Double_t Syst=0.0138;
  Double_t minx=1.012, miny=42476.7;      // value and NLL of the approved result
  Double_t x[100], y[100], z[100], s[100];// likelihood scan data point follow...
  x[n] = 0.0; y[n] = 42512.7; z[n]=y[n]-miny; s[n]=sqrt(2*z[n]); n++;
  x[n] = 0.5; y[n] = 42483.1; z[n]=y[n]-miny; s[n]=sqrt(2*z[n]); n++;
  x[n] = 1.012; y[n] = 42476.7; z[n]=y[n]-miny; s[n]=sqrt(2*z[n]); n++;
  x[n] = 1.5; y[n] = 42480; z[n]=y[n]-miny; s[n]=sqrt(2*z[n]); n++;
  // x[n] = 0.8; y[n] = -20352.1; z[n]=y[n]-miny; s[n]=sqrt(2*z[n]); n++;
  // x[n] = 1.2; y[n] = -20354.3; z[n]=y[n]-miny; s[n]=sqrt(2*z[n]); n++;
  // x[n] = 1.66;y[n] = -20355.1; z[n]=y[n]-miny; s[n]=sqrt(2*z[n]); n++;
  // x[n] = 2.0; y[n] = -20354.7; z[n]=y[n]-miny; s[n]=sqrt(2*z[n]); n++;
  // x[n] = 2.4; y[n] = -20353.4; z[n]=y[n]-miny; s[n]=sqrt(2*z[n]); n++;
  // x[n] = 2.8; y[n] = -20351.4; z[n]=y[n]-miny; s[n]=sqrt(2*z[n]); n++;
  // x[n] = 3.2; y[n] = -20349.0; z[n]=y[n]-miny; s[n]=sqrt(2*z[n]); n++;

  TGraph * gr = new TGraph(n,x,z);
  gr->SetLineColor(2);
  gr->SetLineWidth(2);
  gr->SetLineStyle(2);
  gr->SetMarkerColor(2);
  gr->SetMarkerStyle(21);
  gr->SetTitle("");
  gr->GetYaxis()->SetTitle("#Delta(log(L))");
  gr->GetXaxis()->SetTitle("R_{CP}^{4#pi} ");

  TGraph * gs = new TGraph(n,x,s);
  gs->SetLineColor(2);
  gs->SetLineWidth(2);
  gs->SetLineStyle(2);
  gs->SetMarkerColor(2);
  gs->SetMarkerStyle(21);
  gs->SetTitle("");
  gs->GetXaxis()->SetTitle("R_{CP}^{4#pi} ");
  gs->GetYaxis()->SetTitle("#sigma=#surd(2#times#Delta(log(L)))");
  
  
  
  
  double likelMAX=-99999;
  double likelMAXCONV=-99999;
  Double_t LInterpolation[5000],xInterpolation[5000];
  int nInterpolation=100;
  double step=(MAX-MIN)/nInterpolation;
  double tmp=MIN;
  for(int i=0; i<=nInterpolation; i++){
    xInterpolation[i]=tmp;
    LInterpolation[i]=TMath::Exp(-gr->Eval(tmp,0,"S"));
    if(LInterpolation[i]>likelMAX) likelMAX=LInterpolation[i];
    tmp=tmp+step;
  }
  TGraph *gL = new TGraph(nInterpolation,xInterpolation,LInterpolation);
  gL->SetLineColor(2);
  gL->SetLineWidth(4);
  gL->SetMarkerColor(2);
  gL->SetMarkerStyle(0);
  gL->SetTitle("Likelihood");
  gL->GetXaxis()->SetTitle("R_{CP}^{4#pi} ");
  gL->GetYaxis()->SetTitle("Likelihood");
  //////////////////////////////////////////
  //CONVOLUTION
  //////////////////////////////////////////
  double zConvMIN=999999999;
  Double_t LConvoluted[5000];  Double_t zConv[5000]; Double_t zConvs[5000];
  double t=MIN;
  for(int i=0; i<=nInterpolation; i++){
    xInterpolation[i]=t;
    double sum=0;
    for(double TAU=MIN;TAU<=MAX; TAU=TAU+step){
      double left =gL->Eval( TAU      ,0,"S")*TMath::Gaus(t- TAU      ,0,Syst,kTRUE);
      double right=gL->Eval((TAU+step),0,"S")*TMath::Gaus(t-(TAU+step),0,Syst,kTRUE);
      sum = sum + ((left+right)/2*step);
    }
    LConvoluted[i]=(sum);
    if(LConvoluted[i]>likelMAXCONV) likelMAXCONV=LConvoluted[i];
    zConv[i]=-TMath::Log(LConvoluted[i]);
    if(zConv[i]<zConvMIN) zConvMIN=zConv[i];
    t=t+step;
  }
  //for(int i=0; i<=nInterpolation; i++) LConvoluted[i]=LConvoluted[i]/likelMAXCONV*likelMAX;
  for(int i=0; i<=nInterpolation; i++) zConv[i]=zConv[i]-zConvMIN;
  for(int i=0; i<=nInterpolation; i++) zConvs[i]=sqrt(2*zConv[i]);
  
  TGraph *gConv = new TGraph(nInterpolation,xInterpolation,LConvoluted);
  gConv->SetLineColor(4);
  gConv->SetLineWidth(4);
  gConv->SetMarkerColor(4);
  gConv->SetMarkerStyle(0);
  gConv->SetTitle("Likelihood Convoluted");
  gConv->GetXaxis()->SetTitle("R_{CP}^{4#pi} ");
  gConv->GetYaxis()->SetTitle("Likelihood Convoluted");
  TGraph *gzConv = new TGraph(nInterpolation,xInterpolation,zConv);
  TGraph *gzConvsv = new TGraph(nInterpolation,xInterpolation,zConvs);
  
  
  gzConv->SetLineColor(4);
  gzConv->SetLineWidth(2);
  gzConv->SetMarkerColor(4);
  gzConv->SetMarkerStyle(0);
  gzConv->SetTitle("Likelihood Convoluted");
  gzConv->GetXaxis()->SetTitle("R_{CP}^{4#pi} ");
  gzConv->GetYaxis()->SetTitle("Likelihood Convoluted");
  //////////////////////////////////////////////
  gzConvsv->SetLineColor(4);
  gzConvsv->SetLineWidth(2);
  gzConvsv->SetMarkerColor(4);
  gzConvsv->SetMarkerStyle(0);
  gzConvsv->SetTitle("Likelihood Convoluted");
  gzConvsv->GetXaxis()->SetTitle("R_{CP}^{4#pi} ");
  gzConvsv->GetYaxis()->SetTitle("Likelihood Convoluted");
  
  
  // Results
  std::cout << "Original Y intercept: " << gr->GetY()[0] << std::endl;
  std::cout << "Original significance: " << sqrt(2*gr->GetY()[0]) << std::endl;
  std::cout << "New Y intercept: " << gzConv->GetY()[0] << std::endl;
  std::cout << "New significance: " << sqrt(2*gzConv->GetY()[0]) << std::endl;
  
  
      
          
  TCanvas *c1 = new TCanvas("likelihood","",200,10,600,500);
	c1->Divide(2);
  c1->cd(1)->SetGrid();  
  gr->Draw("ACP");  gzConv->Draw("CPsame");
  //lhcbpreliminary->Draw();
  c1->cd(2)->SetGrid();
  //gs->Draw("ACP");gzConvsv->Draw("CPsame");
  gL->Draw("AC");
  gConv->Draw("Csame");
  // lhcbpreliminary->Draw();
  c1->SaveAs("4pi.pdf");
}
