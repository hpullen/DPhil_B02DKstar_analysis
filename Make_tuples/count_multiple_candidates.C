// ============================================================================================
// Script to count the number of events containing multiple B candidates in the selected sample
// ============================================================================================
std::string count_one(std::string mode, std::string run) {

    // Get years and nBodies
    TString bod = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi") ?
        "fourBody" : "twoBody";
    std::vector<TString> years;
    if (run == "run2") {
        years = {"2015", "2016"};
    } else {
        years = {"2011", "2012"};
    }

    // Open file
    TChain * tree = new TChain("DecayTree");
    for (auto const & year : years) {
        for (TString mag : {"_up", "_down"}) {
            tree->Add("/data/lhcb/users/pullen/B02DKstar/data/" + bod 
                    + "/" + year + mag + "/" + mode + "_selected.root");
        }
    }

    // Set up event number
    int evtNum = 0;
    tree->SetBranchAddress("eventNumber", &evtNum);
    std::map<int, int> evtCounts;

    // Loop through and count events
    for (unsigned int i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        if (evtCounts.find(evtNum) == evtCounts.end()) {
            evtCounts[evtNum] = 1;
        } else {
            evtCounts[evtNum]++;
        }
    }

    // Get number of multiple candidates
    double count_all = 0;
    int count_multiple = 0;
    for (auto event : evtCounts) {
        count_all++;
        if (event.second > 1) {
            count_multiple++;
        }
    }

    // Return number and ratio
    std::stringstream output;
    output << std::setprecision(2) << count_multiple/count_all;
    std::string full = output.str() + " (" + std::to_string(count_multiple) + ")";
    return full;
}


// Main function
void count_multiple_candidates() {

    for (std::string run : {"run1", "run2"}) {
        std::cout << "Kpi_" << run << " " << count_one("Kpi", run) << std::endl;
        std::cout << "piK_" << run << " " << count_one("piK", run) << std::endl;
        std::cout << "KK_" << run << " " << count_one("KK", run) << std::endl;
        std::cout << "pipi_" << run << " " << count_one("pipi", run) << std::endl;
        std::cout << "Kpipipi_" << run << " " << count_one("Kpipipi", run) << std::endl;
        std::cout << "piKpipi_" << run << " " << count_one("piKpipi", run) << std::endl;
    }
    std::cout << "pipipipi_run2" << " " << count_one("pipipipi", "run2") << std::endl;
}
