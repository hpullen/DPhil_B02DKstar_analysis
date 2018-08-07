void KK_study() {
    // 1D KK cut study
    gROOT->ForceStyle();
    TGraph * A_KK_graph = new TGraph();
    TGraph * R_KK_graph = new TGraph();
    int count = 0;

    // Loop through cuts
    for (double cut = 0.0; cut < 0.99999; cut += 0.1) {

        // Open files for this cut
        TChain * tree = new TChain("toy_tree");
        std::cout << "cut: " << std::to_string(cut) << std::endl;
        for (int num = 0; num < 10; num++) {
            tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/BDT_cut_optimisation/"
                    "toyFit_KK_" + std::to_string(cut) + "Kpi_0.500000Kpipipi_0."
                    "500000pipi_0.500000pipipipi_0.500000" + std::to_string(num) 
                    + ".root").c_str());
        }
        std::cout << "Tree has " << tree->GetEntries() << " entries" << std::endl;

        // Get RMS of each
        tree->Draw(("fit_shape_final_error_R_KK_vs_Kpi_signal / fit_shape_final_value_R_KK_vs_Kpi_signal >> hist_R_KK_" 
                    + std::to_string(cut)).c_str(), "status ==0");
        TH1F *R_KK_hist = (TH1F*)gDirectory->Get(("hist_R_KK_" 
                    + std::to_string(cut)).c_str());
        // double R_KK = R_KK_hist->GetMean();
        R_KK_hist->Fit("gaus");
        double R_KK = ((TF1*)R_KK_hist->GetFunction("gaus"))->GetParameter("Mean");
        tree->Draw(("fit_shape_final_error_A_KK_signal >> hist_A_KK_" 
                    + std::to_string(cut)).c_str(), "status ==0");
        TH1F *A_KK_hist = (TH1F*)gDirectory->Get(("hist_A_KK_" 
                    + std::to_string(cut)).c_str());
        // double A_KK = A_KK_hist->GetMean();
        A_KK_hist->Fit("gaus");
        double A_KK = ((TF1*)A_KK_hist->GetFunction("gaus"))->GetParameter("Mean");
        std::cout << "A_KK: " << A_KK << std::endl;

        // Fill graphs
        A_KK_graph->SetPoint(count, cut, A_KK);
        R_KK_graph->SetPoint(count, cut, R_KK);

        count++;
    }


    // Plot graphs
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    A_KK_graph->GetXaxis()->SetTitle("KK BDT cut");
    A_KK_graph->GetYaxis()->SetTitle("A_{KK} error");
    A_KK_graph->Draw();
    canv->SaveAs("../../Plots/A_KK_error.pdf");
    canv->Clear();
    R_KK_graph->GetXaxis()->SetTitle("KK BDT cut");
    R_KK_graph->GetYaxis()->SetTitle("R_{KK} error");
    R_KK_graph->Draw();
    canv->SaveAs("../../Plots/R_KK_error_frac.pdf");
}

