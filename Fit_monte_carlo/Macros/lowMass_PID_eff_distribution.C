void lowMass_PID_eff_distribution(TString year = "2016") {

    TCanvas * canv = new TCanvas("canv", "", 900, 900);
    for (TString h : {"010", "101"}) {
        for (TString p : {"gamma", "pi"}) {
            for (TString b : {"Bs_", ""}) {

                // Load MC
                TString name = b + p + "_" + h + "_" + year;
                TChain * tree = new TChain("DecayTree");
                TString dir = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/"
                    + b + "lowMass/" + p;
                std::vector<TString> hels;
                if (h == "010") hels = {"010"};
                else hels = {"100", "001"};
                for (auto hf : hels) {
                    for (auto m : {"_up", "_down"}) {
                        tree->Add(dir + "/" + hf + "/" + year + m + 
                                "/Kpi_selected_withPIDweights.root");
                    }
                }

                // Draw B mass vs PID efficiency
                canv->Clear();
                canv->cd();
                tree->Draw("PID_efficiency:Bd_ConsD_MD", "", "COLZ");
                canv->SaveAs("../Plots/Macro_output/lowMass_PID_efficiency/"
                        + name + ".pdf");
            }
        }
    }


}
