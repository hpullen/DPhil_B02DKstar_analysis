// ==========================================================================
// Script to make plots of invariant masses of 3-body charged daughter combos
// ==========================================================================

// Function to set up Lorentz vector branch
void LorentzVectorBranch(TTree* tree, TLorentzVector& vec,
        const TString& name) {
    tree->SetBranchAddress(name + "_PE", &vec[3]);
    tree->SetBranchAddress(name + "_PX", &vec[0]);
    tree->SetBranchAddress(name + "_PY", &vec[1]);
    tree->SetBranchAddress(name + "_PZ", &vec[2]);
}


// Main function
void make_overlay_plots(TString mode) {

    // Daughter names and labels for each mode
    std::vector<TString> names;
    std::map<TString, TString> labels;
    names.push_back("KstarK");
    labels["KstarK"] = "K^{+}_{K^{*}}";
    names.push_back("KstarPi");
    labels["KstarPi"] = "#pi^{-}_{K^{*}}";
    if (mode == "Kpi") {
        names.push_back("D0K");
        labels["D0K"] = "K^{+}_{D}";
        names.push_back("D0Pi");
        labels["D0Pi"] = "#pi^{-}_{D}";
    } else if (mode == "piK") {
        names.push_back("D0K");
        labels["D0K"] = "K^{-}_{D}";
        names.push_back("D0Pi");
        labels["D0Pi"] = "#pi^{+}_{D}";
    } else if (mode == "KK") {
        names.push_back("D0Kplus");
        labels["D0Kplus"] = "K^{+}_{D}";
        names.push_back("D0Kminus");
        labels["D0Kminus"] = "K^{-}_{D}";
    } else if (mode == "pipi") {
        names.push_back("D0PiPlus");
        labels["D0PiPlus"] = "#pi^{+}_{D}";
        names.push_back("D0PiMinus");
        labels["D0PiMinus"] = "#pi^{-}_{D}";
    } else {
        std::cout << "Error: mode " << mode << " not recognised!"
            << std::endl;
        return;
    }

    // Open input files
    TString data_dir = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    TChain * tree = new TChain("DecayTree");
    TChain * tree_selected = new TChain("DecayTree");
    for (TString year : {"2011", "2012", "2015", "2016"}) {
        for (TString mag : {"_up", "_down"}) {
            tree->Add(data_dir + year + mag + "/" + mode + ".root");
            tree_selected->Add(data_dir + year + mag + "/" + mode + "_selected.root");
        }
    }

    // Set up branches for charged daughters
    std::map<TString, TLorentzVector> daughters;
    std::map<TString, TLorentzVector> daughters_selected;
    for (auto name : names) {
        LorentzVectorBranch(tree, daughters[name], name);
        LorentzVectorBranch(tree_selected, daughters_selected[name], name);
    }

    // Make histograms
    gROOT->ForceStyle();
    std::map<int, TH1F*> hists;
    std::map<int, TH1F*> hists_selected;
    for (unsigned int i = 0; i < 4; i++) {
        hists[i] = new TH1F("hist_" + names[i],
                "", 100, 1500, 4000);
        hists_selected[i] = new TH1F("hist_selected_" + names[i],
                "", 100, 1500, 4000);
    }

    // Make daughter combinations;
    std::map<int, std::vector<TString>> combos;
    combos[0] = {names[0], names[1], names[2]};
    combos[1] = {names[0], names[1], names[3]};
    combos[2] = {names[0], names[2], names[3]};
    combos[3] = {names[1], names[2], names[3]};

    // Fill histograms
    for (unsigned int i = 0; i < tree->GetEntries(); i++) {

        // Get entry 
        tree->GetEntry(i);

        // Calculate masses and fill histograms
        for (auto combo : combos) {

            // Sum vectors for the combo
            TLorentzVector vector_sum(0, 0, 0, 0);
            for (auto daughter : combo.second) {
                vector_sum += daughters[daughter];
            }

            // Get mass and fill histogram
            double inv_mass = vector_sum.M();
            hists[combo.first]->Fill(inv_mass);
        }
    }
    for (unsigned int i = 0; i < tree_selected->GetEntries(); i++) {

        // Get entry 
        tree_selected->GetEntry(i);

        // Calculate masses and fill histograms
        for (auto combo : combos) {

            // Sum vectors for the combo
            TLorentzVector vector_sum(0, 0, 0, 0);
            for (auto daughter : combo.second) {
                vector_sum += daughters_selected[daughter];
            }

            // Get mass and fill histogram
            double inv_mass = vector_sum.M();
            hists_selected[combo.first]->Fill(inv_mass);
        }
    }

    // Draw and save histograms
    for (auto combo : combos) {

        // X and Y labels
        TString x_string = "#it{m}(";
        for (auto daughter : combo.second) {
            x_string += labels[daughter];
        }
        x_string += ")";
        hists[combo.first]->GetXaxis()->SetTitle(x_string);
        hists[combo.first]->GetYaxis()->SetTitle("Candidates");
        hists[combo.first]->SetLineWidth(1);
        hists_selected[combo.first]->SetLineWidth(0);
        hists_selected[combo.first]->SetLineColor(ANABlue);
        hists_selected[combo.first]->SetFillColorAlpha(ANABlue, 0.5);
        hists_selected[combo.first]->Scale(hists[combo.first]->Integral()
                / hists_selected[combo.first]->Integral());

        // Draw
        TCanvas * canvas = new TCanvas("canvas_" + names[combo.first],
                "", 900, 600);
        hists[combo.first]->Draw("HIST");
        hists_selected[combo.first]->Draw("HIST_SAME");
        hists[combo.first]->Draw("HIST SAME");
        canvas->Update();

        // Draw lines at D and Ds mass
        double D_mass = 1869.65;
        double Ds_mass = 1968.34;
        double y_min = gPad->GetUymin();
        double y_max = gPad->GetUymax();
        TLine D_line(D_mass, y_min, D_mass, y_max);
        TLine Ds_line(Ds_mass, y_min, Ds_mass, y_max);
        D_line.SetLineStyle(2);
        D_line.SetLineColor(kRed);
        D_line.SetLineWidth(1);
        Ds_line.SetLineStyle(2);
        Ds_line.SetLineColor(kRed);
        Ds_line.SetLineWidth(1);
        D_line.Draw();
        Ds_line.Draw();
        gPad->RedrawAxis();

        // Save
        TString hist_name = "Plots/" + mode + "/overlay";
        for (auto daughter : combo.second) {
            hist_name += "_" + daughter;
        }
        hist_name += ".pdf";
        canvas->SaveAs(hist_name);
    }
}
