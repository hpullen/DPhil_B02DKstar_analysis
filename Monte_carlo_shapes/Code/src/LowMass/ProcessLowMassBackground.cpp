#include "LowMass/ProcessLowMassBackground.hpp"
#include "LowMass/LowMassBackground.hpp"


// ===========
// Constructor
// ===========
ProcessLowMassBackground::ProcessLowMassBackground() {
}


// ==========
// Destructor
// ==========
ProcessLowMassBackground::~ProcessLowMassBackground() {
}


// =================================
// Process backgrounds and apply fit
// =================================
void ProcessLowMassBackground::process() {
    // Add data and fit histograms
    addBackgrounds();

    // Save results to files
    // makeParamsFiles();
}

// ================================
// Make low mass background objects
// ================================
void ProcessLowMassBackground::addBackgrounds() {

    // Locations of ROOT files
    std::string dir = "/data/lhcb/users/pullen/gangadir/ntuples/reduced_ntuples/"
        "With_MVA/Monte_carlo/LowMass/";
    std::string Bd_dir = dir + "Bd/";
    std::string Bs_dir = dir + "Bs/";
    std::string treename_run1 = "TuplePartialReco/DecayTree";
    std::string treename_run2 = "Tuple_Kpi/DecayTree";
    std::string treename = "DecayTree";

    // Bd part. reco. 2016
    std::string pb_pi_001_2016 = Bd_dir + "pi_001_Kpi_run2.root";
    std::string pb_pi_100_2016 = Bd_dir + "pi_100_Kpi_run2.root";
    std::string pb_pi_010_2016 = Bd_dir + "pi_010_Kpi_run2.root";
    std::string pb_gamma_001_2016 = Bd_dir + "gamma_001_Kpi_run2.root";
    std::string pb_gamma_100_2016 = Bd_dir + "gamma_100_Kpi_run2.root"; 
    std::string pb_gamma_010_2016 = Bd_dir + "gamma_010_Kpi_run2.root"; 

    // Bs part. reco. 2016
    std::string pbs_pi_001_2016 = Bs_dir + "pi_001_Kpi_run2.root";
    std::string pbs_pi_100_2016 = Bs_dir + "pi_100_Kpi_run2.root";
    std::string pbs_pi_010_2016 = Bs_dir + "pi_010_Kpi_run2.root";
    std::string pbs_gamma_001_2016 = Bs_dir + "gamma_001_Kpi_run2.root";
    std::string pbs_gamma_100_2016 = Bs_dir + "gamma_100_Kpi_run2.root";
    std::string pbs_gamma_010_2016 = Bs_dir + "gamma_010_Kpi_run2.root";

    // Make LowMassBackground objects
    // Bd
    LowMassBackground * Bd_2016 = new LowMassBackground(2);
    Bd_2016->addData(pb_pi_001_2016, treename, Component::pi_100);
    Bd_2016->addData(pb_pi_100_2016, treename, Component::pi_100);
    Bd_2016->addData(pb_pi_010_2016, treename, Component::pi_010);
    Bd_2016->addData(pb_gamma_001_2016, treename, Component::gamma_100);
    Bd_2016->addData(pb_gamma_100_2016, treename, Component::gamma_100);
    Bd_2016->addData(pb_gamma_010_2016, treename, Component::gamma_010);

    Bd_2016->addData(pbs_pi_001_2016, treename, Component::Bs_pi_100);
    Bd_2016->addData(pbs_pi_100_2016, treename, Component::Bs_pi_100);
    Bd_2016->addData(pbs_pi_010_2016, treename, Component::Bs_pi_010);
    Bd_2016->addData(pbs_gamma_001_2016, treename, Component::Bs_gamma_100);
    Bd_2016->addData(pbs_gamma_100_2016, treename, Component::Bs_gamma_100);
    Bd_2016->addData(pbs_gamma_010_2016, treename, Component::Bs_gamma_010);

    // Fit histograms
    Bd_2016->fitHistograms();

    // Clean up
    delete Bd_2016;
}

