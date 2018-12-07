#include <string>
#include <map>

#include "TTree.h"
#include "TH1F.h"
#include "TCut.h"
#include "TPaveText.h"

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
        std::string mode, std::string run, bool high_stats, std::string window_opt) {

    // Results map
    std::map<int, std::map<std::string, std::pair<double, double>>> yield_map;

    // Cut on D0 mass
    std::cout << "ENTRIES: " << tree->GetEntries("D0_M < 1864.83 - 50") << std::endl;
    TCut cut;
    if (mode == "pipi" || window_opt == "high") {
        cut = "D0_M > 1864.84 + 50";
        std::cout << "Fitting to upper D mass sideband only" << std::endl;
    } else if (mode == "pipipipi") {
        cut = "abs(D0_M - 1864.84) > 50 && abs(D0_M - 1864.84) < 100";
    } else if (mode == "KK" || window_opt == "low") {
        cut = "D0_M < 1864.84 - 50";
        std::cout << "Fitting to lower D mass sideband only" << std::endl;
    } else if (window_opt == "low_shift") {
        cut = "D0_M > 1864.84 - 65 && D0_M < 1864.84 - 40";
        std::cout << "Shifting D mass window" << std::endl;
    } else {
        cut = "abs(D0_M - 1864.84) > 50";
    }

    // B0 mass variable
    RooRealVar Bd_M("Bd_M", "", 5000, 5800);
    int binWidth = 20;
    double nBins = ((Bd_M.getMax() - Bd_M.getMin())/binWidth);
    Bd_M.setBins(nBins);

    // Canvas setup
    TCanvas * canvas = new TCanvas("B_canvas", "", 900, 1200);
    canvas->Divide(1, 2, 0.00001, 0.00001);

    // Loop over FDS cuts
    int count = 1;
    for (int cut_val : {0, 3}) {

        // D0 FDS variable
        RooRealVar D0_FDS("D0_FDS", "", cut_val, 100000);
        RooRealVar D0_M("D0_M", "", 0, 100000);

        // Make RooDataSet and apply D0 mass cut
        RooArgList args(Bd_M, D0_FDS, D0_M);
        RooDataSet * data_full_D0 = new RooDataSet("data", "", tree, args);
        RooDataSet * data = (RooDataSet*)data_full_D0->reduce(cut);

        // Fit parameters for B0 shape
        RooRealVar alpha_L("alpha_L", "", 1.31029);
        RooRealVar alpha_R("alpha_R", "", -1.23647);
        RooRealVar frac("frac", "", 0.596596);
        RooRealVar mean("mean", "", 5281.24);
        RooRealVar n_L("n_L", "", 8.33648);
        RooRealVar n_R("n_R", "", 9.99992);
        RooRealVar sigma_L("sigma_L", "", 15.2378);

        // Fit parameters for Bs shape
        RooRealVar Bs_alpha_L("Bs_alpha_L", "", 1.21084);
        RooRealVar Bs_alpha_R("Bs_alpha_R", "", -1.43578);
        RooRealVar Bs_frac("Bs_frac", "", 0.482737);
        RooRealVar Bs_mean("Bs_mean", "", 5368.78);
        RooRealVar Bs_n_L("Bs_n_L", "", 9.99999);
        RooRealVar Bs_n_R("Bs_n_R", "", 8.95325);
        RooRealVar Bs_sigma_L("Bs_sigma_L", "", 15.6907);

        // Exponential slope
        RooRealVar slope("slope", "", -0.0005, -0.01, -0.000001);

        // Yields
        int n_max = data->sumEntries();
        RooRealVar n_Bd("n_Bd", "", 1, -n_max/2, n_max/2);
        RooRealVar n_Bs("n_Bs", "", 1, -n_max/2, n_max/2);
        RooRealVar n_expo("n_expo", "", n_max/2, 0, n_max);

        // Make fit PDF
        RooCBShape CB_L("CB_L", "", Bd_M, mean, sigma_L, alpha_L, n_L);
        RooCBShape CB_R("CB_R", "", Bd_M, mean, sigma_L, alpha_R, n_R);
        RooAddPdf signal_Bd("signal_Bd", "", RooArgList(CB_L, CB_R), RooArgList(frac));
        RooCBShape Bs_CB_L("Bs_CB_L", "", Bd_M, Bs_mean, Bs_sigma_L, Bs_alpha_L, Bs_n_L);
        RooCBShape Bs_CB_R("Bs_CB_R", "", Bd_M, Bs_mean, Bs_sigma_L, Bs_alpha_R, Bs_n_R);
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
        data->plotOn(frame, RooFit::DrawOption("PZ"), RooFit::MarkerSize(1), RooFit::MarkerStyle(8), RooFit::LineWidth(1));
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
        frame->GetYaxis()->SetTitle(("Candidates / (" + std::to_string((int)nBins) + " MeV/#it{c}^{2})").c_str());
    
        // Save
        canvas->cd(count);
        frame->GetYaxis()->SetTitle(("Candidates / (" + std::to_string(binWidth) 
                    + " MeV/#it{c}^{2})").c_str());
        frame->GetXaxis()->SetTitle("#it{m}([" + latex_name + "]_{D}K^{*0}) " +
                    "[MeV/#it{c}^{2}]");
        frame->Draw();

        // Add label
        TPaveText text(0.7, 0.7, 0.73, 0.73);
        if (count == 1) text.AddText("(a)");
        else if (count == 2) text.AddText("(b)");
        text.Draw();

        // Add to map
        yield_map[cut_val].emplace("Bd", std::make_pair(n_Bd.getVal(), n_Bd.getError()));
        if (mode != "Kpi" && mode != "Kpipipi") {
            yield_map[cut_val].emplace("Bs", std::make_pair(n_Bs.getVal(), n_Bs.getError()));
        }
        count++;
    }

    std::string dir = high_stats ? "/high_stats/" : "";
    std::string ext = (window_opt == "") ? "" : "_" + window_opt;
    canvas->SaveAs(("Plots/" + dir + "B0_mass_fit_" + mode + "_run_" + run + ext + ".pdf").c_str());
    delete canvas;
    return yield_map;
}
