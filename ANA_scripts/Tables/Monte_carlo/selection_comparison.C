// ============================================================================
// Make table comparing selection efficiency of different cuts for Kpi and K3pi
// ============================================================================
void print_line(std::ofstream * file, std::string header, 
        double N_init_Kpi, double N_final_Kpi, 
        double N_init_K3pi, double N_final_K3pi) {

    // Get efficiency
    double eff_Kpi = N_final_Kpi/N_init_Kpi;
    double err_Kpi = (1/N_init_Kpi) * sqrt(N_final_Kpi * (1 - N_final_Kpi/N_init_Kpi));
    double eff_K3pi = N_final_K3pi/N_init_K3pi;
    double err_K3pi = (1/N_init_K3pi) * sqrt(N_final_K3pi * (1 - N_final_K3pi/N_init_K3pi));

    // Print line to file
    std::stringstream line;
    line << "      ";
    line << header << std::fixed << std::setprecision(2);
    if (header.find("Full") != std::string::npos) {
        line << std::setprecision(3);
    }
    line << " & " << eff_Kpi * 100 << " $\\pm$ " << err_Kpi * 100;
    if (header.find("$D_{(s)}h$") == std::string::npos) {
        line << " & " << eff_K3pi * 100 << " $\\pm$ " << err_K3pi * 100;
    } else {
        line << "& n/a";
    }
    line << " \\\\\n";
    *file << line.str();
}


