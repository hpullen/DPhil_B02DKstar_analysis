#include <iostream>
#include <fstream>

#include "TCanvas.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1F.h"

#include "CutReader.hpp"

#include "FitDmass.hpp"
#include "FitBmass.hpp"

// =====================================================
// Script to estimate the amount of charmless background
// =====================================================
int main(int argc, char * argv[]) {

    // Get input args
    if (argc < 2) {
        std::cout << "Usage: ./Charmless <mode> <run=1/2/both>" << std::endl;
        return -1;
    }
    std::string mode = argv[1];
    std::string run = argv[2];
    bool high_stats = false;
    std::string window_opt = "";
    if (argc > 2) {
        for (int i = 3; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--high_stats") {
                high_stats = true;
            } else if (arg == "--low") {
                window_opt = "low";
            } else if (arg == "--high") {
                window_opt = "high";
            } else if (arg == "--low_shift") {
                window_opt = "low_shift";
            }
        }
    } 

    // Open files
    TChain * tree_full = new TChain("DecayTree");
    setPlotStyle();
    std::string bod = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi")
        ? "four" : "two";
    std::string dir = "/data/lhcb/users/pullen/B02DKstar/data/" + bod + "Body/";
    for (std::string mag : {"up", "down"}) {
        if (run == "1" || run == "both") {
            tree_full->Add((dir + "/2011_" + mag + "/" + mode + "_withBDTG.root").c_str());
            tree_full->Add((dir + "/2012_" + mag + "/" + mode + "_withBDTG.root").c_str());
        } if (run == "2" || run == "both") {
            tree_full->Add((dir + "/2015_" + mag + "/" + mode + "_withBDTG.root").c_str());
            tree_full->Add((dir + "/2016_" + mag + "/" + mode + "_withBDTG.root").c_str());
        }
    }

    // Reduce with selection cut (except FDS and D0 mass)
    TCut cut;
    CutReader * cr = new CutReader(mode);
    std::vector<std::string> to_ignore = {"D0_M ", "D0_FDS"};
    if (high_stats) to_ignore.push_back("BDTG");
    cut = cr->GetCutExcept(to_ignore);
    if (mode == "piK") {
        cut += "abs(sqrt(pow(Kstar_PE + D0K_PE, 2) - pow(Kstar_PX + D0K_PX, 2)"
                "- pow(Kstar_PY + D0K_PY, 2) - pow(Kstar_PZ + D0K_PZ,2))"
                "- 1864.83) > 25";
    }
    TFile * file_temp = TFile::Open("temp.root", "RECREATE");
    TTree * tree = tree_full->CopyTree(cut);

    // Plot KKpi mass
    TH1F * hist = new TH1F("hist", "", 100, 1500, 4000);
    TH1F * hist_B = new TH1F("hist_B", "", 100, 1500, 4000);
    tree->Draw("sqrt(pow(Kstar_PE + D0K_PE, 2) - pow(Kstar_PX + D0K_PX, 2)"
            "- pow(Kstar_PY + D0K_PY, 2) - pow(Kstar_PZ + D0K_PZ,2)) >> hist",
            "D0_M > 1864.83 - 50");
    tree->Draw("sqrt(pow(Kstar_PE + D0K_PE, 2) - pow(Kstar_PX + D0K_PX, 2)"
            "- pow(Kstar_PY + D0K_PY, 2) - pow(Kstar_PZ + D0K_PZ,2)) >> hist_B",
            "D0_M > (1864.83 - 50) && abs(Bd_M - 5279.81) < 25");
    TCanvas * canvas = new TCanvas("canv_mass", "", 900, 600);
    hist->SetLineWidth(1);
    hist->GetXaxis()->SetTitle("m(KK#pi) / MeV");
    hist_B->SetLineWidth(1);
    hist_B->SetLineColor(kRed);
    hist_B->SetFillColor(kRed);
    hist->Draw("HIST");
    hist_B->Draw("HIST SAME");
    canvas->SaveAs("Plots/checks/KKpi_mass.pdf");

    // Fit D0 mass with and without FDS cut
    double window_vs_sideband_ratio = FitDmass(tree, mode, run, high_stats, false, window_opt);    
    FitDmass(tree, mode, run, high_stats, true, window_opt);    

    // Fit B0 mass
    std::map<int, std::map<std::string, std::pair<double, double>>> yields = 
        FitBmass(tree, mode, run, high_stats, window_opt);

    // Save results to file
    std::string extra_dir = high_stats ? "/high_stats/" : "";
    std::string ext = (window_opt == "") ? "" : "_" + window_opt;
    std::ofstream file("Results/" + extra_dir + mode + "_run_" + run + ext + ".txt");
    for (auto cut : yields) {
        for (auto mode : cut.second) {
            file << cut.first << " " << mode.first << " " << 
                mode.second.first * window_vs_sideband_ratio << " " <<
                mode.second.second * window_vs_sideband_ratio << std::endl;
        }
    }
    file.close();
    file_temp->Close();
}
