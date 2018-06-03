void old_vs_new() {

    // Get trees
    TFile * file_old = TFile::Open("tuples/old.root", "READ");
    TFile * file_new = TFile::Open("tuples/new.root", "READ");
    TTree * tree_old = (TTree*)file_old->Get("DecayTree");
    TTree * tree_new = (TTree*)file_new->Get("DecayTree");

    // Make histograms
    TH1F * hist_old = new TH1F("hist_old", "", 1000, 0, 3000e6);
    TH1F * hist_new = new TH1F("hist_new", "", 1000, 0, 3000e6);

    // Draw
    tree_old->Draw("eventNumber>>hist_old");
    tree_new->Draw("eventNumber>>hist_new");
    hist_old->SetLineColor(kRed);
    hist_new->SetLineColor(kBlue);
    TCanvas * canv = new TCanvas("canv", "", 900, 600);
    hist_old->Draw("HIST");
    hist_new->Draw("HIST SAME");
    canv->SaveAs("~/eventNumber.pdf");

    // Other variables
    std::vector<TString> vars = {
        "Bd_MAXDOCA",
        "Bd_PT",
        "Bd_LOKI_VFASPF_VCHI2VDOF",
        "Bd_MIPCHI2_PV",
        "Bd_TAU",
        "Bd_DIRA_OWNPV",
        "Bd_M",
        "D0_PT",
        "D0_MAXDOCA",
        "D0_LOKI_VFASPF_VCHI2VDOF",
        "(D0_ENDVERTEX_Z - PVZ) / sqrt(pow(D0_ENDVERTEX_ZERR, 2) + pow(PVZERR, 2))",
        "D0_DIRA_OWNPV",
        "D0_M",
        "Kstar_P",
        "Kstar_PT",
        "Kstar_MAXDOCA",
        "Kstar_LOKI_VFASPF_VCHI2VDOF",
        "(Kstar_ENDVERTEX_Z - PVZ) / sqrt(pow(Kstar_ENDVERTEX_ZERR, 2) + pow(PVZERR, 2))",
        "Kstar_DIRA_OWNPV",
        "Kstar_M"
    };

    // Add charged track variables
    for (TString track : {"D0K", "D0Pi", "KstarK", "KstarPi"}) {
        vars.push_back(track + "_PT");
        vars.push_back(track + "_PT");
        vars.push_back(track + "_TRCHI2DOF");
        vars.push_back(track + "_MIPCHI2_PV");
    }

    // Loop through and plot
    for (auto var : vars) {

        TString histname = var;
        if (std::string(var).find("D0_ENDVERTEX") != std::string::npos) {
            histname = "D0_PVSIG";
        } else if (std::string(var).find("Kstar_ENDVERTEX") != std::string::npos) {
            histname = "Kstar_PVSIG";
        }
        tree_old->Draw(var + ">>hist_old_" + histname);
        tree_new->Draw(var + ">>hist_new_" + histname);
        TH1F * hist_old = (TH1F*)gDirectory->Get("hist_old_" + histname);
        TH1F * hist_new = (TH1F*)gDirectory->Get("hist_new_" + histname);
        hist_old->SetLineColor(kRed);
        hist_new->SetLineColor(kBlue);
        canv->Clear();
        canv->cd();
        hist_old->Draw("HIST");
        hist_new->Draw("HIST SAME");
        canv->SaveAs("Plots/" + histname + ".pdf");
    }
}

