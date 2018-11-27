#include <iostream>
#include <string>

#include "TCanvas.h"
#include "TBox.h"
#include "TChain.h"
#include "TPad.h"
#include "TLine.h"
#include "TCut.h"
#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"

#include "CutReader.hpp"
#include "PlotStyle.hpp"

// =======================================================================================
// Script to plot selected candidates without the DK veto, and draw lines at veto position
// =======================================================================================
int main(int argc, char * argv[]) {

    // Get input args
    if (argc != 2) {
        std::cout << "Usage: ./PlotDKmass <mode>" << std::endl;
        return -1;
    }
    std::string mode = argv[1];

    // Open files
    std::string bod = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi") ?
        "fourBody" : "twoBody";
    TChain * chain = new TChain("DecayTree");
    for (std::string year : {"2011", "2012", "2015", "2016"}) {
        if (mode == "pipipipi" && (year == "2011" || year == "2012")) continue;
        for (std::string mag : {"up", "down"}) {
            chain->Add(("/data/lhcb/users/pullen/B02DKstar/data/" + bod + "/"
                        + year + "_" + mag + "/" + mode + "_withBDTG.root").c_str());
        }
    }

    // Plot DK and DPi masses

    // Plot invariant mass
    setPlotStyle();
    TH1F * hist = new TH1F("hist", "", 100, 5000, 5800);
    CutReader * cr = new CutReader(mode);
    TCut cut = cr->GetCutExcept("DK_mass");
    chain->Draw("DK_mass>>hist", cut);

    // Draw
    hist->SetLineWidth(1);
    hist->SetStats(false);
    hist->GetXaxis()->SetTitle("#it{m}(D^{0}K^{+}) [MeV/#it{c}^{2}]");
    hist->GetYaxis()->SetTitle("Candidates / (8 MeV/#it{c}^{2})");
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    hist->Draw("E");
    canvas->Update();

    // Make lines at veto
    double mass = 5279.81;
    double cut_width = 25.0;
    double max = gPad->GetUymax();
    TBox box(mass - cut_width, 0, mass + cut_width, max);
    box.SetLineColor(0);
    box.SetLineStyle(0);
    box.SetLineWidth(0);
    box.SetFillColorAlpha(kRed, 0.3);
    box.Draw();

    // Update
    hist->Draw("E SAME");
    gPad->RedrawAxis();
    canvas->SaveAs(("selected/D0K_inv_mass_" + mode + ".pdf").c_str());
}

