// ==========================================================
// Plot uncertainty vs. BDT cut for a particular D decay mode
// ==========================================================
void PlotFromData(TString mode) {

    // Get parameters of interest
    std::vector<TString> params = {};
    if (mode == "Kpi") {
        params = {"R_signal_piK_plus", "R_signal_piK_minus"};
    } else if (mode == "Kpipipi") {
        params = {"R_signal_piKpipi_plus", "R_signal_piKpipi_minus"};
    } else if (mode == "KK") {
        params = {"R_signal_KK_run1", "R_signal_KK_run2", "A_signal_KK_run1", 
            "A_signal_KK_run2"};
    } else if (mode == "pipi") {
        params = {"R_signal_pipi_run1", "R_signal_pipi_run2", "A_signal_pipi_run1", 
            "A_signal_pipi_run2"};
    } else if (mode == "pipipipi") {
        params = {"R_signal_pipipipi_run2", "A_signal_pipipipi_run2"};
    }

    // Get file name
    TString filename = "/data/lhcb/users/pullen/B02DKstar/BDT_studies/"
        "data/" + mode + "_full.root";

    // Previous analysis uncertainties
    double run2_scale = 1/sqrt(1.5);
    double both_scale = 1/sqrt(2.5);
    std::map<TString, double> prev_errs = {
        {"A_signal_KK_run1", 0.145},
        {"R_signal_KK_run1", 0.165},
        {"A_signal_pipi_run1", 0.217},
        {"R_signal_pipi_run1", 0.283},
        {"A_signal_KK_run2", 0.145 * run2_scale},
        {"R_signal_KK_run2", 0.165 * run2_scale},
        {"A_signal_pipi_run2", 0.217 * run2_scale},
        {"R_signal_pipi_run2", 0.283 * run2_scale},
        {"R_signal_piK_plus", 0.029 * both_scale},
        {"R_signal_piK_minus", 0.032 * both_scale}
    };

    // Open file
    TFile * file = TFile::Open(filename, "READ");
    TTree * tree = (TTree*)file->Get("study_tree");

    // Draw each parameter uncertainty vs. cut
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    for (auto par : params) {

        // Draw into histogram
        TString hist_name = "hist_" + mode + "_" + par;
        tree->Draw(par + "_blind_err:cut_" + mode + ">>" + hist_name);

        // Histogram properties
        canvas->Clear();
        TH2F * hist = (TH2F*)gDirectory->Get(hist_name);
        hist->SetMarkerStyle(2);
        hist->GetXaxis()->SetTitle("BDT cut");
        hist->GetYaxis()->SetTitle(par + " uncertainty");
        hist->GetYaxis()->SetTitleOffset(1.25);
        hist->Draw();

        // Make line for previous analysis value
        TLine line(hist->GetXaxis()->GetXmin(), 0, hist->GetXaxis()->GetXmax(), 0);
        line.SetLineStyle(2);
        line.SetLineColor(kRed);
        if (prev_errs.find(par) != prev_errs.end()) {
            line.SetY1(prev_errs[par]);
            line.SetY2(prev_errs[par]);
            line.Draw();
            gPad->RedrawAxis();
        }
        canvas->SaveAs("Plots/Data/" + mode + "_" + par + ".pdf");
    }

    // Plot significance (S/sqrt(S + B))
    TString sig_mode = mode;
    if (mode == "Kpi") sig_mode = "piK";
    else if (mode == "Kpipipi") sig_mode = "piKpipi";
    for (TString run : {"run1", "run2"}) {
        if (run == "run1" && mode == "pipipipi") continue;
        canvas->Clear();
        TString hist_name = "sig_hist_" + run;
        tree->Draw("(sig_" + sig_mode + "_" + run + "_plus + sig_" + 
                sig_mode + "_" + run + "_minus)/2:cut_" + mode + ">>" + hist_name);
        TH2F * sig_hist = (TH2F*)gDirectory->Get(hist_name);
        sig_hist->SetMarkerStyle(2);
        sig_hist->GetXaxis()->SetTitle("BDT cut");
        sig_hist->GetYaxis()->SetTitle("S/#sqrt{S + B}");
        if (mode == "pipipipi") {
            sig_hist->GetYaxis()->SetRangeUser(0, 0.05);
        }
        sig_hist->Draw();
        canvas->SaveAs("Plots/Data/" + mode + "_significance_" + run + ".pdf");
    }

}
