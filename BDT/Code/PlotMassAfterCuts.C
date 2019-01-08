// ==========================================================================
// Plot Kpi invariant mass distribution after various cuts in Run 1 and Run 2
// ==========================================================================
void PlotMassAfterCuts() {

    for (TString run : {"1", "2"}) {

        // Load data
        TChain * tree = new TChain("DecayTree");
        TString data_path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
        for (TString mag : {"_up", "_down"}) {
            if (run == "1") {
                tree->Add(data_path + "2011" + mag + "/Kpi_withBDTG.root");
                tree->Add(data_path + "2012" + mag + "/Kpi_withBDTG.root");
            } else {
                tree->Add(data_path + "2015" + mag + "/Kpi_withBDTG.root");
                tree->Add(data_path + "2016" + mag + "/Kpi_withBDTG.root");
            }
        }

        // Make cut (no BDT cut) 
        TCut cut;
        cut += "D0K_hasRich == 1";
        cut += "D0Pi_hasRich == 1";
        cut += "D0K_PIDK > 1";
        cut += "D0Pi_PIDK < -1";
        cut += "abs(D0_M_doubleSwap - 1864.84) > 15";
        cut += "abs(sqrt(pow(KstarK_PE + KstarPi_PE + D0Pi_PE, 2) - "
            "pow(KstarK_PX + KstarPi_PX + D0Pi_PX, 2) - pow(KstarK_PY + "
            "KstarPi_PY + D0Pi_PY, 2) - pow(KstarK_PZ + KstarPi_PZ + "
            "D0Pi_PZ, 2)) - 1869.65) > 15";
        cut += "abs(sqrt(pow(KstarK_PE + KstarPi_PE + D0K_PE, 2) - "
            "pow(KstarK_PX + KstarPi_PX + D0K_PX, 2) - pow(KstarK_PY + "
            "KstarPi_PY + D0K_PY, 2) - pow(KstarK_PZ + KstarPi_PZ + "
            "D0K_PZ, 2)) - 1968.34) > 15";
        cut += "abs(Kstar_M - 895.5) < 50";
        cut += "abs(D0_M - 1864.84) < 25";
        cut += "D0_FDS > 3";
        cut += "KstarK_hasRich == 1";
        cut += "KstarPi_hasRich == 1";
        cut += "KstarK_PIDK > 5";
        cut += "KstarPi_PIDK < -1";
        cut += "abs(DK_mass - 5279.32) > 25";

        // Loop through BDT cuts
        int color = 1;
        TLegend leg(0.6, 0.5, 0.85, 0.9);
        std::map<TString, TH1F*> hists;
        for (TString cut_val : {"-0.6", "-0.3", "0", "0.3", "0.6"}) {

            // Make BDT cut
            TCut BDT_cut = ("BDTG_Kpi_run2 > " + std::string(cut_val)).c_str();

            // Make histogram
            TString hist_name = "hist_" + run + "_" + cut_val;
            TH1F * hist = new TH1F(hist_name, "", 50, 5000, 5800);
            tree->Draw("Bd_ConsD_MD >> " + hist_name, cut + BDT_cut);

            // Style
            color++;
            if (color == 5) color++;
            hist->SetLineWidth(1);
            hist->SetLineColor(color);
            hists[cut_val] = hist;

            // Add to legend
            leg.AddEntry(hist, "BDT > " + cut_val, "L");
        }

        // Draw on canvas
        TCanvas * canv = new TCanvas("canvas", "", 900, 600);
        hists["-0.6"]->Draw("HIST");
        for (TString cut_val : {"-0.3", "0", "0.3", "0.6"}) {
            hists[cut_val]->Draw("HIST SAME");
        } 

        // Save canvas
        gPad->RedrawAxis();
        leg.Draw();
        canv->SaveAs("BDT_cuts_Kpi_run" + run + ".pdf");
        delete canv;
    }
}
