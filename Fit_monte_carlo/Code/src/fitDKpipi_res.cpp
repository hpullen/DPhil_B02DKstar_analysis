#include <fstream>
#include <iostream>

#include "TApplication.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TString.h"

#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooCBShape.h"
#include "RooGaussian.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooHILLdini.h"
#include "RooHist.h"
#include "RooHORNSdini.h"
#include "RooPlotable.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "PlotStyle.hpp"

// ==========================================
// Fit to B+ -> D0 K*0 pi+ shape with RooHILL
// ==========================================
int main(int argc, char * argv[]) {

    // Get background number
    if (argc != 2) {
        std::cout << "Usage: ./FitDKpipi <bg-number>" << std::endl;
        return -1;
    }
    int bg_num = atoi(argv[1]);

    // Get name
    std::string name;
    std::string dir;
    switch (bg_num) {
        case 1 : name = "1_Kstar_1680";
                 break;
        case 2 : name = "2_Kstar_892";
                 break;
        case 3 : name = "3_K1_1400";
                 break;
        case 5 : name = "5_Kstar_1410";
                 break;
        case 7 : name = "7_K1_1270";
                 break;
    }
    if (bg_num == 2) {
        dir = name;
    } else {
        dir = name + "/Kstar";
    }

    // Get data
    TFile * file = TFile::Open(("/data/lhcb/users/pullen/RapidSim/DKpipi/" + dir +
            "/tree.root").c_str(), "READ");
    TTree * tree = (TTree*)file->Get("DecayTree");

    // Cut on daughter masses
    TString cut = "(abs(D0_M - 1.86484) < 0.025 && abs(Kstar_M - 0.8955) < 0.05)";

    double min = 5000;
    double max = 5500;
    RooRealVar * Bd_M = new RooRealVar("Bd_M", "", min, max);
    TH1F * hist = new TH1F("hist", "", 100, min, max);
    tree->Draw("Bd_M*1000>>hist", cut);
    RooDataHist * data = new RooDataHist("data", "", *Bd_M, RooFit::Import(*hist));

    // Make parameter file
    std::ofstream pfile("../Results/DKpipi_" + name + ".param");

    // Gaussian + HILL fit
    setPlotStyle();
    gROOT->ForceStyle();
    if (bg_num != 2) {


        // Make RooHILLdini
        RooRealVar a("a", "", 4700, 5000, 4400);
        RooRealVar b("b", "", 5139.69);
        // RooRealVar csi("csi", "", 0, -1, 1);
        RooRealVar csi("csi", "", 1);
        RooRealVar sigma("sigma", "", 5, 0, 100);
        RooRealVar ratio("ratio", "", 5, 1, 15);
        RooRealVar frac("frac", "", 0.5, 0, 1);
        RooRealVar shift("shift", "", 0);
        RooHILLdini hill("hill", "", *Bd_M, a, b, csi, shift, sigma, ratio, frac);

        // Make Gaussian
        RooRealVar sigma_gauss("sigma_gauss", "", 50, 0, 200);
        RooRealVar sigma_gauss2("sigma_gauss2", "", 50, 0, 200);
        // RooFormulaVar mean_gauss("mean_gauss", "(@0 + @1)/2", RooArgList(a, b));
        RooRealVar mean_gauss("mean_gauss", "", 5000, 4700, 5100);
        RooGaussian gauss("gauss", "", *Bd_M, mean_gauss, sigma_gauss);
        RooRealVar f_gauss("f_gauss", "", 0.2, 0, 1);
        RooAddPdf model("model", "", RooArgList(gauss, hill), RooArgList(f_gauss));

        // Perform fit
        RooFitResult * r = model.fitTo(*data, RooFit::Save(), 
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
        r->Print("v");

        // Save results
        pfile << "a " << a.getVal() << " " << a.getError() << std::endl;
        pfile << "b " << b.getVal() << std::endl;
        pfile << "csi " << csi.getVal() << " " << csi.getError() << std::endl;
        pfile << "sigma " << sigma.getVal() << " " << sigma.getError() << std::endl;
        pfile << "ratio " << ratio.getVal() << " " << ratio.getError() << std::endl;
        pfile << "frac " << frac.getVal() << " " << frac.getError() << std::endl;
        pfile << "sigma_gauss " << sigma_gauss.getVal() << " " << sigma_gauss.getError() << std::endl;
        pfile << "sigma_gauss2 " << sigma_gauss2.getVal() << " " << sigma_gauss2.getError() << std::endl;
        pfile << "mean_gauss " << mean_gauss.getVal() << " " << mean_gauss.getError() << std::endl;
        pfile << "f_gauss " << f_gauss.getVal() << " " << f_gauss.getError() << std::endl;

        // Plot
        RooPlot * plt = Bd_M->frame();
        data->plotOn(plt, RooFit::MarkerStyle(1), RooFit::DrawOption("E"));
        model.plotOn(plt, RooFit::LineColor(kBlack));
        RooHist * pulls = plt->pullHist();
        model.plotOn(plt, RooFit::Components("gauss"), RooFit::LineColor(kRed));
        model.plotOn(plt, RooFit::Components("hill"), RooFit::LineColor(kBlue));
        TCanvas * canvas = new TCanvas("canvas", "", 900, 900);
        TPad * pad1 = new TPad("plot", "", 0.0, 1.0/3, 1.0, 1.0);
        pad1->cd();
        plt->Draw();
        TPad * pad2 = new TPad("pullplot", "", 0.0, 0.0, 1.0, 1.0/3);
        pad2->cd();
        RooPlot * plt2 = Bd_M->frame();
        plt2->addPlotable(pulls, "BEX0");
        plt2->Draw();
        canvas->cd();
        pad1->Draw();
        pad2->Draw();
        canvas->SaveAs(("../Plots/DKpipi_" + name + ".pdf").c_str());

    } else {

        // RooHILL fit
        // Make RooHILLdini
        RooRealVar a("a", "", 2760.65);
        RooRealVar b("b", "", 5139.69);/* , 5130, 5160); */
        RooRealVar csi("csi", "", -5, -9, 0.0);
        RooRealVar sigma("sigma", "", 5, 0, 100);
        RooRealVar ratio("ratio", "", 5, 1, 15);
        RooRealVar frac("frac", "", 0.5, 0, 1);
        RooRealVar shift("shift", "", 0, -10, 10);
        RooHILLdini model("model", "", *Bd_M, a, b, csi, shift, sigma, ratio, frac);

        // Perform fit
        RooFitResult * r = model.fitTo(*data, RooFit::Save(), 
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
        r->Print("v");

        // Save results
        pfile << "a " << a.getVal() << std::endl;
        pfile << "b " << b.getVal() << std::endl;
        pfile << "csi " << csi.getVal() << " " << csi.getError() << std::endl;
        pfile << "sigma " << sigma.getVal() << " " << sigma.getError() << std::endl;
        pfile << "ratio " << ratio.getVal() << " " << ratio.getError() << std::endl;
        pfile << "frac " << frac.getVal() << " " << frac.getError() << std::endl;
        pfile << "shift " << shift.getVal() << " " << shift.getError() << std::endl;

        // Plot
        RooPlot * plt = Bd_M->frame();
        data->plotOn(plt, RooFit::MarkerStyle(1), RooFit::DrawOption("E"));
        model.plotOn(plt);
        RooHist * pulls = plt->pullHist();
        TCanvas * canvas = new TCanvas("canvas", "", 900, 900);
        TPad * pad1 = new TPad("plot", "", 0.0, 1.0/3, 1.0, 1.0);
        pad1->cd();
        plt->Draw();
        TPad * pad2 = new TPad("pullplot", "", 0.0, 0.0, 1.0, 1.0/3);
        pad2->cd();
        RooPlot * plt2 = Bd_M->frame();
        plt2->addPlotable(pulls, "BEX0");
        plt2->Draw();
        canvas->cd();
        pad1->Draw();
        pad2->Draw();
        canvas->SaveAs(("../Plots/DKpipi_" + name + ".pdf").c_str());
    }
    pfile.close();

}
