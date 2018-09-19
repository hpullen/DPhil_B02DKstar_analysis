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
void make_4body_plots(TString mode, bool selected = false) {

    // Daughter names and labels for each mode
    std::vector<TString> names;
    std::map<TString, TString> labels;
    names.push_back("KstarK");
    labels["KstarK"] = "K^{#pm}_{K*}";
    names.push_back("KstarPi");
    labels["KstarPi"] = "#pi^{#mp}_{K*}";
    if (mode == "Kpipipi") {
        names.push_back("D0K");
        labels["D0K"] = "K^{#pm}_{D}";
        names.push_back("D0Pi");
        labels["D0Pi"] = "#pi^{#mp}_{D}";
        names.push_back("D0PiPlus");
        labels["D0Pi"] = "#pi^{#pm}_{D}";
        names.push_back("D0PiMinus");
        labels["D0PiMinus"] = "#pi^{#mp}_{D}";
    } else if (mode == "piKpipi") {
        names.push_back("D0K");
        labels["D0K"] = "K^{#mp}_{D}";
        names.push_back("D0Pi");
        labels["D0Pi"] = "#pi^{#pm}_{D}";
        names.push_back("D0PiPlus");
        labels["D0Pi"] = "#pi^{#pm}_{D}";
        names.push_back("D0PiMinus");
        labels["D0PiMinus"] = "#pi^{#mp}_{D}";
    } else if (mode == "pipipipi") {
        names.push_back("D0PiPlus1");
        labels["D0K"] = "#pi^{#pm}_{D}";
        names.push_back("D0PiPlus2");
        labels["D0Pi"] = "#pi^{#pm}_{D}";
        names.push_back("D0PiMinus1");
        labels["D0K"] = "#pi^{#mp}_{D}";
        names.push_back("D0PiMinus2");
        labels["D0Pi"] = "#pi^{#mp}_{D}";
    } else {
        std::cout << "Error: mode " << mode << " not recognised!"
            << std::endl;
        return;
    }

    // Open input files
    TString data_dir = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/";
    TChain * tree = new TChain("DecayTree");
    for (TString year : {"2011", "2012", "2015", "2016"}) {
        if (mode == "pipipipi" && (year == "2011" || year == "2012")) continue;
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
    int n_bins = 100;
    for (unsigned int i = 0; i < 6; i++) {
        // int n_bins = selected ? 100 : 200;
        hists[i] = new TH1F("hist_" + names[i],
                "", n_bins, 1500, 4000);
    }

    // KstarK ID for flavour tagging
    int KstarK_ID = 0;
    tree->SetBranchAddress("KstarK_ID", &KstarK_ID);

    // Make daughter combinations;
    std::map<int, std::vector<TString>> combos;
    combos[0] = {names[0], names[1], names[2], names[3], names[4]};
    combos[1] = {names[0], names[1], names[2], names[3], names[5]};
    combos[2] = {names[0], names[1], names[2], names[4], names[5]};
    combos[3] = {names[0], names[1], names[3], names[4], names[5]};
    combos[4] = {names[0], names[2], names[3], names[4], names[5]};
    combos[5] = {names[1], names[2], names[3], names[4], names[5]};

    // Extra KK histogram (look for phi)
    TH1F * hist_KK = new TH1F("hist_KK", "", n_bins, 0, 2000);

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

        // Fill KK histogram
        if (mode != "pipipipi") {
            TLorentzVector KK_sum = daughters["KstarK"] + daughters["D0K"];
            hist_KK->Fill(KK_sum.M());
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
        D_line.SetLineColor(ANAGreen);
        D_line.SetLineWidth(1);
        Ds_line.SetLineStyle(2);
        Ds_line.SetLineColor(ANAGreen);
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

    // Draw KK histogram
    if (mode != "pipipipi") {

        // X and Y labels
        TString x_string = "#it{m}(KK)";
        hist_KK->GetXaxis()->SetTitle(x_string);
        hist_KK->GetXaxis()->SetTitleOffset(1.1);
        hist_KK->GetYaxis()->SetTitle("Candidates");
        hist_KK->GetYaxis()->SetTitleOffset(1.1);
        hist_KK->SetLineWidth(1);

        // Draw
        TCanvas * canvas = new TCanvas("canvas_KK", "", 900, 600);
        hist_KK->Draw("HIST");
        canvas->Update();

        // Draw lines at phi mass
        double phi_mass = 1019.445;
        double y_min = gPad->GetUymin();
        double y_max = gPad->GetUymax();
        TLine phi_line(phi_mass, y_min, phi_mass, y_max);
        phi_line.SetLineStyle(2);
        phi_line.SetLineColor(ANAGreen);
        phi_line.SetLineWidth(1);
        phi_line.Draw();
        gPad->RedrawAxis();

        // Save
        TString hist_name = "Plots/" + mode + "/mass_KK";
        if (selected) hist_name += "_selected";
        hist_name += ".pdf";
        canvas->SaveAs(hist_name);
    }

}
