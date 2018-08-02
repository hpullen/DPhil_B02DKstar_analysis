// ===========================
// Plot S/sqrt(S+B) for a mode
// ===========================
void PlotBDTstudy_new(TString mode) {

    // Open file
    TString file_mode = mode;
    if (mode == "piK") file_mode = "Kpi";
    else if (mode == "piKpipi") file_mode = "Kpipipi";
    TFile * file = TFile::Open("../Results/BDT/" + file_mode + ".root", "READ");
    TTree * tree = (TTree*)file->Get("study_tree");
    std::cout << "Fits with status 0: " << tree->GetEntries("status == 0") 
        << std::endl;

    // Plot S/sqrt(S+B) vs. cut
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    TString cut_name = "cut_" + file_mode;
    for (TString run : {"_run1", "_run2"}) {
        if (mode == "pipipipi" && run == "_run1") continue;
        TString signal_name = "S_" + mode + run + "_plus + S_" + mode + run 
            + "_minus";
        TString bg_name = "B_" + mode + run + "_plus + B_" + mode + run + "_minus";
        TString significance_name = "(" + signal_name + ") / sqrt(" + signal_name
            + " + " + bg_name + ")";
        tree->Draw(significance_name + ":" + cut_name + ">>hist" + run);
        TH2F * hist = (TH2F*)gDirectory->Get("hist" + run);
        hist->SetMarkerStyle(2);
        hist->GetXaxis()->SetTitle("BDT cut");
        hist->GetYaxis()->SetTitle("S/#sqrt{S + B}");
        hist->Draw();
        canvas->SaveAs("../Plots/BDT/significance_" + mode + run + ".pdf");
        canvas->Clear();
    }
}