void KK_2D_study() {
    // 2D KK cut study
    gROOT->ForceStyle();
    TH2F * A_KK_graph = new TH2F("A_KK", "", 10, -0.05, 0.95, 10, -0.05, 0.95);
    TH2F * R_KK_graph = new TH2F("R_KK", "", 10, -0.05, 0.95, 10, -0.05, 0.95);
    int count = 0;

    // Loop through cuts
    for (double cut = 0.0; cut < 0.99999; cut += 0.1) {

        for (double cut2 = 0.0; cut2 < 0.99999; cut2 += 0.1) {

            // Open files for this cut
            TChain * tree = new TChain("toy_tree");
            std::cout << "cut: " << std::to_string(cut) << std::endl;
            for (int num = 0; num < 10; num++) {
                tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/BDT_cut_optimisation/"
                            "toyFit_KK_" + std::to_string(cut) + "Kpi_" + std::to_string(cut2) + "Kpipipi_0."
                            "500000pipi_0.500000pipipipi_0.500000" + std::to_string(num) 
                            + ".root").c_str());
            }
            std::cout << "Tree has " << tree->GetEntries() << " entries" << std::endl;

            // Get RMS of each
            tree->Draw(("fit_shape_final_error_R_KK_vs_Kpi_signal / fit_shape_final_value_R_KK_vs_Kpi_signal >> hist_R_KK_" 
                        + std::to_string(cut)).c_str(), "status ==0");
            TH1F *R_KK_hist = (TH1F*)gDirectory->Get(("hist_R_KK_" 
                        + std::to_string(cut)).c_str());
            // double R_KK = R_KK_hist->GetMean();
            R_KK_hist->Fit("gaus");
            double R_KK = ((TF1*)R_KK_hist->GetFunction("gaus"))->GetParameter("Mean");
            tree->Draw(("fit_shape_final_error_A_KK_signal >> hist_A_KK_" 
                        + std::to_string(cut)).c_str(), "status ==0");
            TH1F *A_KK_hist = (TH1F*)gDirectory->Get(("hist_A_KK_" 
                        + std::to_string(cut)).c_str());
            // double A_KK = A_KK_hist->GetMean();
            A_KK_hist->Fit("gaus");
            double A_KK = ((TF1*)A_KK_hist->GetFunction("gaus"))->GetParameter("Mean");
            std::cout << "A_KK: " << A_KK << std::endl;

            // Fill graphs
            A_KK_graph->Fill(cut2, cut, A_KK);
            R_KK_graph->Fill(cut2, cut, R_KK);

            count++;
        }
    }
    // remove outliers
    A_KK_graph->SetBinContent(3,1,0);
    A_KK_graph->SetBinContent(3,5,0);

    // Plot graphs
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    A_KK_graph->GetXaxis()->SetTitle("ADS BDT cut");
    A_KK_graph->GetYaxis()->SetTitle("KK BDT cut");
    A_KK_graph->GetZaxis()->SetRangeUser(0.08, 0.095);
    A_KK_graph->Draw("COLZ2");
    canv->SaveAs("../../Plots/A_KK_error_2D.pdf");
    canv->Clear();
    R_KK_graph->GetXaxis()->SetTitle("ADS BDT cut");
    R_KK_graph->GetYaxis()->SetTitle("KK BDT cut");
    R_KK_graph->Draw("COLZ");
    canv->SaveAs("../../Plots/R_KK_error_frac_2D.pdf");
}

void pipi_2D_study() {
    // 2D pipi cut study
    gROOT->ForceStyle();
    TH2F * A_pipi_graph = new TH2F("A_pipi", "", 10, -0.05, 0.95, 10, -0.05, 0.95);
    TH2F * R_pipi_graph = new TH2F("R_pipi", "", 10, -0.05, 0.95, 10, -0.05, 0.95);
    int count = 0;

    // Loop through cuts
    for (double cut = 0.0; cut < 0.99999; cut += 0.1) {

        for (double cut2 = 0.0; cut2 < 0.99999; cut2 += 0.1) {

            // Open files for this cut
            TChain * tree = new TChain("toy_tree");
            std::cout << "cut: " << std::to_string(cut) << std::endl;
            for (int num = 0; num < 10; num++) {
                tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/BDT_cut_optimisation/"
                            "toyFit_KK_0.500000Kpi_" + std::to_string(cut2) + "Kpipipi_0."
                            "500000pipi_" + std::to_string(cut) + "pipipipi_0.500000" + std::to_string(num) 
                            + ".root").c_str());
            }
            std::cout << "Tree has " << tree->GetEntries() << " entries" << std::endl;

            // Get RMS of each
            tree->Draw(("fit_shape_final_error_R_pipi_vs_Kpi_signal / fit_shape_final_value_R_pipi_vs_Kpi_signal >> hist_R_pipi_" 
                        + std::to_string(cut)).c_str(), "status ==0");
            TH1F *R_pipi_hist = (TH1F*)gDirectory->Get(("hist_R_pipi_" 
                        + std::to_string(cut)).c_str());
            // double R_pipi = R_pipi_hist->GetMean();
            R_pipi_hist->Fit("gaus");
            double R_pipi = ((TF1*)R_pipi_hist->GetFunction("gaus"))->GetParameter("Mean");
            tree->Draw(("fit_shape_final_error_A_pipi_signal >> hist_A_pipi_" 
                        + std::to_string(cut)).c_str(), "status ==0");
            TH1F *A_pipi_hist = (TH1F*)gDirectory->Get(("hist_A_pipi_" 
                        + std::to_string(cut)).c_str());
            // double A_pipi = A_pipi_hist->GetMean();
            A_pipi_hist->Fit("gaus");
            double A_pipi = ((TF1*)A_pipi_hist->GetFunction("gaus"))->GetParameter("Mean");
            std::cout << "A_pipi: " << A_pipi << std::endl;

            // Fill graphs
            A_pipi_graph->Fill(cut2, cut, A_pipi);
            R_pipi_graph->Fill(cut2, cut, R_pipi);

            count++;
        }
    }

    // Plot graphs
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    A_pipi_graph->GetXaxis()->SetTitle("ADS BDT cut");
    A_pipi_graph->GetYaxis()->SetTitle("pipi BDT cut");
    // A_pipi_graph->GetZaxis()->SetRangeUser(0.075, 0.095);
    A_pipi_graph->Draw("COLZ2");
    canv->SaveAs("../../Plots/A_pipi_error_2D.pdf");
    canv->Clear();
    R_pipi_graph->GetXaxis()->SetTitle("ADS BDT cut");
    R_pipi_graph->GetYaxis()->SetTitle("pipi BDT cut");
    R_pipi_graph->Draw("COLZ");
    canv->SaveAs("../../Plots/R_pipi_error_frac_2D.pdf");
}


