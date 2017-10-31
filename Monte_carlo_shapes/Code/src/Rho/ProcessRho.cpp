// System includes
#include <fstream>
#include <iostream>

// Local includes
#include "Rho/RhoBackground.hpp"
#include "Rho/ProcessRho.hpp"

// ===========
// Constructor
// ===========
ProcessRho::ProcessRho() {
}

// ==========
// Destructor
// ==========
ProcessRho::~ProcessRho() {
}

// ====================
// Process all datasets
// ====================
void ProcessRho::processAll() {
    // Add data and fit histograms
    addBackgrounds();

    // Save results to files
    bool saveEvents = false;
    if (saveEvents) makeNeventsFile();
    makeParamsFiles();
}

// ===========================================
// Add data from ROOT files to dataset objects
// ===========================================
void ProcessRho::addBackgrounds() {

    // Directory containing merged ntuples
    std::string dir = "/data/lhcb/users/pullen/gangadir/ntuples/merged/";

    // Names of files to use
    std::string rho_file = dir + "rho0/rho0_2016_PIDGen.root";
    std::string rho_tree = "DecayTree";

    // Make RhoBackground objects
    // Without PID cuts
    RhoBackground * rho_noPID = new RhoBackground("rho_noPID", false);
    rho_noPID->addData(rho_file, rho_tree);
    rho_noPID->fitHistogram("rho_run2");
    m_params.emplace("rho_noPID", rho_noPID->getParameters());
    delete rho_noPID;

    // With PID cuts
    RhoBackground * rho_allPID = new RhoBackground("rho_allPID", true);
    rho_allPID->addData(rho_file, rho_tree);
    rho_allPID->fitHistogram("rho_run2");
    m_params.emplace("rho_allPID", rho_allPID->getParameters());
    delete rho_allPID;
}


// =============================================
// Create file with number of each type of event
// =============================================
void ProcessRho::makeNeventsFile() {
    // Open file for nEvents
    std::ofstream eventsFile;
    eventsFile.open("../Results/number_of_events.txt");

    // Iterate through map of event numbers and print to file
    for (auto entry : m_events) {
        eventsFile << entry.first << " ";
        eventsFile << entry.second << std::endl;
    }

    // Close file
    eventsFile.close();
}

// =========================================
// Create files with parameters for each fit
// =========================================
void ProcessRho::makeParamsFiles() {
    // Iterate through parameters map and save file for each set
    for (auto entry : m_params) {
        // Open output file
        std::ofstream paramFile;
        paramFile.open("../Results/" + entry.first + ".param");

        // Get parameters
        auto parameters = entry.second;

        // Iterate through map and print parameters
        for (auto param : parameters) {
            // Print parameter
            paramFile << param.first << " " << param.second << std::endl;
        }
    }
}
