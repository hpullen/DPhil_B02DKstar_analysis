// =====================================================
// Plot response curves for training and testing samples
// =====================================================
void BDT_response(bool log = false) {

    std::vector<TString> modes = {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"};
    TCanvas * canvas = new TCanvas("canvas", "", 1000, 1200);
    canvas->Divide(2, 3);
    canvas->SetLogy();
    int count = 1;
    TLegend * leg = new TLegend(0.1, 0.2, 1, 0.8);
    for (TString mode : modes) {
        TString BDT_name = mode + "_run2";
        gROOT->ForceStyle();

        // Get histograms
        TFile * file = TFile::Open("/home/pullen/analysis/B02DKstar/BDT/Outputs/"
                "Classification_" + BDT_name + ".root", "READ");
        TH1D * sig_test = (TH1D*)file->Get("Method_BDT/BDTG_" + BDT_name + "/MVA_BDTG_" 
                + BDT_name + "_S");
        TH1D * bg_test = (TH1D*)file->Get("Method_BDT/BDTG_" + BDT_name + "/MVA_BDTG_" 
                + BDT_name + "_B");
        TH1D * sig_train = (TH1D*)file->Get("Method_BDT/BDTG_" + BDT_name + "/MVA_BDTG_" 
                + BDT_name + "_Train_S");
        TH1D * bg_train = (TH1D*)file->Get("Method_BDT/BDTG_" + BDT_name + "/MVA_BDTG_" 
                + BDT_name + "_Train_B");

        // Name in latex form
        TString latex_mode;
        if (mode == "Kpi") latex_mode = "K#pi";
        else if (mode == "KK") latex_mode = "KK";
        else if (mode == "pipi") latex_mode = "#pi#pi";
        else if (mode == "Kpipipi") latex_mode = "K#pi#pi#pi";
        else if (mode == "pipipipi") latex_mode = "#pi#pi#pi#pi";

        // Format test response
        sig_test->SetFillColor(38);
        sig_test->SetLineColor(kBlue);
        sig_test->SetLineWidth(2);
        bg_test->SetFillColor(kRed);
        bg_test->SetFillStyle(3004);
        bg_test->SetLineColor(kRed);
        bg_test->SetLineWidth(2);

        // Format training response
        sig_train->SetMarkerColor(kBlue);
        sig_train->SetMarkerStyle(8);
        sig_train->SetLineColor(kBlue);
        sig_train->SetLineWidth(1);
        bg_train->SetMarkerColor(kRed);
        bg_train->SetMarkerStyle(8);
        bg_train->SetLineColor(kRed);
        bg_train->SetLineWidth(1);

        // Axis titles
        bg_train->GetXaxis()->SetTitle(latex_mode + " BDT response");
        bg_train->GetXaxis()->SetTitleOffset(1.1);
        if (!log) {
            bg_train->GetYaxis()->SetTitle("(1/N) dN / dx");
        } else {
            bg_train->GetYaxis()->SetTitle("log (1/N) dN / dx");
        }

        // Draw
        TPad * current = (TPad*)canvas->cd(count);
        if (log) {
            current->SetLogy();
        }
        bg_train->Draw("E1");
        sig_test->Draw("HIST SAME");
        bg_test->Draw("HIST SAME");
        sig_train->Draw("E1 SAME");
        bg_train->Draw("E1 SAME");

        // Draw legend
        if (count == 5) {
            leg->AddEntry(sig_test, "Signal (test sample)", "F");
            leg->AddEntry(bg_test, "Background (test sample)", "F");
            leg->AddEntry(sig_train, "Signal (training sample)", "P");
            leg->AddEntry(bg_train, "Background (training sample)", "P");
        }

        gPad->RedrawAxis();
        count++;
    }


    // Save
    canvas->cd(count);
    leg->Draw();    
    if (!log) {
        canvas->SaveAs("/home/pullen/analysis/B02DKstar/ANA_plots/Plots/Selection/"
                "BDT/BDT_response.pdf");
    } else {
        canvas->SaveAs("/home/pullen/analysis/B02DKstar/ANA_plots/Plots/Selection/"
                "BDT/BDT_response_log.pdf");
    }


}
