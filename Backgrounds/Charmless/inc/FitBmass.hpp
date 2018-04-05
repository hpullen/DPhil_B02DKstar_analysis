#include <string>
#include <map>

#include "TTree.h"
#include "TH1F.h"
#include "TCut.h"

#include "RooAddPdf.h"
#include "RooFormulaVar.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooCBShape.h"
#include "RooRealVar.h"
#include "RooExponential.h"

#include "PlotStyle.hpp"

// ================================================================
// Script for fitting and plotting B mass distribution in sidebands
// ================================================================
std::map<int, std::map<std::string, std::pair<double, double>>> FitBmass(TTree * tree, 
        std::string mode, std::string run) {

    // Results map
    std::map<int, std::map<std::string, std::pair<double, double>>> yield_map;

    // Cut on D0 mass
    TCut cut;
    if (mode == "pipi" || mode == "pipipipi") {
        cut = "D0_M > 1864.84 + 50";
    } else if (mode == "KK") {
        cut = "D0_M < 1864.84 - 50";
    } else {
        cut = "abs(D0_M - 1864.84) > 50";
    }

    // B0 mass variable
    RooRealVar Bd_M("Bd_M", "", 5000, 5800);
    int binWidth = 8;
    double nBins = ((Bd_M.getMax() - Bd_M.getMin())/binWidth);
    Bd_M.setBins(nBins);

    // Canvas setup
    TCanvas * canvas = new TCanvas("B_canvas", "", 900, 1200);
    canvas->Divide(1, 2, 0.00001, 0.00001);

    // Loop over FDS cuts
    int count = 1;
    for (int cut_val : {0, 2}) {

        // D0 FDS variable
        RooRealVar D0_FDS("D0_FDS", "", cut_val, 100000);
        RooRealVar D0_M("D0_M", "", 0, 100000);

        // Make RooDataSet and apply D0 mass cut
        RooArgList args(Bd_M, D0_FDS, D0_M);
        RooDataSet * data_full_D0 = new RooDataSet("data", "", tree, args);
        RooDataSet * data = (RooDataSet*)data_full_D0->reduce(cut);

        // Fit parameters for B0 shape
        RooRealVar alpha_L("alpha_L", "", 1.89541);
        RooRealVar alpha_R("alpha_R", "", -3.00005);
        RooRealVar frac("frac", "", 0.274661);
        RooRealVar mean("mean", "", 5279.88);
        RooRealVar n_L("n_L", "", 2.48425);
        RooRealVar n_R("n_R", "", 0.76137);
        RooRealVar sigma_L("sigma_L", "", 20.035);
        RooRealVar sigma_ratio("sigma_ratio", "", 0.544588);
        RooFormulaVar sigma_R("sigma_R", "@0 * @1", RooArgList(sigma_L, sigma_ratio));

        // Fit parameters for Bs shape
        RooRealVar Bs_alpha_L("Bs_alpha_L", "", 1.43642);
        RooRealVar Bs_alpha_R("Bs_alpha_R", "", -2.80021);
        RooRealVar Bs_frac("Bs_frac", "", 0.36109);
        RooRealVar Bs_mean("Bs_mean", "", 5367.08);
        RooRealVar Bs_n_L("Bs_n_L", "", 2.9686);
        RooRealVar Bs_n_R("Bs_n_R", "", 0.873806);
        RooRealVar Bs_sigma_L("Bs_sigma_L", "", 18.6779);
        RooRealVar Bs_sigma_ratio("Bs_sigma_ratio", "", 0.603092);
        RooFormulaVar Bs_sigma_R("Bs_sigma_R", "@0 * @1", 
                RooArgList(Bs_sigma_L, Bs_sigma_ratio));

        // Exponential slope
        RooRealVar slope("slope", "", -0.0005, -0.01, -0.000001);

        // Yields
        int n_max = data->sumEntries();
        RooRealVar n_Bd("n_Bd", "", 1, 0, n_max/2);
        RooRealVar n_Bs("n_Bs", "", 1, 0, n_max/2);
        RooRealVar n_expo("n_expo", "", n_max/2, 0, n_max);

        // Make fit PDF
        RooCBShape CB_L("CB_L", "", Bd_M, mean, sigma_L, alpha_L, n_L);
        RooCBShape CB_R("CB_R", "", Bd_M, mean, sigma_R, alpha_R, n_R);
        RooAddPdf signal_Bd("signal_Bd", "", RooArgList(CB_L, CB_R), RooArgList(frac));
        RooCBShape Bs_CB_L("Bs_CB_L", "", Bd_M, Bs_mean, Bs_sigma_L, Bs_alpha_L, Bs_n_L);
        RooCBShape Bs_CB_R("Bs_CB_R", "", Bd_M, Bs_mean, Bs_sigma_R, Bs_alpha_R, Bs_n_R);
        RooAddPdf signal_Bs("signal_Bs", "", RooArgList(Bs_CB_L, Bs_CB_R), 
                RooArgList(Bs_frac));
        RooExponential expo("expo", "", Bd_M, slope);
        RooAddPdf * model;
        if (mode == "Kpi" || mode == "Kpipipi") {
            model = new RooAddPdf("model", "", RooArgList(signal_Bd, expo), 
                    RooArgList(n_Bd, n_expo));
        } else {
            model = new RooAddPdf("model", "", RooArgList(signal_Bd, signal_Bs, expo), 
                    RooArgList(n_Bd, n_Bs, n_expo));
        }

        // Perform fit
        RooFitResult * r = model->fitTo(*data, RooFit::Save(true), 
                RooFit::Offset(true), RooFit::Minimizer("Minuit2", "migrad"), 
                RooFit::Strategy(2));
        r->Print();

        // Plot fit results
        setPlotStyle();
        RooPlot * frame = Bd_M.frame();
        data->plotOn(frame, RooFit::DrawOption("PZ"), RooFit::LineWidth(1));
        model->plotOn(frame, RooFit::Components("signal_Bd"), RooFit::LineWidth(2),
                    RooFit::LineColor(kRed), RooFit::ProjWData(*data),
                    RooFit::DrawOption("C"));
        if (mode != "Kpi" && mode != "Kpipipi") {
            model->plotOn(frame, RooFit::Components("signal_Bs"), RooFit::LineWidth(2),
                        RooFit::LineColor(ANAGreen), RooFit::ProjWData(*data),
                        RooFit::DrawOption("C"));
        }
        model->plotOn(frame, RooFit::Components("expo"), RooFit::LineWidth(2),
                    RooFit::LineColor(ANABlue), RooFit::ProjWData(*data),
                    RooFit::DrawOption("C"));
        model->plotOn(frame, RooFit::DrawOption("C"), RooFit::LineWidth(2), 
                RooFit::ProjWData(*data), RooFit::LineColor(kBlack));

        // Set axis titles
        TString latex_name;
        if (mode == "Kpi") latex_name = "K#pi";
        else if (mode == "piK") latex_name = "#pi K";
        else if (mode == "KK") latex_name = "KK";
        else if (mode == "pipi") latex_name = "#pi#pi";
        else if (mode == "Kpipipi") latex_name = "K#pi#pi#pi";
        else if (mode == "piKpipi") latex_name = "#piK#pi#pi";
        else if (mode == "pipipipi") latex_name = "#pi#pi#pi#pi";
        frame->GetXaxis()->SetTitle("m([" + latex_name + "]_{D}) [MeV/#it{c}^{2}]");
        frame->GetYaxis()->SetTitle("Candidates / (2 MeV/#it{c}^{2})");
    
        // Save
        canvas->cd(count);
        count++;
        frame->GetYaxis()->SetTitle(("Candidates / (" + std::to_string(binWidth) 
                    + " MeV/#it{c}^{2})").c_str());
        frame->GetXaxis()->SetTitle("#it{m}([" + latex_name + "]_{D}K^{*0}) " +
                    "[MeV/#it{c}^{2}]");
        frame->Draw();

        // Add to map
        yield_map[cut_val].emplace("Bd", std::make_pair(n_Bd.getVal(), n_Bd.getError()));
        if (mode != "Kpi" && mode != "Kpipipi") {
            yield_map[cut_val].emplace("Bs", std::make_pair(n_Bs.getVal(), n_Bs.getError()));
        }
    }

    canvas->SaveAs(("Plots/B0_mass_fit_" + mode + "_run_" + run + ".pdf").c_str());
    delete canvas;
    return yield_map;

}
