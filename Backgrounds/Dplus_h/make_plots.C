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
void make_plots(TString mode, bool selected = false) {

    // Daughter names and labels for each mode
    std::vector<TString> names;
    std::map<TString, TString> labels;
    names.push_back("KstarK");
    labels["KstarK"] = "K^{#pm}_{K*}";
    names.push_back("KstarPi");
    labels["KstarPi"] = "#pi^{#mp}_{K*}";
    if (mode == "Kpi") {
        names.push_back("D0K");
        labels["D0K"] = "K^{#pm}_{D}";
        names.push_back("D0Pi");
        labels["D0Pi"] = "#pi^{#mp}_{D}";
    } else if (mode == "piK") {
        names.push_back("D0K");
        labels["D0K"] = "K^{#mp}_{D}";
        names.push_back("D0Pi");
        labels["D0Pi"] = "#pi^{#pm}_{D}";
    } else if (mode == "KK") {
        names.push_back("D0Kplus");
        labels["D0Kplus"] = "K^{#pm}_{D}";
        names.push_back("D0Kminus");
        labels["D0Kminus"] = "K^{#mp}_{D}";
    } else if (mode == "pipi") {
        names.push_back("D0PiPlus");
        labels["D0PiPlus"] = "#pi^{#pm}_{D}";
        names.push_back("D0PiMinus");
        labels["D0PiMinus"] = "#pi^{#mp}_{D}";
    } else {
        std::cout << "Error: mode " << mode << " not recognised!"
            << std::endl;
        return;
    }

    // Open input files
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

    // Set up branches for charged daughters
    std::map<TString, TLorentzVector> daughters;
    for (auto name : names) {
        LorentzVectorBranch(tree, daughters[name], name);
    }

    // Make histograms
    gROOT->ForceStyle();
    std::map<int, TH1F*> hists;
    for (unsigned int i = 0; i < 4; i++) {
        // int n_bins = selected ? 100 : 200;
        int n_bins = 100;
        hists[i] = new TH1F("hist_" + names[i],
                "", n_bins, 1500, 4000);
    }

    // KstarK ID for flavour tagging
    int KstarK_ID = 0;
    tree->SetBranchAddress("KstarK_ID", &KstarK_ID);

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

            // Use opposite combo if GLW and B0-bar
            std::vector<TString> combo_to_use = combo.second;
            if (mode == "KK" || mode == "pipi") {
                if (KstarK_ID < 0) {
                    if (combo.first == 0) {
                        combo_to_use = combos[1];
                    } else if (combo.first == 1) {
                        combo_to_use = combos[0];
                    }
                }
            }

            // Sum vectors for the combo
            TLorentzVector vector_sum(0, 0, 0, 0);
            for (auto daughter : combo_to_use) {
                vector_sum += daughters[daughter];
            }

            // Get mass and fill histogram
            double inv_mass = vector_sum.M();
            hists[combo.first]->Fill(inv_mass);
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
        hists[combo.first]->GetXaxis()->SetTitleOffset(1.1);
        hists[combo.first]->GetYaxis()->SetTitle("Candidates");
        hists[combo.first]->GetYaxis()->SetTitleOffset(1.1);
        hists[combo.first]->SetLineWidth(1);

        // Draw
        TCanvas * canvas = new TCanvas("canvas_" + names[combo.first],
                "", 900, 600);
        hists[combo.first]->Draw("HIST");
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
        TString hist_name = "Plots/" + mode + "/mass";
        for (auto daughter : combo.second) {
            hist_name += "_" + daughter;
        }
        if (selected) hist_name += "_selected";
        hist_name += ".pdf";
        canvas->SaveAs(hist_name);
    }
}
