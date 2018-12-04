// ==============================================
// Plot invariant mass difference of m(D*) - m(D)
// ==============================================
void plot_mass(std::string mode, bool selected = true) {

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
    if (selected) {
        outdir += "selected";
    } else {
        outdir += "preselection";
    }

    // Cut to apply to K* daughters (PID cut on full data samples)
    TCut cut = "";
    // if (selected) {
        // cut = "KstarK_PIDK > 5";
        // cut += "KstarPi_PIDK < -1";
    // }

    // Loop through Kstar daughters
    for (TString d : {"KstarK", "KstarPi"}) {

        // Mass of pion 
        TString m_pi = "139.57";

        // Make invariant mass string 
        TString inv_mass = "sqrt(pow(D0_PE + sqrt(" + d + "_PE * " + d + "_PE + " + m_pi + "*" + m_pi + "), 2)";
        for (TString mom : {"PX", "PY", "PZ"}) {
            inv_mass += " - pow(D0_" + mom + " + " + d + "_" + mom + ", 2)";
        }
        inv_mass += ")";

        // Mass mass difference string
        TString delta_mass = inv_mass + " - D0_M";

        // Plot
        // TH1F * hist = new TH1F("hist", "", 100, 0, 300);
        tree->Draw(delta_mass + ">>hist", cut);
        TH1F * hist = (TH1F*)gDirectory->Get("hist");
        TCanvas * canv = new TCanvas("canvas", "", 900, 600);
        hist->SetLineWidth(1);
        hist->Draw();
        canv->SaveAs((outdir + "/" + mode + "_" + std::string(d) + ".pdf").c_str());
        canv->Clear();
        tree->Draw(inv_mass + ">>hist_mass", cut);
        TH1F * hist_mass = (TH1F*)gDirectory->Get("hist_mass");
        hist_mass->SetLineWidth(1);
        hist_mass->Draw();
        canv->SaveAs((outdir + "/" + mode + "_" + std::string(d) + "_Dstar_mass.pdf").c_str());
        delete hist;
        delete hist_mass;
    }



}
