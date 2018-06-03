void old_vs_new_allvars() {

    // Get trees
    TFile * file_old = TFile::Open("tuples/old.root", "READ");
    TFile * file_new = TFile::Open("tuples/new.root", "READ");
    TTree * tree_old = (TTree*)file_old->Get("DecayTree");
    TTree * tree_new = (TTree*)file_new->Get("DecayTree");
    TObjArray * arr = tree_new->GetListOfBranches();

    // Loop through and plot
    TCanvas * canv = new TCanvas("canv", "", 900, 600);
    for (int i = 0; i < arr->GetEntriesFast(); i++) {

        TString var = arr->At(i)->GetName();
        TString histname = var;
        tree_old->Draw(var + ">>hist_old_" + histname);
        tree_new->Draw(var + ">>hist_new_" + histname);
        TH1F * hist_old = (TH1F*)gDirectory->Get("hist_old_" + histname);
        TH1F * hist_new = (TH1F*)gDirectory->Get("hist_new_" + histname);
        hist_old->SetLineColor(kRed);
        hist_new->SetLineColor(kBlue);
        hist_old->Scale(1/hist_old->Integral());
        hist_new->Scale(1/hist_new->Integral());
        canv->Clear();
        canv->cd();
        hist_old->Draw("HIST");
        hist_new->Draw("HIST SAME");
        TLegend * leg;
        if (var == "Bd_M" || std::string(var).find("TRCHI2") != std::string::npos) {
            leg = new TLegend(0.65, 0.7, 0.85, 0.9);
        } else {
            leg = new TLegend(0.2, 0.7, 0.4, 0.9);
        }
        leg->AddEntry(hist_old, "S24", "L");
        leg->AddEntry(hist_new, "S24r1", "L");
        leg->Draw();
        canv->SaveAs("Plots/" + histname + ".pdf");
    }
}

