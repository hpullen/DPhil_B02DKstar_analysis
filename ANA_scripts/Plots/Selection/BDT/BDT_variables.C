// =============================================================
// Script to plot signal vs background for variables used in BDT
// =============================================================
void BDT_variables() {

    // List of variables    
    std::vector<TString> vars = {
        "acos_Bd_DIRA_OWNPV_",
        "Bd_IPCHI2_OWNPV",
        "Bd_LOKI_VFASPF_VCHI2VDOF",
        "Bd_ptasy_1.50",
        "log10_D0_IPCHI2_OWNPV_",
        "log10_KstarK_IPCHI2_OWNPV_",
        "log10_D0K_IPCHI2_OWNPV_",
        "log10_D0Pi_IPCHI2_OWNPV_",
        "log10_KstarPi_IPCHI2_OWNPV_",
        "log10_D0K_PT_",
        "log10_D0Pi_PT_",
        "log10_KstarK_PT_",
        "log10_KstarPi_PT_",
    };

    // Open file
    TFile * file = TFile::Open("/home/pullen/analysis/B02DKstar/BDT/Outputs/"
            "Classification_Kpi_run2.root", "READ");

    // Loop through vars and create histograms
    gROOT->ForceStyle();
    TCanvas * canvas = new TCanvas("canvas", "", 700, 1000);
    canvas->Divide(3, 5);
    int count = 1;
    for (TString var : vars) {
        canvas->cd(count);
        TString full_var = "Method_BDT/BDTG_Kpi_run2/" + var;
        TString var_sig = full_var + "__Signal";
        TString var_bg = full_var + "__Background";
        TH1F * hist_signal = (TH1F*)file->Get(var_sig);
        TH1F * hist_bg = (TH1F*)file->Get(var_bg);
        hist_signal->Scale(1/hist_signal->Integral());
        hist_bg->Scale(1/hist_bg->Integral());
        hist_signal->SetFillColor(38);
        hist_signal->SetLineColor(kBlue);
        hist_signal->SetLineWidth(2);
        hist_signal->GetYaxis()->SetTitleOffset(1.3);
        hist_bg->GetYaxis()->SetTitleOffset(1.3);
        hist_signal->GetXaxis()->SetTitleOffset(1.1);
        hist_bg->GetXaxis()->SetTitleOffset(1.1);
        hist_bg->SetFillColor(kRed);
        gStyle->SetHatchesSpacing(1);
        hist_bg->SetFillStyle(3004);
        hist_bg->SetLineColor(kRed);
        hist_bg->SetLineWidth(2);
        if (hist_signal->GetMaximum() > hist_bg->GetMaximum()) {
            hist_signal->Draw("HIST");
            hist_bg->Draw("HIST SAME");
        } else {
            hist_bg->Draw("HIST");
            hist_signal->Draw("HIST SAME");
            hist_bg->Draw("HIST SAME");
        }
        gPad->RedrawAxis();

        // Add legend to first plot
        if (count == 1) {
            TLegend * leg = new TLegend(0.4, 0.7, 0.85, 0.87);
            leg->AddEntry(hist_signal, "Signal", "f");
            leg->AddEntry(hist_bg, "Background", "f");
            leg->Draw();
        }

        count++;
    }

    // Save
    canvas->SaveAs("/home/pullen/analysis/B02DKstar/ANA_plots/Plots/Selection/BDT/"
            "BDT_variables.pdf");
}

