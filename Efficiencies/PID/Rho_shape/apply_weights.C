// Add a PID weight branch to TTree
void apply_weights(TString mag, TString type = "rho", TString year = "2016") {

    // Setup
    TString data_dir = "/data/lhcb/users/pullen/B02DKstar/";
    std::vector<TString> particles = {};
    std::vector<TString> helicities = {};
    if (type == "rho_lowMass" || type == "lowMass" || type == "Bs_lowMass") {
        particles = {"gamma", "pi"};
        helicities = {"001", "010", "100"};
    }

    for (auto particle : particles) {
        for (auto hel : helicities) {

            // File names
            std::cout << "Processing " << type << " " << particle << " " << 
                hel << std::endl;
            TString infile = data_dir + "MC/backgrounds/" + type + "/" + particle +
                "/" + hel + "/" + year + "_" + mag + "/Kpi_selected.root";
            TString outfile = data_dir + "MC/backgrounds/" + type + "/" + particle +
                "/" + hel + "/" + year + "_" + mag + 
                "/Kpi_selected_withPIDweights.root";
            TString weights_file;
            if (type != "rho") {
                weights_file = data_dir + "PIDCalib/Results/" + year + "_" + mag
                + "/" + type + "_Kpi_" + particle + "_" + hel + ".root";
            } else {
                weights_file = data_dir + "PIDCalib/Results/" + year + "_" + mag
                + "/" + type + "_Kpi.root";
            }

            // Open input files
            std::cout << "Opening input file: " << infile << std::endl;
            TFile * file_in = TFile::Open(infile, "READ");
            TTree * intree = (TTree*)file_in->Get("DecayTree");
            if (intree->GetEntries() == 0) {
                std::cout << "No selected entries!" << std::endl;
                continue;
            }

            // Open output files
            TFile * file_weights = TFile::Open(weights_file, "READ");
            TTree * weights_tree = (TTree*)file_weights->Get("CalibTool_PIDCalibTree");

            // PIDCalib cut
            TString eta = (year == "2016") ? "ETA" : "LOKI_ETA";
            TString cut = "3000 <= D0K_P && 100000 > D0K_P && 1.5 <= D0K_" + eta + " && "
                "5 > D0K_" + eta + " && 3000 <= D0Pi_P && 100000 > D0Pi_P && 1.5 <= D0Pi_" + eta + 
                "&& 5 > D0Pi_" + eta + " && 3000 <= KstarK_P && 100000 > KstarK_P && "
                "1.5 <= KstarK_" + eta + " && 5 > KstarK_" + eta + " && 3000 <= KstarPi_P && "
                "100000 > KstarPi_P && 1.5 <= KstarPi_" + eta + " && 5 > KstarPi_" + eta;

            // Check entries
            std::cout << "Weights entries: " << weights_tree->GetEntries() << std::endl;
            std::cout << "Data entries: " << intree->GetEntries(cut) << std::endl;

            // Make output tree
            TFile * file_out = TFile::Open(outfile, "RECREATE");
            TTree * outtree = intree->CloneTree(0);
            float eff;
            outtree->Branch("PID_efficiency", &eff, "PID_efficiency/F");
            float eff_in;
            weights_tree->SetBranchAddress("Event_PIDCalibEff", &eff_in);

            // Apply cut and loop through
            intree->Draw(">>elist", cut);
            TEventList * evtList = (TEventList*)gDirectory->Get("elist");
            for (int i = 0; i < evtList->GetN(); i++) {
                intree->GetEntry(evtList->GetEntry(i));
                weights_tree->GetEntry(i);
                eff = eff_in;
                outtree->Fill();
            }

            // Save
            outtree->Write("DecayTree");
            file_out->Close();
            file_in->Close();
            file_weights->Close();
        }
    }
}
