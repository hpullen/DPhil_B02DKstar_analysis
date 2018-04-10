int GetEvents(TString filepath) {
    TFile * file = TFile::Open(filepath, "READ");
    TTree * tree = (TTree*)file->Get("DecayTree");
    int entries = tree->GetEntries();
    file->Close();
    return entries;
}


// =================================================================================================
// Script to create a LaTeX table with numbers of signal and background events used for BDT training
// =================================================================================================
void training_events_table() {

    // Open table file
    ofstream table("/home/pullen/analysis/B02DKstar/ANA_resources/Tables/Selection/"
            "BDT/training_events.tex");
    table << "\\begin{table}[h]" << std::endl;
    table << "  \\centering" << std::endl;
    table << "  \\begin{tabular}{lccccc}" << std::endl;
    table << "      \\toprule" << std::endl;
    table << "      & $K\\pi$ & $KK$ & $\\pi\\pi$ & $K\\pi\\pi\\pi$ && "
        "$\\pi\\pi\\pi\\pi$ \\\\" << std::endl;
    table << "      \\midrule" << std::endl;

    // Get event numbers
    TString data_dir = "/data/lhcb/users/pullen/B02DKstar/";
    std::map<TString, int> n_sig;
    std::map<TString, int> n_bkg;
    for (TString mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
        n_sig[mode] = 0;
        n_bkg[mode] = 0;
    }
    for (TString mag : {"up", "down"}) {
        for (TString year : {"2015", "2016"}) {
            for (TString mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {
                if ((mode == "Kpipipi" || mode == "pipipipi") && year == "2015") {
                    continue;
                }
                TString bod = (mode == "Kpipipi" || mode == "pipipipi") ?
                     "fourBody" : "twoBody";
                n_sig[mode] += GetEvents(data_dir + "MC/" + bod + "/" + mode + "/" + year + 
                        "_" + mag + "/" + mode + "_trainingSample.root");
                n_bkg[mode] += GetEvents(data_dir + "data/" + bod + "/" + year + "_" + mag + 
                        "/" + mode + "_trainingSample.root");
            }
        }
    }

    // Print signal and background numbers
    table << "      Signal & " << n_sig["Kpi"] << " & " << n_sig["KK"] << " & " <<
        n_sig["pipi"] << " & " << n_sig["Kpipipi"] << " & " << n_sig["pipipipi"] <<
        " \\\\" << std::endl;
    table << "      Background & " << n_bkg["Kpi"] << " & " << n_bkg["KK"] << " & " 
        << n_bkg["pipi"] << " & " << n_bkg["Kpipipi"] << " & " << n_bkg["pipipipi"] <<
        " \\\\" << std::endl;

    // Finish table
    table << "      \\bottomrule" << std::endl;
    table << "  \\end{tabular}" << std::endl;
    table << "\\end{table}" << std::endl;
    table.close();
}