void pipipipi_2D_study() {
    // 2D pipipipi cut study
    gROOT->ForceStyle();
    TH2F * A_pipipipi_graph = new TH2F("A_pipipipi", "", 10, -0.05, 0.95, 10, -0.05, 0.95);
    TH2F * R_pipipipi_graph = new TH2F("R_pipipipi", "", 10, -0.05, 0.95, 10, -0.05, 0.95);
    int count = 0;

    // Loop through cuts
    for (double cut = 0.0; cut < 0.99999; cut += 0.1) {

        for (double cut2 = 0.0; cut2 < 0.99999; cut2 += 0.1) {

            // Open files for this cut
            TChain * tree = new TChain("toy_tree");
            std::cout << "cut: " << std::to_string(cut) << std::endl;
            for (int num = 0; num < 10; num++) {
                tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/BDT_cut_optimisation/"
                            "toyFit_KK_0.500000Kpi_0.500000Kpipipi_" + std::to_string(cut2) +
                            "pipi_0.500000pipipipi_" + std::to_string(cut) + std::to_string(num) 
                            + ".root").c_str());
            }
            std::cout << "Tree has " << tree->GetEntries() << " entries" << std::endl;

            // Get RMS of each
            tree->Draw(("fit_shape_final_error_R_pipipipi_vs_Kpipipi_signal / fit_shape_final_value_R_pipipipi_vs_Kpipipi_signal >> hist_R_pipipipi_" 
                        + std::to_string(cut)).c_str(), "status ==0");
            TH1F *R_pipipipi_hist = (TH1F*)gDirectory->Get(("hist_R_pipipipi_" 
                        + std::to_string(cut)).c_str());
            // double R_pipipipi = R_pipipipi_hist->GetMean();
            R_pipipipi_hist->Fit("gaus");
            double R_pipipipi = ((TF1*)R_pipipipi_hist->GetFunction("gaus"))->GetParameter("Mean");
            tree->Draw(("fit_shape_final_error_A_pipipipi_signal >> hist_A_pipipipi_" 
                        + std::to_string(cut)).c_str(), "status ==0");
            TH1F *A_pipipipi_hist = (TH1F*)gDirectory->Get(("hist_A_pipipipi_" 
                        + std::to_string(cut)).c_str());
            // double A_pipipipi = A_pipipipi_hist->GetMean();
            A_pipipipi_hist->Fit("gaus");
            double A_pipipipi = ((TF1*)A_pipipipi_hist->GetFunction("gaus"))->GetParameter("Mean");
            std::cout << "A_pipipipi: " << A_pipipipi << std::endl;

            // Fill graphs
            A_pipipipi_graph->Fill(cut2, cut, A_pipipipi);
            R_pipipipi_graph->Fill(cut2, cut, R_pipipipi);

            count++;
        }
    }
    R_pipipipi_graph->SetBinContent(1, 10, 0);
    R_pipipipi_graph->GetZaxis()->SetRangeUser(0.19, 0.22);

    // Plot graphs
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    A_pipipipi_graph->GetXaxis()->SetTitle("ADS BDT cut");
    A_pipipipi_graph->GetYaxis()->SetTitle("pipipipi BDT cut");
    // A_pipipipi_graph->GetZaxis()->SetRangeUser(0.075, 0.095);
    A_pipipipi_graph->Draw("COLZ2");
    canv->SaveAs("../../Plots/A_pipipipi_error_2D.pdf");
    canv->Clear();
    R_pipipipi_graph->GetXaxis()->SetTitle("ADS BDT cut");
    R_pipipipi_graph->GetYaxis()->SetTitle("pipipipi BDT cut");
    R_pipipipi_graph->Draw("COLZ");
    canv->SaveAs("../../Plots/R_pipipipi_error_frac_2D.pdf");
}

