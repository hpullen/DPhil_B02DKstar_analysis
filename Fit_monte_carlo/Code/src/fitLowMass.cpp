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

    // Check for Run 1 arg
    bool run1 = false;
    if (argc != 1) {
        if (std::string(argv[1]) == "--run1") {
            run1 = true;
            std::cout << "Fitting to Run 1 samples" << std::endl;
        }
    }
    
    setPlotStyle();
    gROOT->ForceStyle();

    // List of modes
    std::vector<std::string> parents = {"", "Bs_"};
    std::vector<std::string> particles = {"gamma", "pi"};
    std::vector<std::string> helicities = {"010", "101"};

    // Make TChains to hold data
    std::map<std::string, std::map<std::string, std::map<std::string, TChain*>>>
        trees;
    for (auto parent : parents) {
        for (auto particle : particles) {
            for (auto hel : helicities) {
                trees[parent][particle][hel] = new TChain("DecayTree");
            }
        }
    }

    // Read in data
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/";
    for (std::string mag : {"up", "down"}) {
        for (auto parent : parents) {
            for (auto particle : particles) {
                if (run1) {
                    trees[parent][particle]["010"]->Add((path + parent + "lowMass/" +
                                particle + "/010/2012_" + mag +
                                "/Kpi_selected.root").c_str());
                    trees[parent][particle]["101"]->Add((path + parent + "lowMass/" +
                                particle + "/001/2012_" + mag +
                                "/Kpi_selected.root").c_str());
                    trees[parent][particle]["101"]->Add((path + parent + "lowMass/" +
                                particle + "/100/2012_" + mag +
                                "/Kpi_selected.root").c_str());
                    if (parent == "") {
                        trees[parent][particle]["010"]->Add((path + parent + "lowMass/" +
                                    particle + "/010/2011_" + mag +
                                    "/Kpi_selected.root").c_str());
                        trees[parent][particle]["101"]->Add((path + parent + "lowMass/" +
                                    particle + "/001/2011_" + mag +
                                    "/Kpi_selected.root").c_str());
                        trees[parent][particle]["101"]->Add((path + parent + "lowMass/" +
                                    particle + "/100/2011_" + mag +
                                    "/Kpi_selected.root").c_str());
                    }
                } else {
                    trees[parent][particle]["010"]->Add((path + parent + "lowMass/" +
                                particle + "/010/2016_" + mag +
                                "/Kpi_selected.root").c_str());
                    trees[parent][particle]["101"]->Add((path + parent + "lowMass/" +
                                particle + "/001/2016_" + mag +
                                "/Kpi_selected.root").c_str());
                    trees[parent][particle]["101"]->Add((path + parent + "lowMass/" +
                                particle + "/100/2016_" + mag +
                                "/Kpi_selected.root").c_str());
                }
            }
        }
    }

    // Make mass variable
    RooRealVar Bd_M("Bd_ConsD_MD", "#it{m}(DK^{*0}) [MeV/#it{c}^{2}]", 4800, 5400);
    int binWidth = 8;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);
    RooArgList args;
    args.add(Bd_M);

    // Make RooDataSets
    std::map<std::string, std::map<std::string, std::map<std::string, RooDataSet*>>>
        data;
    for (auto parent : parents) {
        for (auto particle : particles) {
            for (auto hel : helicities) {
                data[parent][particle][hel] = new RooDataSet(("data_" + 
                            parent + particle + hel).c_str(), "",
                        trees[parent][particle][hel], args);
            }
        }
    }

    // Loop through and fit to each
    RooRealVar shift("shift", "", 0);
    RooRealVar delta_M("delta_M", "", 87.26);
    std::map<TString, RooAbsPdf*> pdfs;
    for (auto parent : parents) {
        for (auto particle : particles) {
            for (auto hel : helicities) {

                TString name = parent + particle + "_" + hel;
                std::cout << "Fitting " << name << std::endl;

                // Make fit variables
                // Kinematic endpoints
                double a_min;
                double a_max;
                double b_min;
                double b_max;
                if (particle == "pi") {
                    a_min = 5013;
                    a_max = 5028;
                    b_min = 5108;
                    b_max = 5120;
                } else {
                    a_min = 4910;;
                    a_max = 4923;
                    b_min = 5218;
                    b_max = 5230;
                }
                if (parent == "Bs_") {
                    a_min += 87.26; 
                    a_max += 87.26;
                    b_min += 87.26;
                    b_max += 87.26;
                }
                RooRealVar * a = new RooRealVar("a_" + name, "", a_min, a_max);
                RooRealVar * b = new RooRealVar("b_" + name, "", b_min, b_max);
                std::cout << "a: " << a_min << " - " << a_max << std::endl;
                std::cout << "b: " << b_min << " - " << b_max << std::endl;

                // Other variables
                RooRealVar * frac = new RooRealVar("frac_" + name, "", 0.9, 0, 1);
                RooRealVar * ratio = new RooRealVar("ratio_" + name, "", 8, 2, 15);
                RooRealVar * csi = new RooRealVar("csi_" + name, "", 0.8, 0.05, 1.5);
                RooRealVar * sigma = new RooRealVar("sigma_" + name, "", 15, 0, 50);
                RooRealVar * shift = new RooRealVar("shift", "", 0);

                // Make PDF
                RooAbsPdf * pdf;
                if (particle == "pi") { 
                    if (hel == "010") {
                        pdf = new RooHORNSdini("horns_" + name, "", Bd_M,
                                *a, *b, *csi, *shift, *sigma, *ratio, *frac);
                    }
                    else {
                        pdf = new RooHILLdini("hill_" + name, "", Bd_M,
                                *a, *b, *csi, *shift, *sigma, *ratio, *frac);
                    }
                } else {
                    if (hel == "010") {
                        pdf = new RooHILLdini("hill_" + name, "", Bd_M,
                                *a, *b, *csi, *shift, *sigma, *ratio, *frac);
                    }
                    else {
                        pdf = new RooLITTLEHORNSdini("horns_" + name, "", Bd_M,
                                *a, *b, *csi, *shift, *sigma, *ratio, *frac, *shift);
                    }
                }

                // Perform fit
                std::cout << "Fitting to " << data[parent][particle][hel]->sumEntries()
                    << " events." << std::endl;
                RooFitResult * r = pdf->fitTo(*data[parent][particle][hel], 
                        RooFit::Save(), RooFit::NumCPU(8, 2),
                        RooFit::Optimize(false), RooFit::Offset(true),
                        RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
                r->Print("v");

                // Plot
                TCanvas * canvas = new TCanvas("canvas", "", 900, 900);
                TCanvas * canvas_noPull = new TCanvas("canvas_noPull", "", 900, 600);
                RooPlot * plot = Bd_M.frame();
                data[parent][particle][hel]->plotOn(plot, RooFit::DrawOption("PZ"));
                pdf->plotOn(plot, RooFit::ProjWData(*data[parent][particle][hel]),
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
                std::string run = run1 ? "_run1" : "";
                canvas->SaveAs("../Plots/" + name + run + ".pdf");
                canvas_noPull->SaveAs("../Plots/" + name + run + "_noPull.pdf");

                // Output to parameter file
                std::ofstream file("../Results/lowMass_" + name + run + ".param");
                file << "a " << a->getVal() << " " << a->getError() << std::endl;
                file << "b " << b->getVal() << " " << b->getError() << std::endl;
                file << "csi " << csi->getVal() << " " << csi->getError() << std::endl;
                file << "sigma " << sigma->getVal() << " " << sigma->getError() << std::endl;
                file << "frac " << frac->getVal() << " " << frac->getError() << std::endl;
                file << "ratio " << ratio->getVal() << " " << ratio->getError() << std::endl;
                file.close();

                // Save a histogram
                TFile * outfile = TFile::Open("../Histograms/" + name + run + ".root",
                        "RECREATE");
                TH1F * hist = (TH1F*)pdf->createHistogram("fit", Bd_M,
                        RooFit::Binning(nBins * 10));
                hist->Write("fit");
                r->Write("fit_result");
                outfile->Close();

                // Put PDF in map
                if (parent == "Bs_") {
                    pdfs[name] = pdf;
                } else {

                    // Make version with a, b shifted upwards
                    RooFormulaVar * a_shifted = new RooFormulaVar("a_shifted_" + name,
                            "", "@0 + @1", RooArgList(*a, delta_M));
                    RooFormulaVar * b_shifted = new RooFormulaVar("b_shifted_" + name,
                            "", "@0 + @1", RooArgList(*b, delta_M));
                    if (particle == "pi") { 
                        if (hel == "010") {
                            pdfs[name] = new RooHORNSdini("horns_shifted_" + name, "", Bd_M,
                                    *a_shifted, *b_shifted, *csi, *shift, *sigma, *ratio, *frac);
                        }
                        else {
                            pdfs[name] = new RooHILLdini("hill_shifted_" + name, "", Bd_M,
                                    *a_shifted, *b_shifted, *csi, *shift, *sigma, *ratio, *frac);
                        }
                    } else {
                        if (hel == "010") {
                            pdfs[name] = new RooHILLdini("hill_shifted_" + name, "", Bd_M,
                                    *a_shifted, *b_shifted, *csi, *shift, *sigma, *ratio, *frac);
                        }
                        else {
                            pdfs[name] = new RooLITTLEHORNSdini("horns_shifted_" + name, "", Bd_M,
                                    *a_shifted, *b_shifted, *csi, *shift, *sigma, *ratio, *frac, *shift);
                        }
                    }
                }
            }
        }
    }

    // Make B0 and Bs comparison plots
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    for (auto p : particles) {
        for (auto h : helicities) {
            RooPlot * frame = Bd_M.frame(); 
            pdfs[("Bs_" + p + "_" + h).c_str()]->plotOn(frame, RooFit::LineColor(kRed));
            pdfs[(p + "_" + h).c_str()]->plotOn(frame, RooFit::LineColor(kBlue));
            canv->Clear();
            frame->Draw();
            canv->SaveAs(("../Plots/compare_B0_Bs_" + p + "_" + h + ".pdf").c_str());
        }
    }

}
