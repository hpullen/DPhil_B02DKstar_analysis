// ==============================================================
// Macro to plot D0 + K invariant mass for data vs. Monte Carlo
// ==============================================================
void compare_signal_MC(std::string mode) {

    // Open files: data
    TChain * chain = new TChain("DecayTree");
    for (std::string year : {"2011", "2012", "2015", "2016"}) {
        for (std::string mag : {"up", "down"}) {
            chain->Add(("/data/lhcb/users/pullen/B02DKstar/data/twoBody/"
                        + year + "_" + mag + "/" + mode + "_selected.root").c_str());
        }
    }

    // Open MC (2016)
    TChain * chain_MC = new TChain("DecayTree");
    for (std::string year : {"2016"}) {
        for (std::string mag : {"up", "down"}) {
            chain->Add(("/data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/"
                        + year + "_" + mag + "/" + mode + "_selected.root").c_str());
        }
    }

    // Plot invariant mass
    gROOT->ForceStyle();
    TH1F * hist = new TH1F("hist", "", 100, 5000, 5800);
    TString inv_mass = "sqrt(pow(D0_PE + KstarK_PE, 2) - "
        "pow(D0_PX + KstarK_PX, 2) - pow(D0_PY + KstarK_PY, 2) - "
        "pow(D0_PZ + KstarK_PZ, 2))";
    chain->Draw(inv_mass + ">>hist");
    hist->Scale(1/hist->Integral());
    TH1F * hist_MC = new TH1F("hist_MC", "", 100, 5000, 5800);
    chain_MC->Draw(inv_mass + ">>hist_MC");
    hist_MC->Scale(1/hist_MC->Integral());

    // Make line at B mass
    double mass = 5279.81;
    TLine line(mass, 0, mass, hist->GetMaximum() * 1.25);
    line.SetLineColor(ANAGreen);
    line.SetLineStyle(2);
    line.SetLineWidth(1);

    // Save
    hist->SetLineWidth(1);
    hist->GetXaxis()->SetTitle("#it{m}(D^{0}K^{+}) [MeV/#it{c}^{2}]");
    hist->GetYaxis()->SetTitle("Candidates / (8 MeV/#it{c}^{2})");
    hist_MC->SetLineWidth(1);
    hist_MC->SetLineColor(kRed);
    TCanvas * canvas = new TCanvas("canavs", "", 900, 600);
    hist->Draw("HIST");
    hist_MC->Draw("HIST SAME");
    line.Draw();
    gPad->RedrawAxis();
    canvas->SaveAs(("data_vs_MC_" + mode + ".pdf").c_str());
}