void pipi_study() {
    // 1D pipi cut study
    gROOT->ForceStyle();
    TGraph * A_pipi_graph = new TGraph();
    TGraph * R_pipi_graph = new TGraph();
    int count = 0;

    // Loop through cuts
    for (double cut = 0.0; cut < 0.99999; cut += 0.1) {

        // Open files for this cut
        TChain * tree = new TChain("toy_tree");
        std::cout << "cut: " << std::to_string(cut) << std::endl;
        for (int num = 0; num < 10; num++) {
            tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/BDT_cut_optimisation/"
                    "toyFit_KK_0.500000Kpi_0.500000Kpipipi_0."
                    "500000pipi_" + std::to_string(cut) + "pipipipi_0.500000" + std::to_string(num) 
                    + ".root").c_str());
        }
        std::cout << "Tree has " << tree->GetEntries() << " entries" << std::endl;

        // Get RMS of each
        tree->Draw(("fit_shape_final_error_R_pipi_vs_Kpi_signal / fit_shape_final_value_R_pipi_vs_Kpi_signal >> hist_R_pipi_" 
                    + std::to_string(cut)).c_str(), "status ==0");
        TH1F *R_pipi_hist = (TH1F*)gDirectory->Get(("hist_R_pipi_" 
                    + std::to_string(cut)).c_str());
        // double R_pipi = R_pipi_hist->GetMean();
        R_pipi_hist->Fit("gaus");
        double R_pipi = ((TF1*)R_pipi_hist->GetFunction("gaus"))->GetParameter("Mean");
        tree->Draw(("fit_shape_final_error_A_pipi_signal >> hist_A_pipi_" 
                    + std::to_string(cut)).c_str(), "status ==0");
        TH1F *A_pipi_hist = (TH1F*)gDirectory->Get(("hist_A_pipi_" 
                    + std::to_string(cut)).c_str());
        // double A_pipi = A_pipi_hist->GetMean();
        A_pipi_hist->Fit("gaus");
        double A_pipi = ((TF1*)A_pipi_hist->GetFunction("gaus"))->GetParameter("Mean");
        std::cout << "A_pipi: " << A_pipi << std::endl;

        // Fill graphs
        A_pipi_graph->SetPoint(count, cut, A_pipi);
        R_pipi_graph->SetPoint(count, cut, R_pipi);

        count++;
    }

    // Plot graphs
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    A_pipi_graph->GetXaxis()->SetTitle("#pi#pi BDT cut");
    A_pipi_graph->GetYaxis()->SetTitle("A_{#pi#pi} error");
    A_pipi_graph->Draw();
    canv->SaveAs("../../Plots/A_pipi_error.pdf");
    canv->Clear();
    R_pipi_graph->GetXaxis()->SetTitle("#pi#pi BDT cut");
    R_pipi_graph->GetYaxis()->SetTitle("R_{#pi#pi} error");
    R_pipi_graph->Draw();
    canv->SaveAs("../../Plots/R_pipi_error_frac.pdf");

}


