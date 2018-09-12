// ====================================================
// Plot ROC curve for previous vs current variable list
// ====================================================
void vars_vs_previous() {

    gROOT->ForceStyle();

    // Open file and get histograms
    TFile * file = TFile::Open("/home/pullen/analysis/B02DKstar/BDT/Outputs/"
            "Classification_Kpi_run2.root", "READ");
    TH1F * roc = (TH1F*)file->Get("Method_BDT/BDTG_Kpi_run2/"
            "MVA_BDTG_Kpi_run2_effBvsS");
    TFile * file_previous = TFile::Open("/home/pullen/analysis/tuple_scripts/"
            "analysis_code/multivariate_analysis/Outputs/"
            "Classification_Kpi_run2_previous.root", "READ");
    TH1F * roc_previous = (TH1F*)file_previous->Get("Method_BDT/"
            "BDTG_Kpi_run2_previous/MVA_BDTG_Kpi_run2_previous_effBvsS");

    // Draw
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    roc_previous->SetLineColor(ANABlue);
    roc_previous->SetLineWidth(2);
    roc_previous->GetXaxis()->SetTitle("Signal efficiency");
    roc_previous->GetYaxis()->SetTitle("Background efficiency");
    roc->SetLineColor(kRed + 2);
    roc->SetLineWidth(2);
    roc_previous->Draw("C");
    roc->Draw("C SAME");
    TLegend * leg = new TLegend(0.2, 0.6, 0.5, 0.85);
    leg->AddEntry(roc_previous, "Previous analysis", "L");
    leg->AddEntry(roc, "Current analysis", "L");
    leg->Draw();
    canvas->SaveAs("/home/pullen/analysis/B02DKstar/ANA_resources/Plots/Selection/BDT/"
            "vars_vs_previous.pdf");
}
