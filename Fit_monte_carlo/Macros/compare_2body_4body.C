void compare_2body_4body() {

    // Loop through types
    std::vector<TString> types = {
        "gamma_010",
        "pi_010",
        "gamma_100",
        "pi_100"
    };
    for (auto type : types) {

        // Open files and get trees
        TFile * file2 = TFile::Open("/data/lhcb/users/pullen/RapidSim/DstarKstar/twoBody/" + type + "_tree.root", "READ");
        TFile * file4 = TFile::Open("/data/lhcb/users/pullen/RapidSim/DstarKstar/"
                "fourBody/" + type + "_tree.root", "READ");
        TTree * tree2 = (TTree*)file2->Get("DecayTree");
        TTree * tree4 = (TTree*)file4->Get("DecayTree");

        // Make histograms
        TH1F * hist2 = new TH1F("hist2", "", 300, 4800, 5400);
        TH1F * hist4 = new TH1F("hist4", "", 300, 4800, 5400);

        // Draw B mass
        tree2->Draw("1000*Bd_M>>hist2");
        tree4->Draw("1000*Bd_M>>hist4");

        // Plot together
        hist2->Scale(1/hist2->Integral());
        hist4->Scale(1/hist4->Integral());
        hist2->SetLineColor(kRed);
        hist4->SetLineColor(kBlue);
        hist2->SetLineWidth(1);
        hist4->SetLineWidth(1);
        TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
        hist4->Draw("HIST");
        hist2->Draw("HIST SAME");
        TLegend * leg = new TLegend(0.7, 0.75, 0.9, 0.9);
        leg->AddEntry(hist2, "2-body", "L");
        leg->AddEntry(hist4, "4-body", "L");
        leg->Draw();
        gPad->RedrawAxis();
        canvas->SaveAs("../Plots/compare_2body_4body_" + type + ".pdf");
    }
}
