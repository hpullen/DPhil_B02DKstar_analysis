// ===================================================
// Script to produce a plot showing D0 mass cut region
// ===================================================
void helicity_cut() {

    // Open signal
    TChain * sig_tree = new TChain("DecayTree");
    std::vector<std::string> years = {"2015_down", "2015_up", "2016_down", "2016_up"};
    for (auto year : years) {
        sig_tree->Add(("/data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/" + year +
                "/Full_helicity/Kpi.root").c_str());
    }

    // Open background
    TChain * bg_tree = new TChain("DecayTree");
    for (auto year : years) {
        bg_tree->Add(("/data/lhcb/users/pullen/B02DKstar/data/twoBody/" + year +
                "/Full_helicity/Kpi.root").c_str());
    }

    // Plot the helicity distribution
    TH1F * hel_hist_sig = new TH1F("hel_hist_sig", "", 200, -1, 1);
    sig_tree->Draw("Kstar_helicity_angle>>hel_hist_sig", "Bd_BKGCAT == 0");
    TH1F * hel_hist_bg = new TH1F("hel_hist_bg", "", 200, -1, 1);
    bg_tree->Draw("Kstar_helicity_angle>>hel_hist_bg", "Bd_ConsD_M > 5800");

    // Draw
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    hel_hist_sig->SetLineWidth(1);
    hel_hist_sig->SetMarkerSize(0);
    hel_hist_sig->GetXaxis()->SetTitle("cos(#theta)");
    hel_hist_sig->GetYaxis()->SetTitle("Candidates");
    double y_max = hel_hist_sig->GetMaximum() * 1.05;
    hel_hist_sig->GetYaxis()->SetRangeUser(0, y_max);
    hel_hist_sig->Draw("HIST");
    hel_hist_bg->Scale(hel_hist_sig->Integral()/hel_hist_bg->Integral());
    hel_hist_bg->SetLineWidth(1);
    hel_hist_bg->SetLineColor(kRed);
    hel_hist_bg->SetMarkerColor(kRed);
    hel_hist_bg->SetMarkerSize(0);
    hel_hist_bg->Draw("HIST SAME");
    hel_hist_sig->Draw("HIST SAME");

    // Draw cut regions
    std::vector<TBox*> boxes;
    boxes.push_back(new TBox(-1, 0, -0.4, y_max));
    boxes.push_back(new TBox(0.4, 0, 1, y_max));
    for (auto box : boxes) {
        box->SetFillColorAlpha(ANABlue, 0.3);
        box->SetLineColor(0);
        box->SetLineWidth(0);
        // box->SetMarkerSize(0);
        // box->SetMarkerStyle(0);
        // box->SetMarkerColor(0);
        box->Draw();
    }
    // gPad->RedrawAxis();

    // Draw legend
    TLegend * leg = new TLegend(0.2, 0.75, 0.5, 0.9);
    leg->AddEntry(hel_hist_sig, "Signal");
    leg->AddEntry(hel_hist_bg, "Background");
    // leg->AddEntry(boxes[0], "Selected region");
    leg->SetBorderSize(1);
    leg->Draw();

    // Save
    canvas->cd();
    canvas->SaveAs("/home/pullen/analysis/B02DKstar/ANA_resources/Plots/Selection/"
            "Preselection/helicity_cut.pdf");
}
