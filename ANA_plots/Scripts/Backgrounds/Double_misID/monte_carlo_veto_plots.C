// ===================================
// Script to plot the veto mass window
// ===================================
void monte_carlo_veto_plots() {

    // Open Run 2 Kpi MC
    TChain * tree = new TChain("DecayTree");
    TString dir = "/data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/";
    for (TString year : {"2015", "2016"}) {
        for (TString mag : {"up", "down"}) {
            tree->Add(dir + year + "_" + mag + "/Kpi_withVars_withCuts.root");
        }
    }

    // Open Kpipipi MC
    TChain * tree_4body = new TChain("DecayTree");
    TString dir_4body = "/data/lhcb/users/pullen/B02DKstar/MC/fourBody/Kpipipi/";
    for (TString mag : {"up", "down"}) {
        tree_4body->Add(dir_4body + "2016_" + mag + "/Kpipipi_withVars_withCuts.root");
    }

    // Cut to apply (no cut on D0 doubly swapped mass)
    TCut cut = "abs(D0_M - 1864.83) < 25 && "
               "Bd_ConsD_MD > 5000 && "
               "Bd_ConsD_MD < 5800 && "
               "D0_FDS > 2 &&"
               "Bd_BKGCAT == 0";
    TCut cut_Kpi = cut + "BDTG_Kpi_run2 > 0.5";
    TCut cut_Kpipipi = cut + "BDTG_Kpipipi_run2 > 0.5";

    // Make histograms
    std::map<TString, TH1F*> hists;
    hists["Kpi_D0_M"] = new TH1F("hist_D0_M", "", 200, 1835, 1895);
    hists["Kpi_D0_M_doubleSwap"] = new TH1F("hist_D0_M_doubleSwap", "", 200, 0, 4000);
    hists["Kpipipi_D0_M_doubleSwap_low"] = new TH1F("hist_D0_M_doubleSwap_low", 
            "", 200, 0, 4000);
    hists["Kpipipi_D0_M_doubleSwap_high"] = new TH1F("hist_D0_M_doubleSwap_high", 
            "", 200, 0, 4000);

    // Draw
    tree->Draw("D0_M>>hist_D0_M", cut_Kpi);
    tree->Draw("D0_M_doubleSwap>>hist_D0_M_doubleSwap", cut_Kpi);
    tree_4body->Draw("D0_M_doubleSwap_low>>hist_D0_M_doubleSwap_low", cut_Kpipipi);
    tree_4body->Draw("D0_M_doubleSwap_high>>hist_D0_M_doubleSwap_high", cut_Kpipipi);

    // Draw and plot
    TCanvas * canv = new TCanvas("canv", "", 500, 500);
    for (auto variable : hists) {
        canv->Clear();
        TString var = variable.first;
        TH1F * hist = variable.second;
        double y_max = hist->GetMaximum() * 1.1;
        hist->GetYaxis()->SetRangeUser(0, y_max);
        hist->SetLineWidth(1);
        hist->Draw("HIST");
        hist->GetXaxis()->SetTitle("#it{m}(D) [MeV/#it{c}^{2}]");
        std::stringstream y_title;
        int precision = (var == "D0_M") ? 1 : 2;
        y_title << "Candidates / (" << std::setprecision(precision) << hist->GetBinWidth(1)
            << " MeV/#it{c}^{2})";
        hist->GetYaxis()->SetTitle(y_title.str().c_str());
        hist->GetYaxis()->SetTitleOffset(1.5);
        canv->SetLeftMargin(0.2);
        TBox box(1864.84 - 15, 0, 1864.84 + 15, y_max);
        box.SetFillColorAlpha(kRed, 0.3);
        box.Draw();
        hist->Draw("HIST SAME");
        gPad->RedrawAxis();
        canv->SaveAs("/home/pullen/analysis/B02DKstar/ANA_plots/Plots/"
                "Backgrounds/Double_misID/" + var + ".pdf");
    }

}
