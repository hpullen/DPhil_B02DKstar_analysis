#include <map>

#include "TChain.h"
#include "TLegend.h"
#include "TH1F.h"
#include "TCanvas.h"

#include "CutReader.hpp"
#include "PlotStyle.hpp"

// ==============================================================
// Script to look for possible mis-ID swaps in D sidebands of piK
// ==============================================================
int main(int argc, char * argv[]) {

    // Read in run 2 data
    std::string mode = "piK";
    std::string run = "2";
    TChain * tree = new TChain("DecayTree");
    setPlotStyle();
    std::string dir = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    for (std::string mag : {"up", "down"}) {
        if (run == "1" || run == "both") {
            tree->Add((dir + "/2011_" + mag + "/" + mode + "_withBDTG.root").c_str());
            tree->Add((dir + "/2012_" + mag + "/" + mode + "_withBDTG.root").c_str());
        } if (run == "2" || run == "both") {
            tree->Add((dir + "/2015_" + mag + "/" + mode + "_withBDTG.root").c_str());
            tree->Add((dir + "/2016_" + mag + "/" + mode + "_withBDTG.root").c_str());
        }
    }

    // Get selection cut minus D0 mass, BDT and FDS
    CutReader * cr = new CutReader(mode);
    std::vector<std::string> to_ignore = {"D0_M ", "D0_FDS", "BDTG"};
    TCut cut = cr->GetCutExcept(to_ignore);
    cut += "abs(D0_M -1864.83) > 50";
    TCut cut_with_B = cut + "abs(Bd_M - 5279.81) < 25";

    // Things to plot
    std::map<std::string, std::pair<std::string, std::string>> to_plot = {
        {"D0Pi_KstarK", std::make_pair("sqrt(pow(D0Pi_PE + KstarK_PE, 2) - "
                "pow(D0Pi_PX + KstarK_PX, 2) - pow(D0Pi_PY + KstarK_PY, 2) - "
                "pow(D0Pi_PZ + KstarK_PZ, 2))",
                "m(#pi_{D} K_{K^{*}})")},
        {"D0Pi_KstarK_swap", std::make_pair("sqrt(pow(sqrt(D0Pi_P*D0Pi_P + 493.7*493.7) + "
                "sqrt(KstarK_P*KstarK_P + 139.6*139.6), 2) - "
                "pow(D0Pi_PX + KstarK_PX, 2) - pow(D0Pi_PY + KstarK_PY, 2) - "
                "pow(D0Pi_PZ + KstarK_PZ, 2))",
                "swapped m(#pi_{D} K_{K^{*}})")},
        {"D0K_KstarPi", std::make_pair("sqrt(pow(D0K_PE + KstarPi_PE, 2) - "
                "pow(D0K_PX + KstarPi_PX, 2) - pow(D0K_PY + KstarPi_PY, 2) - "
                "pow(D0K_PZ + KstarPi_PZ, 2))",
                "m(#pi_{D} K_{K^{*}})")},
        {"D0K_KstarPi_swap", std::make_pair("sqrt(pow(sqrt(KstarPi_P*KstarPi_P + 493.7*493.7) + "
                "sqrt(D0K_P*D0K_P + 139.6*139.6), 2) - "
                "pow(D0K_PX + KstarPi_PX, 2) - pow(D0K_PY + KstarPi_PY, 2) - "
                "pow(D0K_PZ + KstarPi_PZ, 2))",
                "swapped m(#pi_{D} K_{K^{*}})")},
        {"diff_KstarK_swapped", std::make_pair("sqrt(pow(sqrt(D0Pi_P*D0Pi_P + 493.7*493.7) + "
                "sqrt(KstarK_P*KstarK_P + 139.6*139.6), 2) - "
                "pow(D0Pi_PX + KstarK_PX, 2) - pow(D0Pi_PY + KstarK_PY, 2) - "
                "pow(D0Pi_PZ + KstarK_PZ, 2)) - sqrt(pow(D0K_PE + KstarPi_PE, 2) - "
                "pow(D0K_PX + KstarPi_PX, 2) - pow(D0K_PY + KstarPi_PY, 2) - "
                "pow(D0K_PZ + KstarPi_PZ, 2))",
                "swapped m(#pi_{D} K_{K^{*}}) - m(#pi_{K^{*}} K_{D})")},
        {"diff_KstarPi_swapped", std::make_pair("sqrt(pow(sqrt(KstarPi_P*KstarPi_P + 493.7*493.7) + "
                "sqrt(D0K_P*D0K_P + 139.6*139.6), 2) - "
                "pow(D0K_PX + KstarPi_PX, 2) - pow(D0K_PY + KstarPi_PY, 2) - "
                "pow(D0K_PZ + KstarPi_PZ, 2)) - sqrt(pow(D0Pi_PE + KstarK_PE, 2) - "
                "pow(D0Pi_PX + KstarK_PX, 2) - pow(D0Pi_PY + KstarK_PY, 2) - "
                "pow(D0Pi_PZ + KstarK_PZ, 2))",
                "swapped m(#pi_{K^{*}} K_{D}) - m(#pi_{D} K_{K^{*}})")}};

    // Make all plots
    for (auto plt : to_plot) {

        // Make histograms
        TH1F * hist = new TH1F("hist", "", 100, 600, 2000);
        TH1F * hist_B = new TH1F("hist_B", "", 100, 600, 2000);
        tree->Draw((plt.second.first + ">>hist").c_str(), cut);
        tree->Draw((plt.second.first + ">>hist_B").c_str(), cut_with_B);

        // Plot
        TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
        hist->SetLineWidth(1);
        hist->SetLineColor(kBlack);
        hist->GetXaxis()->SetTitle(plt.second.second.c_str());
        hist->SetStats(false);
        hist->Draw();
        hist_B->SetLineWidth(1);
        hist_B->SetLineColor(kRed);
        hist_B->SetFillStyle(3335);
        hist_B->SetFillColor(kRed);
        hist_B->Draw("SAME");
        gPad->RedrawAxis();

        // Save
        TLegend leg(0.55, 0.75, 0.85, 0.85);
        leg.AddEntry(hist, "All");
        leg.AddEntry(hist_B, "#pm 25 MeV B mass window", "F");
        leg.Draw();
        canvas->SaveAs(("Plots/checks/" + plt.first + ".pdf").c_str());
        delete hist;
        delete hist_B;
        delete canvas;
    }

}
