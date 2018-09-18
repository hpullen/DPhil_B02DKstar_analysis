// ================================================
// Make string of invariant mass of three particles
// ================================================
TString inv_mass_string(TString d1, TString d2, TString d3) {

    TString str = "sqrt(pow(" + d1 + "_PE + " + d2 + "_PE + " + d3 + "_PE, 2)";
    str += " - pow(" + d1 + "_PX + " + d2 + "_PX + " + d3 + "_PX, 2)";
    str += " - pow(" + d1 + "_PY + " + d2 + "_PY + " + d3 + "_PY, 2)";
    str += " - pow(" + d1 + "_PZ + " + d2 + "_PZ + " + d3 + "_PZ, 2))";
    return str;

}



// =======================================
// Function to get efficiency for one mode
// =======================================
double get_efficiency(TString mode) {

    // Load selected Run 2 MC
    TChain * tree = new TChain("DecayTree");
    TString dir = "/data/lhcb/users/pullen/B02DKstar/MC/twoBody/";
    TString mc_mode = (mode == "piK") ? "Kpi" : mode;
    for (TString mag : {"up", "down"}) {
        tree->Add(dir + mc_mode + "/2016_" + mag + "/" + mc_mode + "_selected.root");
        if (mc_mode == "Kpi") {
            tree->Add(dir + mc_mode + "/2015_" + mag + "/" + mc_mode + "_selected.root");
        }
    }

    // Masses 
    TString m_D = "1869.65";
    TString m_Ds = "1968.34";

    // Get cut
    TString d1 = "KstarK";
    TString d2 = "KstarPi";
    TString cut;
    
    if (mc_mode == "Kpi") {

        // Kpi/piK: no need to cut based on flavour
        TString d3 = (mode == "Kpi") ? "D0Pi" : "D0K";
        TString mass = (mode == "Kpi") ? m_D : m_Ds;
        cut = "abs(" + inv_mass_string(d1, d2, d3) + " - " + mass + ") > 15";

    } else {

        // KK/pipi: cut depends on sign
        TString d3_plus = (mode == "KK") ? "D0Kminus" : "D0PiMinus";
        TString d3_minus = (mode == "KK") ? "D0Kplus" : "D0PiPlus";
        TString mass = (mode == "pipi") ? m_D : m_Ds;
        TString cut1 = "KstarK_TRUEID > 0 && abs(" + inv_mass_string(d1, d2, d3_plus) + " - " + mass + ") > 15";
        TString cut2 = "KstarK_TRUEID < 0 && abs(" + inv_mass_string(d1, d2, d3_minus) + " - " + mass + ") > 15";
        cut = cut1 + " || " + cut2;
    }

    // Get efficiency
    double before = tree->GetEntries();
    double after = tree->GetEntries(cut);
    return after/before;
}



// =======
// Get all
// =======
void get_veto_efficiencies() {

    std::cout << "Kpi: " << get_efficiency("Kpi") << std::endl;
    std::cout << "piK: " << get_efficiency("piK") << std::endl;
    std::cout << "KK: " << get_efficiency("KK") << std::endl;
    std::cout << "pipi: " << get_efficiency("pipi") << std::endl;

}
