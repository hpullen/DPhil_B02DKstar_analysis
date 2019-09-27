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
#include "TLegend.h"

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

    // Loop through plot each histogram
    for (auto particle : particles) {
        for (auto hel : helicities) {

            TString name = particle + "_" + hel;
            std::cout << "Plotting " << name << std::endl;

            // Make histograms
            TH1F * hist = new TH1F("hist_" + name, "", 50, 5000, 5600);
            TH1F * hist_weighted = new TH1F("hist_weighted_" + name, "", 50, 5000, 5600);
            trees[particle][hel]->Draw("Bd_ConsD_MD >> hist_" + name);
            trees[particle][hel]->Draw("Bd_ConsD_MD >> hist_weighted_" + name, "PID_efficiency");
            hist_weighted->Scale(hist->Integral()/hist_weighted->Integral());
            hist->SetLineColor(kBlack);
            hist_weighted->SetLineColor(kRed);
            hist_weighted->SetStats(false);
            hist_weighted->GetXaxis()->SetTitle("B^{0} invariant mass");
            hist_weighted->GetYaxis()->SetTitle("Entries");

            // Plot
            TCanvas * canvas = new TCanvas("canvas", "", 900, 900);
            TPad * pad1 = new TPad("fit", "", 0.0, 1.0/3.0, 1.0, 1.0);
            pad1->cd();
            hist_weighted->Draw("E");
            hist->Draw("E SAME");

            // Make legend
            TLegend leg(0.55, 0.6, 0.85, 0.9);
            leg.SetFillStyle(0);
            leg.AddEntry(hist, "Unweighted");
            leg.AddEntry(hist_weighted, "Weighted");
            leg.Draw();

            // Plot on canvas
            canvas->cd();
            pad1->Draw();

            // Get and plot difference
            TH1F * pulls = new TH1F("pulls_" + name, "", 50, 5000, 5600);
            for (int i = 1; i <= pulls->GetNbinsX(); i++) {
                double pull;
                if (hist->GetBinError(i) == 0) {
                    pull = 0;
                } else {
                    pull = (hist->GetBinContent(i) - hist_weighted->GetBinContent(i))/hist->GetBinError(i);
                }
                pulls->SetBinContent(i, pull);
            }
            TPad * pad2 = new TPad("pullPad", "", 0.0, 0.0, 1.0, 1.0/3.0);
            pad2->cd();
            pulls->SetLineColor(0);
            pulls->SetLineWidth(0);
            pulls->SetFillStyle(1001);
            pulls->SetFillColor(kBlue);
            pulls->GetYaxis()->SetRangeUser(-5, 5);
            pulls->Draw();
            canvas->cd();
            pad2->Draw();

            // Save
            canvas->SaveAs("../Plots/" + name + "_comparison.pdf");

        }
    }
}
