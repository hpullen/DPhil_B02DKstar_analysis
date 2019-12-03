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
#include "RooHORNSdini_misID.h"
#include "RooHILLdini_misID.h"
#include "RooCBShape.h"
#include "RooAddPdf.h"

#include "PlotStyle.hpp"

// ====================================================
// Program for fitting to low mass background MC shapes
// ====================================================
int main(int argc, char * argv[]) {

    setPlotStyle();
    gROOT->ForceStyle();

    // List of modes
    std::vector<std::string> particles = {"gamma", "pi"};
    // std::vector<std::string> helicities = {"010", "101"};
    // std::vector<std::string> particles = {"gamma"};
    std::vector<std::string> helicities = {"010", "101"};

    // Make TChains to hold data
    std::map<std::string, std::map<std::string, TChain*>> trees;
    for (auto particle : particles) {
        for (auto hel : helicities) {
            trees[particle][hel] = new TChain("DecayTree");
        }
    }

    // Read in data
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/";
    for (std::string mag : {"up", "down"}) {
        for (auto particle : particles) {
            trees[particle]["010"]->Add((path + "rho_lowMass/" +
                        particle + "/010/2016_" + mag +
                        "/Kpi_selected_withPIDweights.root").c_str());
            trees[particle]["101"]->Add((path + "rho_lowMass/" +
                        particle + "/001/2016_" + mag +
                        "/Kpi_selected_withPIDweights.root").c_str());
            trees[particle]["101"]->Add((path + "rho_lowMass/" +
                        particle + "/100/2016_" + mag +
                        "/Kpi_selected_withPIDweights.root").c_str());
        }
    }

    // Make mass variable
    RooRealVar Bd_M("Bd_ConsD_MD", "#it{m}(DK^{*0}) [MeV/#it{c}^{2}]", 5000, 5600);
    RooRealVar PID_efficiency("PID_efficiency", "", 0, 1);
    int binWidth = 16;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);
    RooArgList args;
    args.add(Bd_M);
    args.add(PID_efficiency);

    // Make RooDataSets
    std::map<std::string, std::map<std::string, RooDataSet*>> data;
    for (auto particle : particles) {
        for (auto hel : helicities) {
            data[particle][hel] = new RooDataSet(("data_" + 
                        particle + hel).c_str(), "",
                    // trees[particle][hel], args);
                    trees[particle][hel], args, 0, "PID_efficiency");
        }
    }

    // Loop through and fit to each
    RooRealVar shift("shift", "", 0);
    for (auto particle : particles) {
        for (auto hel : helicities) {

            TString name = particle + "_" + hel;
            std::cout << "Fitting " << name << std::endl;

            // Make fit variables
            // Kinematic endpoints
            double a_min;
            double a_max;
            double b_min;
            double b_max;
            double a_start;
            double b_start;
            if (particle == "pi") {
                a_min = 5013;
                a_max = 5028;
                b_min = 5108;
                b_max = 5120;
                a_start = (a_max - a_min)/2;
                b_start = (b_max - b_min)/2;
            } else {
                a_min = 5000;;
                a_max = 5050;
                b_min = 5200;
                b_max = 5300;
                a_start = 5002;
                b_start = 5280;
            }
            RooRealVar * a = new RooRealVar("a_" + name, "", a_start, a_min, a_max);
            RooRealVar * b = new RooRealVar("b_" + name, "", b_start, b_min, b_max);
            std::cout << "a: " << a_min << " - " << a_max << std::endl;
            std::cout << "b: " << b_min << " - " << b_max << std::endl;

            // Other variables
            RooRealVar * frac = new RooRealVar("frac_" + name, "", 0.9, 0, 1);
            RooRealVar * ratio = new RooRealVar("ratio_" + name, "", 8, 2, 15);
            RooRealVar * csi = new RooRealVar("csi_" + name, "", 0.8, 0.05, 2);
            RooRealVar * sigma = new RooRealVar("sigma_" + name, "", 15, 0, 50);
            RooRealVar * shift = new RooRealVar("shift", "", 0);

            // Parameters for mis ID shapes
            RooRealVar * m1 = new RooRealVar("m1_" + name, "", 40, -100, 100);
            RooRealVar * m2 = new RooRealVar("m2_" + name, "", 80, -60, 140);
            RooRealVar * m3 = new RooRealVar("m3_" + name, "", 120, 20, 220);
            RooRealVar * m4 = new RooRealVar("m4_" + name, "", 140, 40, 240);
            RooRealVar * s1 = new RooRealVar("s1_" + name, "", 30, 0, 120);
            RooRealVar * s2 = new RooRealVar("s2_" + name, "", 30, 0, 120);
            RooRealVar * s3 = new RooRealVar("s3_" + name, "", 30, 0, 120);
            RooRealVar * s4 = new RooRealVar("s4_" + name, "", 30, 0, 120);
            RooRealVar * f1 = new RooRealVar("f1_" + name, "", 0.3, 0, 1);
            RooRealVar * f2 = new RooRealVar("f2_" + name, "", 0.3, 0, 1);
            RooRealVar * f3 = new RooRealVar("f3_" + name, "", 0.3, 0, 1);

            // Parameters for CB
            RooRealVar * mean = new RooRealVar("mean", "", 5100, 5050, 5200);
            RooRealVar * sigma_L = new RooRealVar("sigma_L", "", 40, 0, 80);
            RooRealVar * sigma_R = new RooRealVar("sigma_R", "", 20, 0, 50);
            RooRealVar * alpha_L = new RooRealVar("alpha_L", "", 7, 0.5, 10);
            RooRealVar * alpha_R = new RooRealVar("alpha_R", "", -0.4, -5, -0.001);
            RooRealVar * n_L = new RooRealVar("n_L", "", 1);
            RooRealVar * n_R = new RooRealVar("n_R", "", 9, 0, 10);
            RooRealVar * frac_CB = new RooRealVar("frac_CB", "", 0.3, 0, 1);

            // Make PDF
            RooAbsPdf * pdf;
            if (particle == "pi") { 
                if (hel == "010") {
                    pdf = new RooHORNSdini_misID("horns_" + name, "", Bd_M,
                            *a, *b, *csi, *m1, *s1, *m2, *s2, *m3, *s3, *m4, *s4,
                            *f1, *f2, *f3);
                    // pdf = new RooHORNSdini("horns_" + name, "", Bd_M,
                            // *a, *b, *csi, *shift, *sigma, *ratio, *frac);
                }
                else {
                    RooCBShape * pdf_L = new RooCBShape("pdf_L", "", Bd_M, *mean, *sigma_L,
                            *alpha_L, *n_L);
                    RooCBShape * pdf_R = new RooCBShape("pdf_R", "", Bd_M, *mean, *sigma_R,
                            *alpha_R, *n_R);
                    // pdf = new RooAddPdf("CB_" + name, "", RooArgList(*pdf_L, *pdf_R),
                            // RooArgList(*frac_CB));
                    pdf = pdf_R;

                    // pdf = new RooHILLdini_misID("horns_" + name, "", Bd_M,
                            // *a, *b, *csi, *m1, *s1, *m2, *s2, *m3, *s3, *m4, *s4,
                            // *f1, *f2, *f3);
                    // pdf = new RooHILLdini("hill_" + name, "", Bd_M,
                            // *a, *b, *csi, *shift, *sigma, *ratio, *frac);
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
            std::cout << "Fitting to " << data[particle][hel]->sumEntries()
                << " events." << std::endl;
            RooFitResult * r = pdf->fitTo(*data[particle][hel], 
                    RooFit::Save(), RooFit::NumCPU(8, 2),
                    RooFit::Optimize(false), RooFit::Offset(false),
                    RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
            r->Print("v");

            // Plot
            TCanvas * canvas = new TCanvas("canvas", "", 900, 900);
            TCanvas * canvas_noPull = new TCanvas("canvas_noPull", "", 900, 600);
            RooPlot * plot = Bd_M.frame();
            data[particle][hel]->plotOn(plot, RooFit::DrawOption("PZ"));
            pdf->plotOn(plot, RooFit::ProjWData(*data[particle][hel]),
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
            canvas->SaveAs("../Plots/" + name + ".pdf");
            canvas_noPull->SaveAs("../Plots/" + name + "_noPull.pdf");

            // Output to parameter file
            std::ofstream file("../Results/rho_lowMass_" + name + ".param");
            if (particle == "pi" && hel == "101") {
                file << "alpha_L " << alpha_L->getVal() << " " << alpha_L->getError() << std::endl;
                file << "alpha_R " << alpha_R->getVal() << " " << alpha_R->getError() << std::endl;
                file << "frac " << frac_CB->getVal() << " " << frac_CB->getError() << std::endl;
                file << "mean " << mean->getVal() << " " << mean->getError() << std::endl;
                file << "n_L " << n_L->getVal() << " " << n_L->getError() << std::endl;
                file << "n_R " << n_R->getVal() << " " << n_R->getError() << std::endl;
                file << "sigma_L " << sigma_L->getVal() << " " << sigma_L->getError() << std::endl;
                file << "sigma_R " << sigma_R->getVal() << " " << sigma_R->getError() << std::endl;
            } else {
                file << "a " << a->getVal() << " " << a->getError() << std::endl;
                file << "b " << b->getVal() << " " << b->getError() << std::endl;
                file << "csi " << csi->getVal() << " " << csi->getError() << std::endl;
                if (particle == "gamma") {
                    file << "sigma " << sigma->getVal() << " " << sigma->getError() << std::endl;
                    file << "frac " << frac->getVal() << " " << frac->getError() << std::endl;
                    file << "ratio " << ratio->getVal() << " " << ratio->getError() << std::endl;
                } else {
                    file << "m1 " << m1->getVal() << " " << m1->getError() << std::endl;
                    file << "m2 " << m2->getVal() << " " << m2->getError() << std::endl;
                    file << "m3 " << m3->getVal() << " " << m3->getError() << std::endl;
                    file << "m4 " << m4->getVal() << " " << m4->getError() << std::endl;
                    file << "s1 " << s1->getVal() << " " << s1->getError() << std::endl;
                    file << "s2 " << s2->getVal() << " " << s2->getError() << std::endl;
                    file << "s3 " << s3->getVal() << " " << s3->getError() << std::endl;
                    file << "s4 " << s4->getVal() << " " << s4->getError() << std::endl;
                    file << "f1 " << f1->getVal() << " " << f1->getError() << std::endl;
                    file << "f2 " << f2->getVal() << " " << f2->getError() << std::endl;
                    file << "f3 " << f3->getVal() << " " << f3->getError() << std::endl;
                }
            }
            file.close();

            // Save a histogram
            TFile * outfile = TFile::Open("../Histograms/" + name + ".root",
                    "RECREATE");
            TH1F * hist = (TH1F*)pdf->createHistogram("fit", Bd_M,
                    RooFit::Binning(nBins * 10));
            hist->Write("fit");
            r->Write("fit_result");
            outfile->Close();

        }
    }
}
