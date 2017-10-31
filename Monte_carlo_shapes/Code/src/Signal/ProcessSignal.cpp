// System includes
#include <fstream>
#include <iostream>

// Local includes
#include "Signal/ProcessSignal.hpp"

// ===========
// Constructor
// ===========
ProcessSignal::ProcessSignal() {
}

// ==========
// Destructor
// ==========
ProcessSignal::~ProcessSignal() {
}

// ====================
// Process all datasets
// ====================
void ProcessSignal::processAll() {
    // Add data and fit histograms
    addBackgrounds();

    // Save results to files
    bool saveEvents = false;
    if (saveEvents) makeNeventsFile();
    makeParamsFiles();
}

// ========================
// Process a single dataset
// ========================
void ProcessSignal::processSingle(std::string name, std::string filepath, 
        std::string treename, BgType bgType, FitShape shape) {

    // Add the background
    addBackground(name, {filepath}, treename, bgType, shape);

    // Save parameters
    makeParamsFiles();
}

// ===========================================
// Add data from ROOT files to dataset objects
// ===========================================
void ProcessSignal::addBackgrounds() {

    // Directory containing merged ntuples
    std::string dir = "/data/lhcb/users/pullen/gangadir/ntuples/merged/";

    // Names of files to use
    // Signal Bd
    std::string signal_2015 = "/data/lhcb/users/pullen/gangadir/ntuples/"
        "reduced_ntuples/With_MVA/Monte_carlo/Signal/signal_2015.root";
    std::string signal_2016 = "/data/lhcb/users/pullen/gangadir/ntuples/"
        "reduced_ntuples/With_MVA/Monte_carlo/Signal/signal_2016.root";

    // Signal Bs
    std::string Bs_2015 = "/data/lhcb/users/pullen/gangadir/ntuples/"
        "reduced_ntuples/With_MVA/Monte_carlo/Signal/Bs_2015.root";
    std::string Bs_2016 = "/data/lhcb/users/pullen/gangadir/ntuples/"
        "reduced_ntuples/With_MVA/Monte_carlo/Signal/Bs_2016.root";

    // Four body
    std::string Kpipipi = "/data/lhcb/users/pullen/gangadir/ntuples/"
        "reduced_ntuples/With_MVA/Monte_carlo/Signal/"
        "Kpipipi_run2_BDTG_Kpipipi_noptasy.root";
    std::string pipipipi = "/data/lhcb/users/pullen/gangadir/ntuples/"
        "reduced_ntuples/With_MVA/Monte_carlo/Signal/"
        "pipipipi_run2_BDTG_pipipipi_noptasy.root";

     // Double crystal ball data fits
     addBackground("signal_CB_run2", {signal_2015, signal_2016}, "DecayTree",
             BgType::Bd_signal, FitShape::DoubleCB);
     addBackground("Bs_CB_run2", {Bs_2015, Bs_2016}, "DecayTree",
             BgType::Bs_signal, FitShape::DoubleCB);

    //// Four body crystal ball fits
    //addBackground("Kpipipi_signal", {Kpipipi}, "DecayTree", BgType::Kpipipi_signal,
            //FitShape::DoubleCB);
    //addBackground("pipipipi_signal", {pipipipi}, "DecayTree", BgType::pipipipi_signal,
            //FitShape::DoubleCB);

}


// =======================
// Add a single background
// =======================
void ProcessSignal::addBackground(std::string name, 
        std::vector<std::string> filepaths, std::string treename, 
        BgType bgType, FitShape shape) {

    // Make SignalData object
    SignalData* bgData = new SignalData(name, bgType, shape);

    // Add data
    for (auto filepath : filepaths) {
        bgData->addData(filepath, treename);
    }

    // Fit the histogram
    bgData->fitHistogram(name);

    // Get parameters and add to map
    m_params.emplace(name, bgData->getParameters());
    m_events.emplace(name, bgData->getEvents());

    // Delete the object
    delete bgData;
}

// =============================================
// Create file with number of each type of event
// =============================================
void ProcessSignal::makeNeventsFile() {
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
void ProcessSignal::makeParamsFiles() {
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
