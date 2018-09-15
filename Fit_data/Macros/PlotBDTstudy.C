// ==========================================================
// Macro for plotting the results of a BDT study in data (2D)
// ==========================================================
void PlotBDTstudy(TString mode) {

    // Mode to compare with
    TString Kpi = "Kpi";
    TString piK = "piK";
    if (mode == "pipipipi") {
        Kpi = "Kpipipi";
        piK = "piKpipi";
    }

    // Open files
    TChain * tree = new TChain("study_tree");
    tree->Add("../Results/BDT/" + mode + "_" + Kpi + "_*.root");

    // Setup
    double bins = 10;
    double min = -0.05;
    double max = 0.95;

    // Make histograms
    // Status histograms
    TH1F * status_1D = new TH1F("status_1D", "", 6, 0, 5);
    TH2F * status_2D = new TH2F("status_2D", "", bins, min, max, bins, min, max);

    // Significance histograms
    TH2F * sig_mode = new TH2F("sig_mode", "", bins, min, max, bins, min, max);
    TH2F * sig_piK = new TH2F("sig_piK", "", bins, min, max, bins, min, max);
    TH1F * sig_mode_1D = new TH1F("sig_mode_1D", "", bins, min, max);
    TH1F * sig_piK_1D = new TH1F("sig_piK_1D", "", bins, min, max);

    // Error histograms
    TH2F * R_plus = new TH2F("R_plus", "", bins, min, max, bins, min, max);
    TH2F * R_minus = new TH2F("R_minus", "", bins, min, max, bins, min, max);
    TH1F * R_plus_1D = new TH1F("R_plus_1D", "", bins, min, max);
    TH1F * R_minus_1D = new TH1F("R_minus_1D", "", bins, min, max);
    TH2F * R_mode = new TH2F("R_mode", "", bins, min, max, bins, min, max);
    TH2F * A_mode = new TH2F("A_mode", "", bins, min, max, bins, min, max);
    TH1F * R_mode_1D = new TH1F("R_mode_1D", "", bins, min, max);
    TH1F * A_mode_1D = new TH1F("A_mode_1D", "", bins, min, max);

    // String for 2D plot of cuts
    TString cuts = "cut_" + mode + ":cut_" + Kpi;

    // Draw status
    tree->Draw("status >> status_1D", "status < 300");
    tree->Draw("status - 300 >> +status_1D", "status >= 300");
    tree->Draw(cuts + ">> status_2D", "status * (status < 300)");
    tree->Draw(cuts + ">> +status_2D", "(status - 300) * (status >= 300)");

    // Draw significance
    // TString stat_req = "(status <= 2 || (status >= 300 && status <= 302))";
    TString stat_req = "(1 == 1)";
    TString sig_mode_str = "(S_" + mode + "_plus + S_" + mode + "_minus) / sqrt("
        "S_" + mode + "_plus + S_" + mode + "_minus + B_" + mode + "_plus + "
        "B_" + mode + "_minus)";
    TString sig_piK_str = "(S_" + piK + "_plus + S_" + piK + "_minus) / sqrt("
        "S_" + piK + "_plus + S_" + piK + "_minus + B_" + piK + "_plus + "
        "B_" + piK + "_minus)";
    tree->Draw(cuts + ">> sig_mode", sig_mode_str + "*" + stat_req);
    tree->Draw(cuts + ">> sig_piK", sig_piK_str + "*" + stat_req);
    tree->Draw("cut_" + mode + ">> sig_mode_1D", sig_mode_str + 
            "* (cut_" + Kpi + " == 0.5)");
    tree->Draw("cut_" + Kpi + ">> sig_piK_1D", sig_piK_str + 
            "* (cut_" + mode + " == 0.5)");

    // Draw errors of observables
    tree->Draw(cuts + ">> R_plus", "R_plus_signal_blind_" + piK + "*" + stat_req);
    tree->Draw(cuts + ">> R_minus", "R_minus_signal_blind_" + piK + "*" + stat_req);
    tree->Draw("cut_" + Kpi + ">> R_plus_1D", "R_plus_signal_blind_" + piK + 
            "* (cut_" + mode + " == 0.5)");
    tree->Draw("cut_" + Kpi + ">> R_minus_1D", "R_minus_signal_blind_" + piK + 
            "* (cut_" + mode + " == 0.5)");
    tree->Draw(cuts + ">> R_mode", "R_" + mode + "_vs_" + Kpi + "_signal_blind_err"
           + "*"  +stat_req);
    tree->Draw(cuts + ">> A_mode", "A_" + mode + "_signal_blind_err" + "*" 
            + stat_req);
    tree->Draw("cut_" + mode + ">> R_mode_1D", "R_" + mode + "_vs_" + Kpi + 
            "_signal_blind_err * (cut_" + Kpi + " == 0.5)");
    tree->Draw("cut_" + mode + ">> A_mode_1D", "A_" + mode + 
            "_signal_blind_err * (cut_" + Kpi + " == 0.5)");

    // Create and save plots
    TCanvas * canvas = new TCanvas("canvas", "", 500, 500);
    TString dir = "../Plots/BDT/" + mode + "_vs_" + Kpi + "_";

    // 1D status histogram
    status_1D->GetXaxis()->SetTitle("Fit status");
    status_1D->GetYaxis()->SetTitle("Entries");
    status_1D->Draw("HIST");
    canvas->SaveAs(dir + "status_1D.pdf");

    // 2D status histogram
    canvas->Clear();
    status_2D->GetXaxis()->SetTitle("BDT cut " + Kpi + "/" + piK);
    status_2D->GetYaxis()->SetTitle("BDT cut " + mode);
    status_2D->Draw("COLZ");
    canvas->SaveAs(dir + "status_2D.pdf");

    // Significance of mode
    canvas->Clear();
    sig_mode->GetXaxis()->SetTitle("BDT cut " + Kpi + "/" + piK);
    sig_mode->GetYaxis()->SetTitle("BDT cut " + mode);
    sig_mode->Draw("COLZ");
    canvas->SaveAs(dir + "significance_" + mode + ".pdf");

    // Significance of piK
    canvas->Clear();
    sig_piK->GetXaxis()->SetTitle("BDT cut " + Kpi + "/" + piK);
    sig_piK->GetYaxis()->SetTitle("BDT cut " + mode);
    sig_piK->Draw("COLZ");
    canvas->SaveAs(dir + "significance_" + piK + ".pdf");

    // 1D significance of mode
    canvas->Clear();
    sig_mode_1D->GetXaxis()->SetTitle("BDT cut " + mode);
    sig_mode_1D->GetYaxis()->SetTitle("Estimated " + mode + " S/#sqrt{S + B}");
    sig_mode_1D->SetMarkerStyle(2);
    sig_mode_1D->Draw("HIST P L");
    canvas->SaveAs(dir + "significance_" + mode + "_1D.pdf");

    // 1D significance of piK
    canvas->Clear();
    sig_piK_1D->GetXaxis()->SetTitle("BDT cut " + Kpi + "/" + piK);
    sig_piK_1D->GetYaxis()->SetTitle("Estimated " + piK + " S/#sqrt{S + B}");
    sig_piK_1D->SetMarkerStyle(2);
    sig_piK_1D->Draw("HIST P L");
    canvas->SaveAs(dir + "significance_" + piK + "_1D.pdf");

    // Error on R plus
    canvas->Clear();
    R_plus->GetXaxis()->SetTitle("BDT cut " + Kpi + "/" + piK);
    R_plus->GetYaxis()->SetTitle("BDT cut " + mode);
    R_plus->Draw("COLZ");
    canvas->SaveAs(dir + "R_plus_error.pdf");

    // Error on R minus
    canvas->Clear();
    R_minus->GetXaxis()->SetTitle("BDT cut " + Kpi + "/" + piK);
    R_minus->GetYaxis()->SetTitle("BDT cut " + mode);
    R_minus->Draw("COLZ");
    canvas->SaveAs(dir + "R_minus_error.pdf");

    // 1D error on R plus
    canvas->Clear();
    R_plus_1D->GetXaxis()->SetTitle("BDT cut " + Kpi + "/" + piK);
    R_plus_1D->GetYaxis()->SetTitle("R_{+} error");
    R_plus_1D->SetMarkerStyle(2);
    R_plus_1D->Draw("HIST P L");
    canvas->SaveAs(dir + "R_plus_error_1D.pdf");

    // 1D error on R minus
    canvas->Clear();
    R_minus_1D->GetXaxis()->SetTitle("BDT cut " + Kpi + "/" + piK);
    R_minus_1D->GetYaxis()->SetTitle("R_{-} error");
    R_minus_1D->SetMarkerStyle(2);
    R_minus_1D->Draw("HIST P L");
    canvas->SaveAs(dir + "R_minus_error_1D.pdf");

    // Error on ratio for mode
    canvas->Clear();
    R_mode->GetXaxis()->SetTitle("BDT cut " + Kpi + "/" + piK);
    R_mode->GetYaxis()->SetTitle("BDT cut " + mode);
    R_mode->Draw("COLZ");
    canvas->SaveAs(dir + "R_" + mode + "_vs_" + Kpi + "_error.pdf");

    // Error on asymmetry
    canvas->Clear();
    A_mode->GetXaxis()->SetTitle("BDT cut " + Kpi + "/" + piK);
    A_mode->GetYaxis()->SetTitle("BDT cut " + mode);
    A_mode->Draw("COLZ");
    canvas->SaveAs(dir + "A_" + mode + "_error.pdf");

    // 1D error on ratio
    canvas->Clear();
    R_mode_1D->GetXaxis()->SetTitle("BDT cut " + mode);
    R_mode_1D->GetYaxis()->SetTitle("R_{" + mode + "} error");
    R_mode_1D->SetMarkerStyle(2);
    R_mode_1D->Draw("HIST P L");
    canvas->SaveAs(dir + "R_" + mode + "_vs_" + Kpi + "_error_1D.pdf");

    // 1D error on asymmetry
    canvas->Clear();
    A_mode_1D->GetXaxis()->SetTitle("BDT cut " + mode);
    A_mode_1D->GetYaxis()->SetTitle("A_{" + mode + "} error");
    A_mode_1D->SetMarkerStyle(2);
    A_mode_1D->Draw("HIST P L");
    canvas->SaveAs(dir + "A_" + mode + "_error_1D.pdf");

    delete canvas;
}