void pipipipi_study() {
    // 1D pipipipi cut study
    gROOT->ForceStyle();
    TGraph * A_pipipipi_graph = new TGraph();
    TGraph * R_pipipipi_graph = new TGraph();
    int count = 0;

    // Loop through cuts
    for (double cut = 0.0; cut < 0.99999; cut += 0.1) {

        // Open files for this cut
        TChain * tree = new TChain("toy_tree");
        std::cout << "cut: " << std::to_string(cut) << std::endl;
        for (int num = 0; num < 10; num++) {
            tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/BDT_cut_optimisation/"
                    "toyFit_KK_0.500000Kpi_0.500000Kpipipi_0."
                    "500000pipi_0.500000pipipipi_" + std::to_string(cut) + std::to_string(num) 
                    + ".root").c_str());
        }
        std::cout << "Tree has " << tree->GetEntries() << " entries" << std::endl;

        // Get RMS of each
        tree->Draw(("fit_shape_final_error_R_pipipipi_vs_Kpipipi_signal / fit_shape_final_value_R_pipipipi_vs_Kpipipi_signal >> hist_R_pipipipi_" 
                    + std::to_string(cut)).c_str(), "status==0");
        TH1F *R_pipipipi_hist = (TH1F*)gDirectory->Get(("hist_R_pipipipi_" 
                    + std::to_string(cut)).c_str());
        // double R_pipipipi = R_pipipipi_hist->GetMean();
        R_pipipipi_hist->Fit("gaus");
        double R_pipipipi = ((TF1*)R_pipipipi_hist->GetFunction("gaus"))->GetParameter("Mean");
        tree->Draw(("fit_shape_final_error_A_pipipipi_signal >> hist_A_pipipipi_" 
                    + std::to_string(cut)).c_str(), "status==0");
        TH1F *A_pipipipi_hist = (TH1F*)gDirectory->Get(("hist_A_pipipipi_" 
                    + std::to_string(cut)).c_str());
        // double A_pipipipi = A_pipipipi_hist->GetMean();
        A_pipipipi_hist->Fit("gaus");
        double A_pipipipi = ((TF1*)A_pipipipi_hist->GetFunction("gaus"))->GetParameter("Mean");
        std::cout << "A_pipipipi: " << A_pipipipi << std::endl;

        // Fill graphs
        A_pipipipi_graph->SetPoint(count, cut, A_pipipipi);
        R_pipipipi_graph->SetPoint(count, cut, R_pipipipi);

        count++;
    }

    // Plot graphs
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    A_pipipipi_graph->GetXaxis()->SetTitle("#pi#pi#pi#pi BDT cut");
    A_pipipipi_graph->GetYaxis()->SetTitle("A_{#pi#pi#pi#pi} error");
    A_pipipipi_graph->Draw();
    canv->SaveAs("../../Plots/A_pipipipi_error.pdf");
    canv->Clear();
    R_pipipipi_graph->GetXaxis()->SetTitle("#pi#pi#pi#pi BDT cut");
    R_pipipipi_graph->GetYaxis()->SetTitle("R_{#pi#pi#pi#pi} error");
    R_pipipipi_graph->Draw();
    canv->SaveAs("../../Plots/R_pipipipi_error_frac.pdf");

}

void piK_study() {
    // 1D pipipipi cut study
    gROOT->ForceStyle();
    TGraph * R_plus_graph = new TGraph();
    TGraph * R_minus_graph = new TGraph();
    int count = 0;

    // Loop through cuts
    for (double cut = 0.1; cut < 0.99999; cut += 0.1) {

        // Open files for this cut
        TChain * tree = new TChain("toy_tree");
        std::cout << "cut: " << std::to_string(cut) << std::endl;
        for (int num = 0; num < 10; num++) {
            tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/BDT_cut_optimisation/"
                    "toyFit_KK_0.500000Kpi_" + std::to_string(cut) + "Kpipipi_0."
                    "500000pipi_0.500000pipipipi_0.500000" + std::to_string(num) 
                    + ".root").c_str());
        }
        std::cout << "Tree has " << tree->GetEntries() << " entries" << std::endl;

        // Get RMS of each
        tree->Draw(("fit_shape_final_error_R_plus_signal_piK >> hist_R_plus_" 
                    + std::to_string(cut)).c_str(), "status==0");
        TH1F *R_plus_hist = (TH1F*)gDirectory->Get(("hist_R_plus_" 
                    + std::to_string(cut)).c_str());
        R_plus_hist->Fit("gaus");
        double R_plus = ((TF1*)R_plus_hist->GetFunction("gaus"))->GetParameter("Mean");
        tree->Draw(("fit_shape_final_error_R_minus_signal_piK >> hist_R_minus_" 
                    + std::to_string(cut)).c_str(), "status==0");
        TH1F *R_minus_hist = (TH1F*)gDirectory->Get(("hist_R_minus_" 
                    + std::to_string(cut)).c_str());
        R_minus_hist->Fit("gaus");
        double R_minus = ((TF1*)R_minus_hist->GetFunction("gaus"))->GetParameter("Mean");

        // Fill graphs
        R_plus_graph->SetPoint(count, cut, R_plus);
        R_minus_graph->SetPoint(count, cut, R_minus);

        count++;
    }

    // Plot graphs
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    R_plus_graph->GetXaxis()->SetTitle("ADS BDT cut");
    R_plus_graph->GetYaxis()->SetTitle("R^{+}_{#pi K} error");
    R_plus_graph->Draw();
    canv->SaveAs("../../Plots/R_plus_error.pdf");
    canv->Clear();
    R_minus_graph->GetXaxis()->SetTitle("ADS BDT cut");
    R_minus_graph->GetYaxis()->SetTitle("R^{-}_{#pi K} error");
    R_minus_graph->Draw();
    canv->SaveAs("../../Plots/R_minus_error.pdf");

}

