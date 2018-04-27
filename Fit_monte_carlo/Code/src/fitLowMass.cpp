#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "TROOT.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"

#include "RooGlobalFunc.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooAbsPdf.h"
#include "RooCategory.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooHist.h"
#include "RooPlotable.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "Plotter.hpp"
#include "RooHORNSdini.h"
#include "RooHILLdini.h"
#include "RooLITTLEHORNSdini.h"

#include "PlotStyle.hpp"

// ====================================================
// Program for fitting to low mass background MC shapes
// ====================================================
int main(int argc, char * argv[]) {
    
    setPlotStyle();
    gROOT->ForceStyle();

    // List of modes
    std::vector<std::string> modes = {
        "gamma_010",
        "gamma_101",
        "pi_010",
        "pi_101",
        "Bs_gamma_010",
        "Bs_gamma_101",
        "Bs_pi_010",
        "Bs_pi_101"
    };

    // Make TChains to hold data
    std::map<std::string, TChain*> trees;
    for (auto mode : modes) {
        trees[mode] = new TChain("DecayTree");
    }

    // Read in data
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/";
    for (std::string mag : {"up", "down"}) {
        for (std::string parent : {"Bs_", ""}) {
            for (std::string particle : {"pi", "gamma"}) {
                trees[parent + particle + "_010"]->Add((path + parent + "lowMass/" +
                            particle + "/010/2016_" + mag +
                            "/Kpi_selected.root").c_str());
                trees[parent + particle + "_101"]->Add((path + parent + "lowMass/" +
                            particle + "/001/2016_" + mag +
                            "/Kpi_selected.root").c_str());
                trees[parent + particle + "_101"]->Add((path + parent + "lowMass/" +
                            particle + "/100/2016_" + mag +
                            "/Kpi_selected.root").c_str());
            }
        }
    }

    // Make mass variable
    RooRealVar Bd_M("Bd_ConsD_MD", "#it{m}(DK^{*0}) [MeV/#it{c}^{2}]", 4800, 5450);
    int binWidth = 5;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);
    RooArgList args;
    args.add(Bd_M);

    // Make RooDataSets
    std::map<std::string, RooDataSet*> dataMap;
    for (auto tree : trees) {
        dataMap[tree.first] = new RooDataSet(("data_" + tree.first).c_str(), "",
                tree.second, args);
    }

    // Make shared variables
    RooRealVar shift("shift", "", 0);
    std::map<std::string, RooAbsReal*> endpoints;
    endpoints["a_gamma"] = new RooRealVar("a_gamma", "", 4919.1);
    endpoints["b_gamma"] = new RooRealVar("b_gamma", "", 5225.5, 5222, 5228);
    endpoints["a_pi"] = new RooRealVar("a_pi", "", 5021.6, 5020, 5023);
    endpoints["b_pi"] = new RooRealVar("b_pi", "", 5116.9, 5112, 5118);

    // Bs endpoints
    RooRealVar delta_M("delta_M", "", 87.29, 87.29 - 5, 87.29 + 5);
    for (std::string endpoint : {"a_gamma", "a_pi", "b_gamma", "b_pi"}) {
        endpoints["Bs_" + endpoint] = new RooFormulaVar(("Bs_" + endpoint).c_str(),
                "", "@0 + @1", RooArgList(*endpoints[endpoint], delta_M));
    }

    // Individual fit parameters
    std::map<std::string, std::map<std::string, RooRealVar*>> vars;
    for (auto mode : modes) {
        vars["frac"][mode] = new RooRealVar(("frac_" + mode).c_str(), "", 0.9, 0, 1);
        vars["ratio"][mode] = new RooRealVar(("ratio_" + mode).c_str(), "", 8, 2, 15);
        vars["csi"][mode] = new RooRealVar(("csi_" + mode).c_str(), "", 0.7, 0.05, 1);
        vars["sigma"][mode] = new RooRealVar(("sigma_" + mode).c_str(), "", 15, 0, 50);
    }

    // Make PDFs
    std::map<std::string, RooAbsPdf*> pdfs;
    for (std::string parent : {"Bs_", ""}) {
        pdfs[parent + "pi_010"] = new RooHORNSdini((parent + "pi_010").c_str(), "",
                Bd_M, *endpoints[parent + "a_pi"], *endpoints[parent + "b_pi"],
                *vars["csi"][parent + "pi_010"], shift,
                *vars["sigma"][parent + "pi_010"], *vars["ratio"][parent + "pi_010"],
                *vars["frac"][parent + "pi_010"]);
        pdfs[parent + "pi_101"] = new RooHILLdini((parent + "pi_101").c_str(), "",
                Bd_M, *endpoints[parent + "a_pi"], *endpoints[parent + "b_pi"],
                *vars["csi"][parent + "pi_101"], shift,
                *vars["sigma"][parent + "pi_101"], *vars["ratio"][parent + "pi_101"],
                *vars["frac"][parent + "pi_101"]);
        pdfs[parent + "gamma_010"] = new RooHILLdini((parent + "gamma_010").c_str(), "",
                Bd_M, *endpoints[parent + "a_gamma"], *endpoints[parent + "b_gamma"],
                *vars["csi"][parent + "gamma_010"], shift,
                *vars["sigma"][parent + "gamma_010"], *vars["ratio"][parent + "gamma_010"],
                *vars["frac"][parent + "gamma_010"]);
        pdfs[parent + "gamma_101"] = new RooLITTLEHORNSdini((parent + "gamma_101").c_str(), "",
                Bd_M, *endpoints[parent + "a_gamma"], *endpoints[parent + "b_gamma"],
                *vars["csi"][parent + "gamma_101"], shift,
                *vars["sigma"][parent + "gamma_101"], *vars["ratio"][parent + "gamma_101"],
                *vars["frac"][parent + "gamma_101"], shift);
    }

    // Create categories
    RooCategory cat("cat", "");
    for (auto mode : modes) {
        cat.defineType(mode.c_str());
    }

    // Make combined dataset
    RooDataSet combData("combData", "", Bd_M, RooFit::Index(cat),
            RooFit::Import(dataMap));

    // Make simultaneous PDF
    RooSimultaneous * simPdf = new RooSimultaneous("simPdf", "", cat);
    for (auto pdf : pdfs) {
        simPdf->addPdf(*pdf.second, pdf.first.c_str());
    }

    // Perform fit
    RooFitResult * r = simPdf->fitTo(combData, RooFit::Save(), RooFit::NumCPU(8, 2),
            RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // Make plots
    TCanvas * canvas = new TCanvas("canvas", "", 900, 900);
    TCanvas * canvas_noPull = new TCanvas("canvas_noPull", "", 900, 600);
    for (auto mode : modes) {
        canvas->Clear();
        canvas_noPull->Clear();

        // Plot data and fit
        RooPlot * plot = Bd_M.frame();
        combData.plotOn(plot, RooFit::Cut(("cat == cat::" + mode).c_str()),
                    RooFit::DrawOption("PZ"));
        simPdf->plotOn(plot, RooFit::Slice(cat, mode.c_str()),
                RooFit::ProjWData(cat, combData),
                RooFit::LineColor(ANABlue), RooFit::LineWidth(2),
                RooFit::DrawOption("C"));
        TPad * pad1 = new TPad("fit", "", 0.0, 1.0/3.0, 1.0, 1.0);
        pad1->cd();
        plot->Draw();
        canvas->cd();
        pad1->Draw();
        canvas_noPull->cd();
        plot->Draw();

        // Plot pulls
        RooHist * pulls = plot->pullHist();
        RooPlot * pull_frame = Bd_M.frame();
        pull_frame->addPlotable(pulls, "BEX0");
        TPad * pad2 = new TPad("pullPad", "", 0.0, 0.0, 1.0, 1.0/3.0);
        pad2->cd();
        pull_frame->Draw();
        canvas->cd();
        pad2->Draw();

        // Save
        canvas->SaveAs(("../Plots/" + mode + ".pdf").c_str());
        canvas_noPull->SaveAs(("../Plots/" + mode + "_noPull.pdf").c_str());
    }
}
