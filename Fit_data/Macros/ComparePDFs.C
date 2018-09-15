// ============================================
// Compare PDFs from combined fit and split fit
// ============================================
void ComparePDFs() {

    // Open both histograms
    gROOT->ForceStyle();
    TFile * file_comb = TFile::Open("../Histograms/twoAndFourBody_data.root", 
            "READ");
    TFile * file_split = TFile::Open("../Histograms/twoAndFourBody_data_split.root", 
            "READ");
    TH1F * hist_comb = (TH1F*)file_comb->Get("fit_Kpi_run2");
    TH1F * hist_split = (TH1F*)file_split->Get("fit_Kpi_run2_plus");
    hist_split->Scale(hist_comb->Integral()/hist_split->Integral());

    // Plot
    TCanvas * canvas = new TCanvas("canvas", "", 900, 1200);
    canvas->Divide(1, 2);
    canvas->cd(1);
    hist_comb->SetLineColor(ANABlue);
    hist_comb->Draw("C");
    hist_split->SetLineColor(kRed + 2);
    hist_split->Draw("C SAME");

    // Make difference histogram
    TH1F * diff = new TH1F(*hist_comb);
    diff->Add(hist_split, -1);

    // Plot
    canvas->cd(2);
    diff->Draw("HIST");
    canvas->SaveAs("../Plots/compare_combined_split.pdf");

}
