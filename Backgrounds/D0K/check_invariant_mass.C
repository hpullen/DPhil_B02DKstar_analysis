// ===================================
// Macro to plot D0 + K invariant mass
// ===================================
void check_invariant_mass(std::string mode, std::string particle, bool selected = false) {

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
    TH1F * hist; 
    if (particle == "K") {
        hist = new TH1F("hist", "", 100, 5000, 5800);
    } else {
        hist = new TH1F("hist", "", 200, 2000, 6000);
    }
    chain->Draw(("D" + particle + "_mass>>hist").c_str(), "KstarK_PIDK > 5");
    int bin_width = hist->GetBinWidth(1);

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
    TLine line3(mass, 0, mass, hist->GetMaximum() * 1.1);
    line3.SetLineColor(kRed);
    line3.SetLineStyle(2);
    line3.SetLineWidth(1);

    // Save
    hist->SetLineWidth(1);
    TString latex_particle = (particle == "K") ? "K" : "#pi";
    hist->GetXaxis()->SetTitle("#it{m}(D^{0}" + latex_particle + "^{+}) [MeV/#it{c}^{2}]");
    hist->GetYaxis()->SetTitle(("Candidates / (" + std::to_string(bin_width) + " MeV/#it{c}^{2})").c_str());
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    hist->Draw("E");
    if (particle == "K") {
        line.Draw();
        line2.Draw();
    } else {
        line3.Draw();
    }

    gPad->RedrawAxis();
    std::string dir = selected ? "selected" : "preselection";
    canvas->SaveAs((dir + "/D0" + particle + "_inv_mass_" + mode + ".pdf").c_str());
}
