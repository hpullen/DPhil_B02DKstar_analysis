void plotDKpipiComponents() {

    // Open files
    TFile * file = TFile::Open("../Histograms/DKpipi_Kpi.root", "READ");
    TFile * file_1a = TFile::Open("../Histograms/DKpipi_1a_Kpi.root", "READ");
    TFile * file_2 = TFile::Open("../Histograms/DKpipi_2_Kpi.root", "READ");
    TFile * file_3 = TFile::Open("../Histograms/DKpipi_3_Kpi.root", "READ");
    TFile * file_5a = TFile::Open("../Histograms/DKpipi_5a_Kpi.root", "READ");

    // Get histograms
    TH1F * hist = (TH1F*)file->Get("data");
    TH1F * fit_hist = (TH1F*)file->Get("fit");
    TH1F * hist_1a = (TH1F*)file_1a->Get("data");
    TH1F * h_fit_1a = (TH1F*)file_1a->Get("fit");
    TH1F * hist_2 = (TH1F*)file_2->Get("data");
    TH1F * h_fit_2 = (TH1F*)file_2->Get("fit");
    TH1F * hist_3 = (TH1F*)file_3->Get("data");
    TH1F * h_fit_3 = (TH1F*)file_3->Get("fit");
    TH1F * hist_5a = (TH1F*)file_5a->Get("data");
    TH1F * h_fit_5a = (TH1F*)file_5a->Get("fit");

    // Make nice plot of all components
    // Formatting
    hist->SetStats(kFALSE);
    hist->GetXaxis()->SetRangeUser(4000, 6500);
    hist->GetXaxis()->SetTitle("B^{0} mass / MeV");
    hist->GetYaxis()->SetTitle("Candidates");
    hist->SetLineWidth(1);
    fit_hist->SetLineColor(kBlack);
    fit_hist->SetStats(kFALSE);
    hist_1a->SetLineColor(kRed);
    hist_1a->SetLineWidth(1);
    hist_1a->SetStats(kFALSE);
    h_fit_1a->SetLineColor(kRed);
    h_fit_1a->SetStats(kFALSE);
    hist_2->SetLineColor(kBlue);
    hist_2->SetLineWidth(1);
    hist_2->SetStats(kFALSE);
    h_fit_2->SetLineColor(kBlue);
    h_fit_2->SetStats(kFALSE);
    hist_3->SetLineColor(kOrange);
    hist_3->SetLineWidth(1);
    hist_3->SetStats(kFALSE);
    h_fit_3->SetLineColor(kOrange);
    h_fit_3->SetStats(kFALSE);
    hist_5a->SetLineColor(kCyan);
    hist_5a->SetLineWidth(1);
    hist_5a->SetStats(kFALSE);
    h_fit_5a->SetLineColor(kCyan);
    h_fit_5a->SetStats(kFALSE);

    // Plot
    TCanvas * canv = new TCanvas("canv", "", 1000, 700 * 0.8);
    TPad * pad1 = new TPad("pad1", "", 0, 0, 0.7, 1);
    pad1->cd();
    gStyle->SetOptStat(0);
    gROOT->ForceStyle();
    hist->Draw();
    hist_1a->Draw("SAME");
    h_fit_1a->Draw("C SAME");
    hist_2->Draw("SAME");
    h_fit_2->Draw("C SAME");
    hist_3->Draw("SAME");
    h_fit_3->Draw("C SAME");
    hist_5a->Draw("SAME");
    h_fit_5a->Draw("C SAME");
    hist->Draw("SAME");
    fit_hist->Draw("C SAME");

    // Make legend
    TPad * pad2 = new TPad("pad2", "", 0.7, 0, 1, 1);
    TLegend * leg = new TLegend(0, 0.3, 1, 0.8);
    leg->AddEntry(fit_hist, "Overall shape");
    leg->AddEntry(h_fit_1a, "B^{+} #rightarrow (K^{*}(1680)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}");
    leg->AddEntry(h_fit_2, "B^{+} #rightarrow K^{*0} #bar{D}^{0} #pi^{+}");
    leg->AddEntry(h_fit_3, "B^{+} #rightarrow (K_{1}(1400)^{+} #rightarrow K^{*0} "
            "#pi^{+}) #bar{D}^{0}");
    leg->AddEntry(h_fit_5a, "B^{+} #rightarrow (K^{*}(1410)^{+} #rightarrow "
            "K^{*0}#pi^{+}) #bar{D}^{0}");
    pad2->cd();
    leg->Draw();
    canv->cd();
    pad1->Draw();
    pad2->Draw();
    canv->SaveAs("../Plots/DKpipi_fit_components.pdf");

}
