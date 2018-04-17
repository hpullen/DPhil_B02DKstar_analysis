// ========================================================
// Script to plot observables at one point of the BDT study
// ========================================================
void Plot_observables() {

    // Open files
    TChain * tree = new TChain("toy_tree");
    for (int num = 0; num < 10; num++) {
        tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/BDT_cut_optimisation/"
                "toyFit_KK_0.500000Kpi_0.500000Kpipipi_0.500000pipi_0.500000"
                "pipipipi_0.500000" + std::to_string(num) + ".root").c_str());
    }
    std::cout << "Tree has " << tree->GetEntries() << " entries." << std::endl;

    // Draw each observable
    std::vector<TString> obs_list = {
        "R_KK_vs_Kpi_signal",
        "A_KK_signal",
        "R_pipi_vs_Kpi_signal",
        "A_pipi_signal",
        "R_pipipipi_vs_Kpipipi_signal",
        "A_pipipipi_signal",
        "R_plus_signal_piK",
        "R_minus_signal_piK",
        "R_plus_signal_piKpipi",
        "R_minus_signal_piKpipi"
    };
    for (auto obs : obs_list) {
        tree->Draw("fit_shape_final_error_" + obs + ">>hist");
        TH1F * hist = (TH1F*)gDirectory->Get("hist");
        TCanvas * canv = new TCanvas("canvas", "", 900, 600);
        hist->Draw("E");
        canv->SaveAs("../../Plots/" + obs + ".pdf");
        delete canv;
        delete hist;
    }

}
