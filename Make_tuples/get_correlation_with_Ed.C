// Find correlation between my selected sample and Ed's
void get_correlation_with_Ed(bool use_data = false) {

    // Loop through modes
    for (TString mode : {"Kpi", "KK", "pipi"}) {

        // Open selected ntuples
        TChain * my_tree = new TChain("DecayTree");
        TChain * ed_tree = new TChain("DecayTree");
        TString mc_path = "/data/lhcb/users/pullen/B02DKstar/MC/twoBody/" + mode;
        TString data_path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/"; 
        for (TString mag : {"_up", "_down"}) {
            if (!use_data) {
                my_tree->Add(mc_path + "/2012" + mag + "/" + mode + "_selected.root"); 
                ed_tree->Add(mc_path + "/2012" + mag + "/" + mode + "_selected_Ed.root"); 
            } else {
                my_tree->Add(data_path + "/2011" + mag + "/" + mode + "_selected.root"); 
                my_tree->Add(data_path + "/2012" + mag + "/" + mode + "_selected.root"); 
                ed_tree->Add(data_path + "/2011" + mag + "/" + mode + "_selected_Ed.root"); 
                ed_tree->Add(data_path + "/2012" + mag + "/" + mode + "_selected_Ed.root"); 
            }
        }

        // Print entries for each
        if (use_data) {
            std::cout << "\n Run 1 " << mode << ": " << std::endl;
        } else {
            std::cout << "\n 2012 " << mode << ": " << std::endl;
        }
        std::cout << "Ed entries: " << ed_tree->GetEntries() << std::endl;
        std::cout << "My entries: " << my_tree->GetEntries() << std::endl;

        // Draw B mass region to event lists
        my_tree->Draw(">>my_elist", "abs(Bd_ConsD_M - 5279) < 25");
        TEventList * my_elist = (TEventList*)gDirectory->Get("my_elist");
        ed_tree->Draw(">>ed_elist", "abs(Bd_ConsD_M - 5279) < 25");
        TEventList * ed_elist = (TEventList*)gDirectory->Get("ed_elist");

        // Get correlation for each
        int duplicate = 0;
        ULong64_t ed_event_num = 0;
        ed_tree->SetBranchAddress("eventNumber", &ed_event_num);
        for (int i = 0; i < ed_elist->GetN(); i++) {

            // Get the entry
            Long64_t entry = ed_elist->GetEntry(i);
            ed_tree->GetEntry(entry);

            // Check if event number appears in my tuple
            if (my_tree->GetEntries(("eventNumber == " + std::to_string(ed_event_num)).c_str()) > 0) {
                duplicate++;
            }
        }

        // Print result
        std::cout << "Overlapping entries: " << duplicate << std::endl;
        std::cout << "Correlation with my sample: " << static_cast<double>(duplicate)/my_tree->GetEntries("abs(Bd_ConsD_M - 5279) < 25") << std::endl;
    }
}
