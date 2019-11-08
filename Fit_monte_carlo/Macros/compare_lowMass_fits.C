#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TROOT.h"


void compare_lowMass_fits(std::string type = "runs") {

    gROOT->ForceStyle();
    for (std::string missed : {"gamma_010", "gamma_101", "pi_010", "pi_101"}) {

        std::map<int, std::string> names;
        std::map<int, std::string> filenames;
        if (type == "runs") {
            names[0] = "Run 1";
            names[1] = "Run 2";
            filenames[0] = "_run1";
            filenames[1] = "";
            std::cout << "Plotting Run 1 vs. Run 2 comparison" << std::endl;
        } else if (type == "PID") {
            names[0] = "No PID cuts";
            names[1] = "With PID cuts";
            filenames[0] = "";
            filenames[1] = "_PIDcuts";
            std::cout << "Plotting PID cuts comparison for Run 2" << std::endl;
        } else if (type == "PID_run1") {
            names[0] = "No PID cuts";
            names[1] = "With PID cuts";
            filenames[0] = "_run1";
            filenames[1] = "_PIDcuts_run1";
            std::cout << "Plotting PID cuts comparison for Run 1" << std::endl;
        } else {
            std::cout << "Unrecognised comparison type: " << type << std::endl;
            return;
        }

        // Open files
        gROOT->ForceStyle();
        std::map<int, TFile*> files;
        for (auto f : filenames) {
            files[f.first] = TFile::Open(("/home/pullen/analysis/B02DKstar/Fit_monte_carlo/Histograms/" + missed + f.second + ".root").c_str(), "READ");
        }

        // Get fit histograms
        std::map<int, TH1F*> hists;
        std::map<int, int> colours = {{0, kRed}, {1, kBlue}};
        for (auto f : files) {
            hists[f.first] = (TH1F*)f.second->Get("fit");
            hists[f.first]->Scale(1/hists[f.first]->Integral());
            hists[f.first]->SetLineColor(colours[f.first]);
            hists[f.first]->SetStats(kFALSE);
        }

        // Make canvas and draw
        gROOT->ForceStyle();
        TCanvas * canv = new TCanvas("compare", "", 900, 600);
        hists[0]->Draw("HIST C");
        hists[1]->Draw("HIST C SAME");

        // Make legend
        TLegend * leg = new TLegend(0.2, 0.75, 0.5, 0.9);
        for (auto n : names) {
            leg->AddEntry(hists[n.first], n.second.c_str());
        }
        leg->SetFillStyle(0);
        leg->Draw();

        // Save 
        canv->SaveAs(("../Plots/Comparisons/compare_" + type + "_" + missed + ".pdf").c_str());
        delete canv;
    }
}