void piKpipi_study() {

    // 1D piKpipi cut study
    gROOT->ForceStyle();
    TGraph * R_plus_graph = new TGraph();
    TGraph * R_minus_graph = new TGraph();
    int count = 0;

    // Loop through cuts
    for (double cut = 0.1; cut < 0.99999; cut += 0.1) {

        // Open files for this cut
        TChain * tree = new TChain("toy_tree");
        std::cout << "cut: " << std::to_string(cut) << std::endl;
        for (int num = 0; num < 10; num++) {
            tree->Add(("/data/lhcb/users/pullen/B02DKstar/toys/BDT_cut_optimisation/"
                    "toyFit_KK_0.500000Kpi_0.500000Kpipipi_" + std::to_string(cut) + 
                    "pipi_0.500000pipipipi_0.500000" + std::to_string(num) 
                    + ".root").c_str());
        }
        std::cout << "Tree has " << tree->GetEntries() << " entries" << std::endl;

        // Get RMS of each
        tree->Draw(("fit_shape_final_error_R_plus_signal_piKpipi >> hist_R_plus_" 
                    + std::to_string(cut)).c_str(), "status==0");
        TH1F *R_plus_hist = (TH1F*)gDirectory->Get(("hist_R_plus_" 
                    + std::to_string(cut)).c_str());
        R_plus_hist->Fit("gaus");
        double R_plus = ((TF1*)R_plus_hist->GetFunction("gaus"))->GetParameter("Mean");
        tree->Draw(("fit_shape_final_error_R_minus_signal_piKpipi >> hist_R_minus_" 
                    + std::to_string(cut)).c_str(), "status==0");
        TH1F *R_minus_hist = (TH1F*)gDirectory->Get(("hist_R_minus_" 
                    + std::to_string(cut)).c_str());
        R_minus_hist->Fit("gaus");
        double R_minus = ((TF1*)R_minus_hist->GetFunction("gaus"))->GetParameter("Mean");

        // Fill graphs
        R_plus_graph->SetPoint(count, cut, R_plus);
        R_minus_graph->SetPoint(count, cut, R_minus);

        count++;
    }

    // Plot graphs
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    R_plus_graph->GetXaxis()->SetTitle("ADS BDT cut");
    R_plus_graph->GetYaxis()->SetTitle("R^{+}_{#pi K #pi#pi} error");
    R_plus_graph->Draw();
    canv->SaveAs("../../Plots/R_plus_piKpipi_error.pdf");
    canv->Clear();
    R_minus_graph->GetXaxis()->SetTitle("ADS BDT cut");
    R_minus_graph->GetYaxis()->SetTitle("R^{-}_{#pi K #pi#pi} error");
    R_minus_graph->Draw();
    canv->SaveAs("../../Plots/R_minus_piKpipi_error.pdf");

}

// ========================================================
// Script to plot observables at one point of the BDT study
// ========================================================
void Plot_study() {

    // KK_study();
    // pipi_study();
    // pipipipi_study();
    // piK_study();
    // piKpipi_study();

    // KK_2D_study();
    // pipi_2D_study();
    pipipipi_2D_study();

}