// ==========================
// Get efficiency of each cut
// ==========================
void selection_comparison() {

    // Original bookkeeping entries (2016 MC only)
    double N_bkk_Kpi = 503907 + 503354;
    double N_bkk_K3pi = 1008731 + 1004951;

    // Start table
    std::ofstream file("../../../ANA_resources/Tables/Monte_carlo/selection_comparison.tex");
    file << "\\begin{table}\n";
    file << "   \\centering\n";
    file << "   \\begin{tabular}{ccc}\n";
    file << "      \\toprule\n";
    file << "      Cut & $K\\pi$ efficiency (\\%) & $K\\pi\\pi\\pi$ efficiency (\\%)\\\\\n";
    file << "      \\midrule\n";

    // Open nTuples with everything after stripping
    TString dir = "/data/lhcb/users/pullen/B02DKstar/MC/";
    TChain * tree_Kpi_total = new TChain("Tuple_Kpi/DecayTree");
    TChain * tree_K3pi_total = new TChain("Tuple_K3pi/DecayTree");
    for (TString mag : {"_down", "_up"}) {
        tree_Kpi_total->Add(dir + "twoBody/Kpi/2016" + mag + "/Total.root");
        tree_K3pi_total->Add(dir + "fourBody/Kpipipi/2016" + mag + "/Total.root");
    }

    // Get stripping/reco efficiency
    print_line(&file, "Reconstruction and stripping", N_bkk_Kpi, tree_Kpi_total->GetEntries(),
        N_bkk_K3pi, tree_K3pi_total->GetEntries());

    // Get K*0 mass cut efficiency
    file << "      \\midrule\n";
    TCut truth_cut = "Bd_BKGCAT == 0";
    TCut Kstar_M_cut = "abs(Kstar_M - 895.55) < 50 && Bd_BKGCAT == 0" + truth_cut;
    print_line(&file, "$K^{*0}$ mass window", 
            tree_Kpi_total->GetEntries(truth_cut),
            tree_Kpi_total->GetEntries(Kstar_M_cut),
            tree_K3pi_total->GetEntries(truth_cut),
            tree_K3pi_total->GetEntries(Kstar_M_cut));
    delete tree_Kpi_total;
    delete tree_K3pi_total;

    // Get tuple with full helicity
    TChain * tree_Kpi_hel = new TChain("DecayTree");
    TChain * tree_K3pi_hel = new TChain("DecayTree");
    for (TString mag : {"_down", "_up"}) {
        tree_Kpi_hel->Add(dir + "twoBody/Kpi/2016" + mag + "/Full_helicity/Kpi.root");
        tree_K3pi_hel->Add(dir + "fourBody/Kpipipi/2016" + mag + "/Full_helicity/Kpipipi.root");
    }

    // Get helicity cut efficiency
    TCut hel_cut = "abs(Kstar_helicity_angle) > 0.4" + truth_cut;
    print_line(&file, "Helicity angle",
            tree_Kpi_hel->GetEntries(truth_cut),
            tree_Kpi_hel->GetEntries(hel_cut),
            tree_K3pi_hel->GetEntries(truth_cut),
            tree_K3pi_hel->GetEntries(hel_cut));
    delete tree_Kpi_hel;
    delete tree_K3pi_hel;

    // Get tuple with BDT applied
    TChain * tree_Kpi = new TChain("DecayTree");
    TChain * tree_K3pi = new TChain("DecayTree");
    for (TString mag : {"_down", "_up"}) {
        tree_Kpi->Add(dir + "twoBody/Kpi/2016" + mag + "/Kpi_withBDTG.root");
        tree_K3pi->Add(dir + "fourBody/Kpipipi/2016" + mag + "/Kpipipi_withBDTG.root");
    }

    // D0 mass cut
    TCut D0_cut = "abs(D0_M - 1864.84) < 25" + truth_cut;
    print_line(&file, "$D^0$ mass window",
            tree_Kpi->GetEntries(truth_cut),
            tree_Kpi->GetEntries(D0_cut),
            tree_K3pi->GetEntries(truth_cut),
            tree_K3pi->GetEntries(D0_cut));

    // BDT cut
    TCut cut_Kpi = D0_cut + "BDTG_Kpi_run2 > 0";
    TCut cut_K3pi = D0_cut + "BDTG_Kpipipi_run2 > 0";
    print_line(&file, "BDT cut",
            tree_Kpi->GetEntries(D0_cut),
            tree_Kpi->GetEntries(cut_Kpi),
            tree_K3pi->GetEntries(D0_cut),
            tree_K3pi->GetEntries(cut_K3pi));

    // FDS cut
    TCut prev_cut_Kpi = cut_Kpi;
    TCut prev_cut_K3pi = cut_K3pi;
    cut_Kpi += "D0_FDS > 3";
    cut_K3pi += "D0_FDS > 3";
    print_line(&file, "$\\mathrm{FDS}_{D^0}$ cut",
            tree_Kpi->GetEntries(prev_cut_Kpi),
            tree_Kpi->GetEntries(cut_Kpi),
            tree_K3pi->GetEntries(prev_cut_K3pi),
            tree_K3pi->GetEntries(cut_K3pi));

    // Double mis-ID vetos
    prev_cut_Kpi = cut_Kpi;
    prev_cut_K3pi = cut_K3pi;
    cut_Kpi += "abs(D0_M_doubleSwap - 1864.84) > 15";
    cut_K3pi += "abs(D0_M_doubleSwap_low - 1864.84) > 15";
    cut_K3pi += "abs(D0_M_doubleSwap_high - 1864.84) > 15";
    print_line(&file, "Double mis-ID veto",
            tree_Kpi->GetEntries(prev_cut_Kpi),
            tree_Kpi->GetEntries(cut_Kpi),
            tree_K3pi->GetEntries(prev_cut_K3pi),
            tree_K3pi->GetEntries(cut_K3pi));

    // D(s)h vetoes
    prev_cut_Kpi = cut_Kpi;
    prev_cut_K3pi = cut_K3pi;
    cut_Kpi += "abs(sqrt(pow(KstarK_PE + KstarPi_PE + D0Pi_PE, 2) - "
        "pow(KstarK_PX + KstarPi_PX + D0Pi_PX, 2) - "
        "pow(KstarK_PY + KstarPi_PY + D0Pi_PY, 2) - "
        "pow(KstarK_PZ + KstarPi_PZ + D0Pi_PZ, 2)) - 1869.65) > 15";
    cut_Kpi += "abs(sqrt(pow(KstarK_PE + KstarPi_PE + D0K_PE, 2) - "
        "pow(KstarK_PX + KstarPi_PX + D0K_PX, 2) - "
        "pow(KstarK_PY + KstarPi_PY + D0K_PY, 2) - "
        "pow(KstarK_PZ + KstarPi_PZ + D0K_PZ, 2)) - 1968.34) > 15";
    print_line(&file, "$D_{(s)}h$ veto",
            tree_Kpi->GetEntries(prev_cut_Kpi),
            tree_Kpi->GetEntries(cut_Kpi),
            tree_K3pi->GetEntries(prev_cut_K3pi),
            tree_K3pi->GetEntries(cut_K3pi));

    // D0K veto
    prev_cut_Kpi = cut_Kpi;
    prev_cut_K3pi = cut_K3pi;
    cut_Kpi += "abs(DK_mass - 5279.32) > 25";
    cut_K3pi += "abs(DK_mass - 5279.32) > 25";
    print_line(&file, "$DK$ veto",
            tree_Kpi->GetEntries(prev_cut_Kpi),
            tree_Kpi->GetEntries(cut_Kpi),
            tree_K3pi->GetEntries(prev_cut_K3pi),
            tree_K3pi->GetEntries(cut_K3pi));

    // Final efficiency
    file << "      \\midrule\n";
    print_line(&file, "Full selection",
            N_bkk_Kpi, tree_Kpi->GetEntries(cut_Kpi),
            N_bkk_K3pi, tree_K3pi->GetEntries(cut_K3pi));

    // Finish table
    file << "      \\bottomrule\n";
    file << "  \\end{tabular}\n";
    file << "  \\caption{Breakdown of selection efficiencies for each cut for 2016 MC "
        "in the $K\\pi$ and $K\\pi\\pi\\pi$ modes.}\n";
    file << "\\label{tab:sel_breakdown}\n";
    file << "\\end{table}\n";
    file.close();
}
