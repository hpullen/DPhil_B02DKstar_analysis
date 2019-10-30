void compare_2body_3body() {

    TChain * tree2 = new TChain("DecayTree");
    TString data_dir = "/data/lhcb/users/pullen/";
    tree2->Add(data_dir + "B02DKstar/MC/twoBody/Kpi/2016_up/Kpi_selected.root");
    tree2->Add(data_dir + "B02DKstar/MC/twoBody/Kpi/2016_down/Kpi_selected.root");

    for (TString t : {"LL", "DD"}) {
        TChain * tree3 = new TChain("DecayTree");
        tree3->Add(data_dir + "GGSZ/MC/truth_matched/bdt_applied/default_precuts/"
                "D2KsPiPi_" + t + "_16_Up.root");
        tree3->Add(data_dir + "GGSZ/MC/truth_matched/bdt_applied/default_precuts/"
                "D2KsPiPi_" + t + "_16_Down.root");

        // Make histograms
        TH1F * hist2 = new TH1F("hist2_" + t, "", 100, 5200, 5350);
        TH1F * hist3 = new TH1F("hist4_" + t, "", 100, 5200, 5350);

        // Draw B mass
        tree2->Draw("Bd_ConsD_M>>hist2_" + t);
        tree3->Draw("Bu_constD0KSPV_M>>hist4_" + t, "MVA>0.8");

        // Make pulls
        // RooRealVar mass("mass", "", 5200, 5350);
        // RooArgList args(mass);
        // RooDataHist * dHist2 = new RooDataHist("dhist2_" + t, "", args, hist2);
        // RooDataHist * dHist3 = new RooDataHist("dhist3_" + t, "", args, hist3);
        // RooPlot * plt = mass.frame();
        // dHist2->plotOn(plt);
        // dHist3->plotOn(plt);
        // RooHist * pulls = plt->pullHist();
        TH1F * pulls = new TH1F("pulls_" + t, "", 100, 5200, 5350);
        for (int i = 1; i <= 100; i++) {
            double pull = 0;
            int bin_content = hist2->GetBinContent(i) + hist3->GetBinContent(i);
            if (bin_content > 0.5) {
                pull = (hist2->GetBinContent(i) - hist3->GetBinContent(i))/sqrt(hist2->GetBinContent(i) + hist3->GetBinContent(i));
            }
            pulls->Fill(hist2->GetBinCenter(i), pull);
        }

        // Plot together
        hist2->SetStats(false);
        hist3->SetStats(false);
        hist2->Scale(1/hist2->Integral());
        hist3->Scale(1/hist3->Integral());
        hist2->SetLineColor(kRed);
        hist3->SetLineColor(kBlue);
        hist2->SetLineWidth(1);
        hist3->SetLineWidth(1);
        TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
        hist3->Draw("E");
        hist2->Draw("E SAME");
        TLegend * leg = new TLegend(0.7, 0.75, 0.9, 0.9);
        leg->AddEntry(hist2, "D#rightarrow K#pi", "L");
        leg->AddEntry(hist3, "D#rightarrow K^{0}_{S}#pi#pi", "L");
        leg->Draw();
        gPad->RedrawAxis();

        // Plot pulls
        // TPad * pad2 = new TPad("pad2", "", 0, 0, 0.3, 1);
        // pad2->cd();
        // pulls->Paint();
        // canvas->cd();
        // pad1->Draw();
        // pad2->Draw();
        canvas->SaveAs("../Plots/compare_2body_3body_" + t + ".pdf");
    }
}
