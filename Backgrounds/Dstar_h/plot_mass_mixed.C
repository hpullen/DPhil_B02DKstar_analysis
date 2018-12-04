// ===============================================================================
// Plot invariant mass difference of m(D*) - m(D) using mix of D0 and K* daughters
// ===============================================================================
void plot_mass_mixed(std::string mode, bool use_PID, bool selected = false) {

    // Read in data
    TString data_dir = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    TChain * tree = new TChain("DecayTree");
    for (TString year : {"2011", "2012", "2015", "2016"}) {
        for (TString mag : {"_up", "_down"}) {
            if (!selected) {
                tree->Add(data_dir + year + mag + "/" + mode + ".root");
            } else {
                tree->Add(data_dir + year + mag + "/" + mode + "_selected.root");
            }
        }
    }

    // Directory to save to
    std::string outdir = "Plots/";

    // Loop through daughter to use as pi in D* -> Dpi
    // for (TString d : {"D0Pi", "KstarPi"}) {

        // Daughters to use for D0
        TString d = "KstarPi";
        TString d1, d2;
        if (d == "D0Pi") {
            d1 = "D0K";
            d2 = "KstarPi";
        } else {
            d1 = "KstarK";
            d2 = "D0Pi";
        }

        // Bachelor PID cut
        TCut PID_cut = "";
        if (use_PID) {
            // PID_cut = "KstarK_PIDK > 5 && D0Pi_PIDK < -1 && D0K_PIDK > 1 && KstarPi_PIDK < -1";
            PID_cut = "KstarK_PIDK > 5";
        }

        // D0 invariant mass string
        TString D0_mass = "sqrt(pow(" + d1 + "_PE + " + d2 + "_PE, 2)";
        for (TString mom : {"PX", "PY", "PZ"}) {
            D0_mass += " - pow(" + d1 + "_" + mom + " + " + d2 + "_" + mom + ", 2)";
        }
        D0_mass += ")";

        // Dstar invariant mass string
        TString Dstar_mass = "sqrt(pow(" + d1 + "_PE + " + d2 + "_PE + " + d + "_PE, 2)";
        for (TString mom : {"PX", "PY", "PZ"}) {
            Dstar_mass += " - pow(" + d1 + "_" + mom + " + " 
                + d2 + "_" + mom + " + " + d + "_" + mom + ", 2)";
        }
        Dstar_mass += ")";

        // Mass mass difference string
        TString delta_mass = Dstar_mass + " - " + D0_mass;
        // + " - " + D0_mass;

        // Plot
        TH1F * hist = new TH1F("hist", "", 100, 0, 1000);
        tree->Draw(delta_mass + ">>hist", PID_cut);
        TCanvas * canv = new TCanvas("canvas", "", 900, 600);

        // Settings
        hist->SetLineWidth(1);
        hist->GetXaxis()->SetTitle("#it{m}(K_{K^{*}}#pi_{D}#pi_{K^{*}}) - #it{m}(K_{K^{*)}}#pi_{D})");
        hist->Draw();
        TString latex;
        // if (d == "KstarPi") {
            // latex = "#it{m}(K_{K^{*}}#pi_{D}#pi_{K^{*}}) - #it{m}(K_{K^{*)}}#pi_{D})";
        // } else {
            // latex = "#it{m}(K_{K^{*}}#pi_{D}#pi_{K^{*}}) - #it{m}(K_{K^{*)}}#pi_{D})";
        // }
        // hist->GetXaxis()->SetTitle(latex);

        // Save
        std::string ext = selected ? "_selected" : "";
        canv->SaveAs((outdir + "/with_Kstar/delta_M" + ext + ".pdf").c_str());

        // Also draw D mass and D* mass
        canv->Clear();
        TH1F * hist_dmass = new TH1F("hist_dmass", "", 100, 1500, 2500);
        tree->Draw(D0_mass + ">>hist_dmass", PID_cut);
        hist_dmass->SetLineWidth(1);
        hist_dmass->GetXaxis()->SetTitle("#it{m}(K_{K^{*)}}#pi_{D})");
        hist_dmass->Draw();
        TLine line(1864.83, 0, 1864.83, hist_dmass->GetMaximum());
        line.SetLineStyle(2);
        line.SetLineColor(kRed);
        line.SetLineWidth(1);
        line.Draw();
        canv->SaveAs((outdir + "/with_Kstar/D0_M" + ext + ".pdf").c_str());

        canv->Clear();
        TH1F * hist_dstmass = new TH1F("hist_dstmass", "", 100, 1500, 4000);
        tree->Draw(Dstar_mass + ">>hist_dstmass", PID_cut);
        hist_dstmass->SetLineWidth(1);
        hist_dstmass->GetXaxis()->SetTitle("#it{m}(K_{K^{*}}#pi_{D}#pi_{K^{*}})");
        hist_dstmass->Draw();
        TLine line_Dst(2010.26, 0, 2010.26, hist_dstmass->GetMaximum());
        line_Dst.SetLineStyle(2);
        line_Dst.SetLineColor(kRed);
        line_Dst.SetLineWidth(1);
        line_Dst.Draw();
        canv->SaveAs((outdir + "/with_Kstar/Dst_M" + ext + ".pdf").c_str());

        
        delete hist;
        delete hist_dmass;
        delete hist_dstmass;



}
