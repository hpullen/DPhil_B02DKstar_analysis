// ===================================
// Macro to plot D0 + K invariant mass
// ===================================
void check_invariant_mass(std::string mode, bool selected = false) {

    // Open files
    std::string bod = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi") ?
        "fourBody" : "twoBody";
    std::string ext = selected ? "_selected" : "_withBDTG";
    TChain * chain = new TChain("DecayTree");
    for (std::string year : {"2011", "2012", "2015", "2016"}) {
        if (mode == "pipipipi" && (year == "2011" || year == "2012")) continue;
        for (std::string mag : {"up", "down"}) {
            chain->Add(("/data/lhcb/users/pullen/B02DKstar/data/" + bod + "/"
                        + year + "_" + mag + "/" + mode + ext + ".root").c_str());
        }
    }

    // Plot invariant mass
    gROOT->ForceStyle();
    TH1F * hist = new TH1F("hist", "", 100, 5000, 5800);
    chain->Draw("DK_mass>>hist", "KstarK_PIDK > 5");

    // Make line at B mass
    double mass = 5279.81;
    double cut_width = 25.0;
    TLine line(mass - cut_width, 0, mass - cut_width, hist->GetMaximum() * 1.25);
    line.SetLineColor(kRed);
    line.SetLineStyle(2);
    line.SetLineWidth(1);
    TLine line2(mass + cut_width, 0, mass + cut_width, hist->GetMaximum() * 1.25);
    line2.SetLineColor(kRed);
    line2.SetLineStyle(2);
    line2.SetLineWidth(1);

    // Save
    hist->SetLineWidth(1);
    hist->GetXaxis()->SetTitle("#it{m}(D^{0}K^{+}) [MeV/#it{c}^{2}]");
    hist->GetYaxis()->SetTitle("Candidates / (8 MeV/#it{c}^{2})");
    TCanvas * canvas = new TCanvas("canavs", "", 900, 600);
    hist->Draw("E");
    line.Draw();
    line2.Draw();

    gPad->RedrawAxis();
    std::string dir = selected ? "selected" : "preselection";
    canvas->SaveAs((dir + "/D0K_inv_mass_" + mode + ".pdf").c_str());
}
