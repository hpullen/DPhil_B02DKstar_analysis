#include <iostream>
#include <fstream>
#include <math.h>

#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVA/Tools.h"
#include "TMVA/MethodBDT.h"

#include "MVAApplicator.hpp"


// ===========
// Constructor
// ===========
MVAApplicator::MVAApplicator() {
}


// ==========
// Destructor
// ==========
MVAApplicator::~MVAApplicator() {
}


// =================================
// Apply classification to a dataset
// =================================
void MVAApplicator::apply(std::string inputPath, std::string outputName, 
        std::string mvaName, std::string treename, std::string varFile) {

    // Load the library and create reader
    TMVA::Tools::Instance();
    TMVA::Reader * reader = new TMVA::Reader("!Color:!Silent");

    // Set up MVA
    std::map<std::string, Float_t> mva_vars_map;
    setupMVAVariables(reader, varFile, &mva_vars_map);
    bookMVA(reader, mvaName);
    std::cout << "Set up MVA." << std::endl;

    // Prepare input tree
    TFile * inputFile = TFile::Open(inputPath.c_str());
    TTree * inputTree = (TTree*)inputFile->Get(treename.c_str());
    std::map<std::string, double> data_raw_vars_map;
    std::map<std::string, float> data_floats_map;
    setupDataVariables(inputTree, varFile, &data_raw_vars_map, &data_floats_map);

    // Set up output tree
    TFile * outputFile = TFile::Open(outputName.c_str(), "RECREATE");
    TTree * outputTree = inputTree->CloneTree(0);
    Float_t mva_response;
    outputTree->Branch(("BDTG_" + mvaName).c_str(), &mva_response);

    // Evaluate MVA and add to tree
    evaluateMVA(reader, inputTree, outputTree, mvaName, &mva_response,
            0, -1, &mva_vars_map, &data_raw_vars_map, &data_floats_map);

    // // Save new tree
    outputTree->Write();
    outputFile->Close();
    std::cout << "New TTree with MVA " << mvaName << " applied saved to " <<
        outputName << "." << std::endl;
    delete reader;
}


// ==========================================================
// Apply classification to a dataset with limited event range
// ==========================================================
void MVAApplicator::apply(std::string inputPath, std::string outputName, 
        std::string mvaName, std::string treename, std::string varFile,
        int evtMin, int evtMax) {

    // Load the library and create reader
    TMVA::Tools::Instance();
    TMVA::Reader * reader = new TMVA::Reader("!Color:!Silent");

    // Set up MVA
    std::map<std::string, Float_t> mva_vars_map;
    setupMVAVariables(reader, varFile, &mva_vars_map);
    bookMVA(reader, mvaName);
    std::cout << "Set up MVA." << std::endl;

    // Prepare input tree
    TFile * inputFile = TFile::Open(inputPath.c_str());
    TTree * inputTree = (TTree*)inputFile->Get(treename.c_str());
    std::map<std::string, double> data_raw_vars_map;
    std::map<std::string, float> data_floats_map;
    setupDataVariables(inputTree, varFile, &data_raw_vars_map, &data_floats_map);

    // Set up output tree
    TFile * outputFile = TFile::Open((outputName + "_" + std::to_string(evtMin) + 
                "_to_" + std::to_string(evtMax) + ".root").c_str(), "RECREATE");
    TTree * outputTree = inputTree->CloneTree(0);
    Float_t mva_response;
    outputTree->Branch(("BDTG_" + mvaName).c_str(), &mva_response);

    // Evaluate MVA and add to tree
    std::cout << "Applying MVA to events " << evtMin << " - " << evtMax << std::endl;
    evaluateMVA(reader, inputTree, outputTree, mvaName, &mva_response,
            evtMin, evtMax, &mva_vars_map, &data_raw_vars_map, &data_floats_map);

    // // Save new tree
    outputTree->Write();
    outputFile->Close();
    std::cout << "New TTree with MVA " << mvaName << " applied saved to " <<
        outputName << "." << std::endl;
    delete reader;
}


// ============================================================
// Apply classification to dataset and calculate some variables
// ============================================================
void MVAApplicator::apply(std::string mode, std::string inputPath, 
        std::string outputName) {

    // Set BDT name and variable file
    std::string bdt_mode = mode;
    if (mode == "piK") bdt_mode = "Kpi";
    else if (mode == "piKpipi") bdt_mode = "Kpipipi";
    std::string mvaName = bdt_mode + "_run2";
    std::string varFile = "/home/pullen/analysis/B02DKstar/BDT/VariableLists/" 
        + mvaName + ".txt";
    std::string treename = "DecayTree";

    // Load the library and create reader
    TMVA::Tools::Instance();
    TMVA::Reader * reader = new TMVA::Reader("!Color:!Silent");

    // Set up MVA
    std::map<std::string, Float_t> mva_vars_map;
    setupMVAVariables(reader, varFile, &mva_vars_map);
    bookMVA(reader, mvaName);
    std::cout << "Set up MVA." << std::endl;

    // Prepare input tree
    TFile * inputFile = TFile::Open(inputPath.c_str());
    TTree * inputTree = (TTree*)inputFile->Get(treename.c_str());
    std::map<std::string, double> data_raw_vars_map;
    std::map<std::string, float> data_floats_map;
    setupDataVariables(inputTree, varFile, &data_raw_vars_map, &data_floats_map);

    // Set up output tree
    TFile * outputFile = TFile::Open(outputName.c_str(), "RECREATE");
    TTree * outputTree = inputTree->CloneTree(0);
    Float_t mva_response;
    outputTree->Branch(("BDTG_" + mvaName).c_str(), &mva_response);

    // Evaluate MVA and add to tree
    evaluateMVAandCalculateVars(reader, inputTree, outputTree, mvaName, mode, 
            &mva_response, 0, -1, &mva_vars_map, &data_raw_vars_map, 
            &data_floats_map);

     // Save new tree
    outputTree->Write();
    outputFile->Close();
    std::cout << "New TTree with MVA " << mvaName << " applied saved to " <<
        outputName << "." << std::endl;
    delete reader;
}



// =====================
// Set up MVA variables 
// =====================
void  MVAApplicator::setupMVAVariables(TMVA::Reader * reader, 
        std::string varFile, std::map<std::string, Float_t> * vars_map) {

    // Open variable file
    std::ifstream file(varFile);
    std::string line;

    // Read variables from file and add to list and MVA reader
    while (std::getline(file, line)) {
        reader->AddVariable(line.c_str(), &(*vars_map)[line]);
    }
}


// =============================
// Setup variables in data tuple
// =============================
void MVAApplicator::setupDataVariables(TTree * tree, std::string varFile,
        std::map<std::string, double> * vars_map,
        std::map<std::string, float> * floats_map) {

    // Open file
    std::ifstream file(varFile);
    std::string line;

    // Read each line. Check whether an operation is applied, and put
    // raw variable in data variables map
    while (std::getline(file, line)) {

        // Check for daughter names
        std::string d1 = "Kplus";
        std::string d2 = "piplus";
        if (varFile.find("KK") != std::string::npos) {
            d2 = "Kplus_0";
        } else if (varFile.find("pipi") != std::string::npos) {
            d1 = "piplus_0";
        }

        // Check for functions involving multiple raw variables
        if (line.find(":=") != std::string::npos) {
            std::vector<std::string> raw_vars_to_set;
            if(line.find("log10_Bd_ConsD_chi2_ndof") != std::string::npos) {
                raw_vars_to_set.push_back("Bd_ConsD_chi2");
                raw_vars_to_set.push_back("Bd_ConsD_nDOF");
            } else if (line.find("log10_Bd_ConsD_D0_PT") != std::string::npos) {
                raw_vars_to_set.push_back("Bd_ConsD_D0_" + d1 + "_PX");
                raw_vars_to_set.push_back("Bd_ConsD_D0_" + d1 + "_PY");
                raw_vars_to_set.push_back("Bd_ConsD_D0_" + d2 + "_PX");
                raw_vars_to_set.push_back("Bd_ConsD_D0_" + d2 + "_PY");
            } else if (line.find("log10_Bd_ConsD_PT") != std::string::npos) {
                raw_vars_to_set.push_back("Bd_ConsD_D0_" + d1 + "_PX");
                raw_vars_to_set.push_back("Bd_ConsD_D0_" + d1 + "_PY");
                raw_vars_to_set.push_back("Bd_ConsD_D0_" + d2 + "_PX");
                raw_vars_to_set.push_back("Bd_ConsD_D0_" + d2 + "_PY");
                raw_vars_to_set.push_back("Bd_ConsD_Kst_892_0_Kplus_PX");
                raw_vars_to_set.push_back("Bd_ConsD_Kst_892_0_Kplus_PY");
                raw_vars_to_set.push_back("Bd_ConsD_Kst_892_0_piplus_PX");
                raw_vars_to_set.push_back("Bd_ConsD_Kst_892_0_piplus_PY");
            } else {
                std::cout << "Possible error: found unknown function alias in "
                    "variable list." << std::endl;
            }

            // Loop over raw variables needed and add them
            for (auto var : raw_vars_to_set) {
                if (!vars_map->count(var)) {
                    std::cout << "Setting branch address for variable: " <<
                        var << std::endl;
                    tree->SetBranchAddress(var.c_str(), &(*floats_map)[var]);
                }
            }
            continue;
        }

        // Set up a single raw variable
        std::string raw_varname = line;
        
        // Check for keywords log10 and acos
        if (line.find("log10") != std::string::npos || line.find("acos") != 
                std::string::npos) {
            unsigned first = line.find_first_of("(");
            unsigned last = line.find_last_of(")");
            raw_varname = line.substr(first + 1, last - first - 1);
        }

        std::cout << "Setting branch address for variable " << raw_varname 
            << std::endl;
        
        // Set up as float if needed
        if (raw_varname.find("Bd_ConsD") != std::string::npos) {
            tree->SetBranchAddress(raw_varname.c_str(), &(*floats_map)[line]);
        } else {
            tree->SetBranchAddress(raw_varname.c_str(), &(*vars_map)[line]);
        }
    }
}


// =========
// Book MVAs
// =========
void MVAApplicator::bookMVA(TMVA::Reader * reader, std::string mvaName) {
    
    std::string weights_file = "./weights/TMVAClassification_BDTG_" + mvaName + 
        ".weights.xml";
    reader->BookMVA(("BDTG_" + mvaName).c_str(), weights_file.c_str());
}


// ============================================
// Evaluate MVA and add response to output tree
// ============================================
void MVAApplicator::evaluateMVA(TMVA::Reader * reader, TTree * inputTree, 
        TTree * outputTree, std::string mvaName, Float_t * response,
        int evtMin, int evtMax,
        std::map<std::string, Float_t> * mva_vars,
        std::map<std::string, double> * data_vars,
        std::map<std::string, float> * float_vars) {

    // Loop through tree
    int error_entries = 0;
    int less_than_zero_entries = 0;
    std::cout << "Looping over " << inputTree->GetEntries() << " entries." << 
        std::endl;

    // Check evtMax
    Long64_t n_entries = inputTree->GetEntries();
    if (evtMax == -1) {
        std::cout << "Using all " << n_entries << "entries" << std::endl;
        evtMax = n_entries;
    }
    if (evtMax > n_entries) {
        std::cout << "Maximum entry specified is greater than number of entries."
            " Will only go up to entry" << n_entries << std::endl;
        evtMax = n_entries;
    }

    for (Long64_t ievt = evtMin; ievt < evtMax; ievt++) {

        // Get current event
        inputTree->GetEntry(ievt);

        // Assign each of the variables
        bool ipchi2_less_than_zero = false;
        for (auto var : (*mva_vars)) {

            // Get name of variable
            std::string varname = var.first;

            // Check for IPCHI2 values less than zero 
            if (varname.find("IPCHI2") != std::string::npos && 
                    varname.find("Bd") == std::string::npos) {
                if ((*data_vars)[varname] < 0) {
                    ipchi2_less_than_zero = true;
                    break;
                }
            }

            // Check for functions containing multiple raw variables
            if (varname.find(":=") != std::string::npos) {
                if (varname.find("log10_Bd_ConsD_chi2_ndof") != std::string::npos) {
                    float chi2 = (*float_vars)["Bd_ConsD_chi2"];
                    float ndof = (*float_vars)["Bd_ConsD_nDOF"];
                    (*mva_vars)[varname] = (Float_t)log10(chi2 / ndof);
                } else if (varname.find("log10_Bd_ConsD_D0_PT") !=
                        std::string::npos) {
                    float K_PX = (*float_vars)["Bd_ConsD_D0_Kplus_PX"];
                    float K_PY = (*float_vars)["Bd_ConsD_D0_Kplus_PY"];
                    float pi_PX = (*float_vars)["Bd_ConsD_D0_piplus_PX"];
                    float pi_PY = (*float_vars)["Bd_ConsD_D0_piplus_PY"];
                    (*mva_vars)[varname] = (Float_t)log10(sqrt(pow(K_PX + pi_PX, 2) 
                                + pow(K_PY + pi_PY, 2)));
                } else if (varname.find("log10_Bd_ConsD_PT") != 
                        std::string::npos) {
                    float D0_K_PX = (*float_vars)["Bd_ConsD_D0_Kplus_PX"];
                    float D0_K_PY = (*float_vars)["Bd_ConsD_D0_Kplus_PY"];
                    float D0_pi_PX = (*float_vars)["Bd_ConsD_D0_piplus_PX"];
                    float D0_pi_PY = (*float_vars)["Bd_ConsD_D0_piplus_PY"];
                    float Kstar_K_PX = 
                        (*float_vars)["Bd_ConsD_Kst_892_0_Kplus_PX"];
                    float Kstar_K_PY = 
                        (*float_vars)["Bd_ConsD_Kst_892_0_Kplus_PY"];
                    float Kstar_pi_PX = 
                        (*float_vars)["Bd_ConsD_Kst_892_0_piplus_PX"];
                    float Kstar_pi_PY = 
                        (*float_vars)["Bd_ConsD_Kst_892_0_piplus_PY"];
                    (*mva_vars)[varname] = (Float_t)log10(sqrt(pow(D0_K_PX + 
                                    D0_pi_PX + Kstar_K_PX + Kstar_pi_PX, 2) +
                                pow(D0_K_PY + D0_pi_PY + Kstar_K_PY + 
                                    Kstar_pi_PY, 2)));
                }
                continue;
            }

            // Check for keywords acos and log10 and assign value to MVA variable
            if (varname.find("Bd_ConsD") == std::string::npos) {
                if (varname.find("acos") != std::string::npos) {
                    (*mva_vars)[varname] = (Float_t)acos((*data_vars)[varname]); 
                } else if (varname.find("log10") != std::string::npos) {
                    (*mva_vars)[varname] = (Float_t)log10((*data_vars)[varname]);
                } else {
                    (*mva_vars)[varname] = (Float_t)(*data_vars)[varname];
                }
            } else {
                // Deal with floats separately
                if (varname.find("acos") != std::string::npos) {
                    (*mva_vars)[varname] = (Float_t)acos((*float_vars)[varname]); 
                } else if (varname.find("log10") != std::string::npos) {
                    (*mva_vars)[varname] = (Float_t)log10((*float_vars)[varname]);
                } else {
                    (*mva_vars)[varname] = (Float_t)(*float_vars)[varname];
                }
            }
        }

        // Print info
        if (ievt % 10000 == 0) {
            std::cout << "Event number: " << ievt << std::endl;
            for (auto var : *mva_vars) {
                std::cout << var.first << ": " << var.second << std::endl;
            }
            std::cout << std::endl;
        }

        // Skip if any IPCHI2 was less than zero
        if (ipchi2_less_than_zero) {
            less_than_zero_entries++;
            continue;
        }

        // Evaluate MVA for this event
        *response = reader->EvaluateMVA(("BDTG_" + mvaName).c_str());

        // Check for error; fill tree if no error
        if (*response == -999) {
            error_entries++;
        } else {
            outputTree->Fill();
        }
    }

    // Print info
    std::cout << "Applied MVA " << mvaName << " to tree." << std::endl;
    std::cout << "New tree has " << outputTree->GetEntries() << " entries." <<
        std::endl;
    std::cout << "Skipped " << less_than_zero_entries << " entries with IPCHI2 < 0."
        << std::endl;
    std::cout << "Found " << error_entries << " entries with error status." 
        << std::endl;
}


// ==================================================
// Evaluate the MVA and also calculate some variables
// ==================================================
void MVAApplicator::evaluateMVAandCalculateVars(TMVA::Reader * reader, 
        TTree * inputTree, TTree * outputTree, std::string mvaName, std::string mode,
        Float_t * response, int evtMin, int evtMax,
        std::map<std::string, Float_t> * mva_vars,
        std::map<std::string, double> * data_vars,
        std::map<std::string, float> * float_vars) {

    // Set up variables for calculations
    // See if mode is an ADS mode
    bool is_ADS = (mode == "Kpi" || mode == "piK" || mode == "Kpipipi" ||
            mode == "piKpipi");
    // See if four body
    bool is_fourBody = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi");

    // Variables needed for D0 flight distance calculation
    double D0_ENDVERTEX_Z;
    double D0_ENDVERTEX_ZERR;
    double Bd_ENDVERTEX_Z;
    double Bd_ENDVERTEX_ZERR;
    inputTree->SetBranchAddress("D0_ENDVERTEX_Z", &D0_ENDVERTEX_Z);
    inputTree->SetBranchAddress("D0_ENDVERTEX_ZERR", &D0_ENDVERTEX_ZERR);
    inputTree->SetBranchAddress("Bd_ENDVERTEX_Z", &Bd_ENDVERTEX_Z);
    inputTree->SetBranchAddress("Bd_ENDVERTEX_ZERR", &Bd_ENDVERTEX_ZERR);

    // New branches to hold D0 FD
    double D0_FD;
    double D0_FD_ERR;
    double D0_FDS;
    outputTree->Branch("D0_FD", &D0_FD, "D0_FD/D");
    outputTree->Branch("D0_FD_ERR", &D0_FD_ERR, "D0_FD_ERR/D");
    outputTree->Branch("D0_FDS", &D0_FDS, "D0_FDS/D");

    // DK invariant mass
    TLorentzVector v_D0_P;
    TLorentzVector v_KstarK_P;
    LorentzVectorBranch(inputTree, v_D0_P, "D0");
    LorentzVectorBranch(inputTree, v_KstarK_P, "KstarK");
    double DK_mass;
    outputTree->Branch("DK_mass", &DK_mass, "DK_mass/D");

    // EventNumber cast as a double
    unsigned long long eventNumber;
    double eventNumberD;
    inputTree->SetBranchAddress("eventNumber", &eventNumber);
    outputTree->Branch("eventNumberD", &eventNumberD, "eventNumberD/D");

    // Magentic polarity cast as a double
    short polarity;
    double polarityD;
    inputTree->SetBranchAddress("Polarity", &polarity);
    outputTree->Branch("PolarityD", &polarityD, "PolarityD/D");

    // B0 decaytreefitter mass cast as a double
    float Bd_ConsD_M;
    double Bd_ConsD_MD;
    inputTree->SetBranchAddress("Bd_ConsD_M", &Bd_ConsD_M);
    outputTree->Branch("Bd_ConsD_MD", &Bd_ConsD_MD, "Bd_ConsD_MD/D");

    // Variables needed for double mass swap calculation
    const double mass_pi = 139.57;
    const double mass_K = 493.68;
    TLorentzVector v_D0K_P, v_D0Pi_P;

    // New vectors and variable for results
    TLorentzVector v_K_as_pi, v_pi_as_K, v_D0_doubleSwap;
    double D0_M_doubleSwap;
    double D0_deltaM_doubleSwap;
    const double D0_M_PDG = 1864.84;
    
    // Variables needed for 4-body double mass swap calculation 
    TLorentzVector v_D0PiPlus_P, v_D0PiMinus_P;
    int KstarK_ID;

    // New vectors and variable for results
    TLorentzVector v_low_pi_as_K, v_high_pi_as_K, v_D0_doubleSwap_low, 
                   v_D0_doubleSwap_high;
    double D0_M_doubleSwap_low;
    double D0_deltaM_doubleSwap_low;
    double D0_M_doubleSwap_high;
    double D0_deltaM_doubleSwap_high;

    // Set up the branches if mode is an ADS mode
    if (is_ADS) {
        LorentzVectorBranch(inputTree, v_D0K_P, "D0K");
        LorentzVectorBranch(inputTree, v_D0Pi_P, "D0Pi");
        if (!is_fourBody) {
            outputTree->Branch("D0_M_doubleSwap", &D0_M_doubleSwap, "D0_M_doubleSwap/D");
            outputTree->Branch("D0_deltaM_doubleSwap", &D0_deltaM_doubleSwap, 
                    "D0_deltaM_deltadoubleSwap/D");
        } else {
            LorentzVectorBranch(inputTree, v_D0PiPlus_P, "D0PiPlus");
            LorentzVectorBranch(inputTree, v_D0PiMinus_P, "D0PiMinus");
            if (inputTree->GetListOfBranches()->Contains("KstarK_TRUEID")) {
                inputTree->SetBranchAddress("KstarK_TRUEID", &KstarK_ID);
            } else {
                inputTree->SetBranchAddress("KstarK_ID", &KstarK_ID);
            }
            outputTree->Branch("D0_M_doubleSwap_low", &D0_M_doubleSwap_low, 
                    "D0_M_doubleSwap_low/D");
            outputTree->Branch("D0_deltaM_doubleSwap_low", &D0_deltaM_doubleSwap_low, 
                    "D0_deltaM_doubleSwap_low/D");
            outputTree->Branch("D0_M_doubleSwap_high", &D0_M_doubleSwap_high, 
                    "D0_M_doubleSwap_high/D");
            outputTree->Branch("D0_deltaM_doubleSwap_high", &D0_deltaM_doubleSwap_high, 
                    "D0_deltaM_doubleSwap_high/D");
        }
    }

    // ETA and P branches for high and low pion
    double D0Pi_low_P, D0Pi_high_P;
    double D0Pi_low_ETA, D0Pi_high_ETA;
    double D0Pi_ETA, D0PiPlus_ETA, D0PiMinus_ETA;
    if (is_ADS && is_fourBody) {

        // P branches
        outputTree->Branch("D0Pi_low_P", &D0Pi_low_P, "D0Pi_low_P/D");
        outputTree->Branch("D0Pi_high_P", &D0Pi_high_P, "D0Pi_high_P/D");

        // Check name of ETA branch
        TString eta = "_ETA";
        if (!inputTree->GetListOfBranches()->Contains("D0Pi_ETA")) {
            eta = "_LOKI_ETA";
        }

        // Set ETA branches
        inputTree->SetBranchAddress("D0Pi" + eta, &D0Pi_ETA);
        inputTree->SetBranchAddress("D0PiPlus" + eta, &D0PiPlus_ETA);
        inputTree->SetBranchAddress("D0PiMinus" + eta, &D0PiMinus_ETA);
        outputTree->Branch("D0Pi_low" + eta, &D0Pi_low_ETA, "D0Pi_low" + eta + "/D");
        outputTree->Branch("D0Pi_high" + eta, &D0Pi_high_ETA, "D0Pi_high" + eta + "/D");
    }

    // Loop through tree
    int error_entries = 0;
    int less_than_zero_entries = 0;
    std::cout << "Looping over " << inputTree->GetEntries() << " entries." << 
        std::endl;

    // Check evtMax
    Long64_t n_entries = inputTree->GetEntries();
    if (evtMax == -1) {
        std::cout << "Using all " << n_entries << "entries" << std::endl;
        evtMax = n_entries;
    }
    if (evtMax > n_entries) {
        std::cout << "Maximum entry specified is greater than number of entries."
            " Will only go up to entry" << n_entries << std::endl;
        evtMax = n_entries;
    }

    for (Long64_t ievt = evtMin; ievt < evtMax; ievt++) {

        // Get current event
        inputTree->GetEntry(ievt);

        // Assign each of the variables
        bool ipchi2_less_than_zero = false;
        for (auto var : (*mva_vars)) {

            // Get name of variable
            std::string varname = var.first;

            // Check for IPCHI2 values less than zero 
            if (varname.find("IPCHI2") != std::string::npos && 
                    varname.find("Bd") == std::string::npos) {
                if ((*data_vars)[varname] < 0) {
                    ipchi2_less_than_zero = true;
                    break;
                }
            }

            // Check for functions containing multiple raw variables
            if (varname.find(":=") != std::string::npos) {
                if (varname.find("log10_Bd_ConsD_chi2_ndof") != std::string::npos) {
                    float chi2 = (*float_vars)["Bd_ConsD_chi2"];
                    float ndof = (*float_vars)["Bd_ConsD_nDOF"];
                    (*mva_vars)[varname] = (Float_t)log10(chi2 / ndof);
                } else if (varname.find("log10_Bd_ConsD_D0_PT") !=
                        std::string::npos) {
                    float K_PX = (*float_vars)["Bd_ConsD_D0_Kplus_PX"];
                    float K_PY = (*float_vars)["Bd_ConsD_D0_Kplus_PY"];
                    float pi_PX = (*float_vars)["Bd_ConsD_D0_piplus_PX"];
                    float pi_PY = (*float_vars)["Bd_ConsD_D0_piplus_PY"];
                    (*mva_vars)[varname] = (Float_t)log10(sqrt(pow(K_PX + pi_PX, 2) 
                                + pow(K_PY + pi_PY, 2)));
                } else if (varname.find("log10_Bd_ConsD_PT") != 
                        std::string::npos) {
                    float D0_K_PX = (*float_vars)["Bd_ConsD_D0_Kplus_PX"];
                    float D0_K_PY = (*float_vars)["Bd_ConsD_D0_Kplus_PY"];
                    float D0_pi_PX = (*float_vars)["Bd_ConsD_D0_piplus_PX"];
                    float D0_pi_PY = (*float_vars)["Bd_ConsD_D0_piplus_PY"];
                    float Kstar_K_PX = 
                        (*float_vars)["Bd_ConsD_Kst_892_0_Kplus_PX"];
                    float Kstar_K_PY = 
                        (*float_vars)["Bd_ConsD_Kst_892_0_Kplus_PY"];
                    float Kstar_pi_PX = 
                        (*float_vars)["Bd_ConsD_Kst_892_0_piplus_PX"];
                    float Kstar_pi_PY = 
                        (*float_vars)["Bd_ConsD_Kst_892_0_piplus_PY"];
                    (*mva_vars)[varname] = (Float_t)log10(sqrt(pow(D0_K_PX + 
                                    D0_pi_PX + Kstar_K_PX + Kstar_pi_PX, 2) +
                                pow(D0_K_PY + D0_pi_PY + Kstar_K_PY + 
                                    Kstar_pi_PY, 2)));
                }
                continue;
            }

            // Check for keywords acos and log10 and assign value to MVA variable
            if (varname.find("Bd_ConsD") == std::string::npos) {
                if (varname.find("acos") != std::string::npos) {
                    (*mva_vars)[varname] = (Float_t)acos((*data_vars)[varname]); 
                } else if (varname.find("log10") != std::string::npos) {
                    (*mva_vars)[varname] = (Float_t)log10((*data_vars)[varname]);
                } else {
                    (*mva_vars)[varname] = (Float_t)(*data_vars)[varname];
                }
            } else {
                // Deal with floats separately
                if (varname.find("acos") != std::string::npos) {
                    (*mva_vars)[varname] = (Float_t)acos((*float_vars)[varname]); 
                } else if (varname.find("log10") != std::string::npos) {
                    (*mva_vars)[varname] = (Float_t)log10((*float_vars)[varname]);
                } else {
                    (*mva_vars)[varname] = (Float_t)(*float_vars)[varname];
                }
            }
        }

        // Skip if any IPCHI2 was less than zero
        if (ipchi2_less_than_zero) {
            less_than_zero_entries++;
            continue;
        }

        // Evaluate MVA for this event
        *response = reader->EvaluateMVA(("BDTG_" + mvaName).c_str());

        // Skip if BDT response is < -0.5 to reduce tuple size
        // if(*response < -0.5) continue;
        if (*response == -999) {
            error_entries++;
            continue;
        }

        // Calculate extra variables
        eventNumberD = (double)eventNumber;
        polarityD = (double)polarity;
        Bd_ConsD_MD = (double)Bd_ConsD_M;

        // Work out D0 flight distance variables
        D0_FD = D0_ENDVERTEX_Z - Bd_ENDVERTEX_Z;
        D0_FD_ERR = sqrt(pow(D0_ENDVERTEX_ZERR, 2) + pow(Bd_ENDVERTEX_ZERR, 2));
        D0_FDS = D0_FD / D0_FD_ERR;

        // Work out DK invariant mass
        DK_mass = (v_D0_P + v_KstarK_P).M();

        // Work out double swapped mass for ADS modes
        if (is_ADS) {

            // Calculate vector for kaon with pi mass
            v_K_as_pi.SetPxPyPzE(v_D0K_P[0], v_D0K_P[1], v_D0K_P[2],
                    sqrt(pow(v_D0K_P.P(), 2) + pow(mass_pi, 2)));

            // Two body: swap D0 daughter kaon and pion masses
            if (!is_fourBody) {

                // Calculate vectors for pion with K mass
                v_pi_as_K.SetPxPyPzE(v_D0Pi_P[0], v_D0Pi_P[1], v_D0Pi_P[2],
                        sqrt(pow(v_D0Pi_P.P(), 2) + pow(mass_K, 2)));

                // Work out double swapped mass
                v_D0_doubleSwap = v_K_as_pi + v_pi_as_K;
                D0_M_doubleSwap = v_D0_doubleSwap.M();
                D0_deltaM_doubleSwap = std::abs(D0_M_doubleSwap - D0_M_PDG);

            }

            // Four body: swap D0 daughter kaon with both opposite-sign pions
            if (is_fourBody) {

                // Work out which pion to use
                // D0K ID > 0: K+. Want to use pi-.
                // D0K ID < 0: K-. Want to use pi+.
                TLorentzVector * v_piToSwap;
                TLorentzVector * v_piNoSwap;
                if ((mode == "Kpipipi" && KstarK_ID < 0) || 
                        (mode == "piKpipi" && KstarK_ID > 0)) {
                    v_piToSwap = &v_D0PiPlus_P;
                    v_piNoSwap = &v_D0PiMinus_P;
                } else {
                    v_piToSwap = &v_D0PiMinus_P;
                    v_piNoSwap = &v_D0PiPlus_P;
                }

                // Distinguish high and low momentum pions
                TLorentzVector * v_D0Pi_low = (v_D0Pi_P.P() < v_piToSwap->P()) 
                    ? &v_D0Pi_P : v_piToSwap;
                TLorentzVector * v_D0Pi_high = (v_D0Pi_P.P() > v_piToSwap->P()) 
                    ? &v_D0Pi_P : v_piToSwap;

                // Work out vectors for pions with K mass
                v_low_pi_as_K.SetPxPyPzE(v_D0Pi_low->Px(), v_D0Pi_low->Py(), v_D0Pi_low->Pz(),
                        sqrt(pow(v_D0Pi_low->P(), 2) + pow(mass_K, 2)));
                v_high_pi_as_K.SetPxPyPzE(v_D0Pi_high->Px(), v_D0Pi_high->Py(), v_D0Pi_high->Pz(),
                        sqrt(pow(v_D0Pi_high->P(), 2) + pow(mass_K, 2)));

                // Work out double swapped masses
                v_D0_doubleSwap_low = v_K_as_pi + v_low_pi_as_K +
                    *v_D0Pi_high + *v_piNoSwap;
                v_D0_doubleSwap_high = v_K_as_pi + v_high_pi_as_K +
                    *v_D0Pi_low + *v_piNoSwap;
                D0_M_doubleSwap_low = v_D0_doubleSwap_low.M();
                D0_M_doubleSwap_high = v_D0_doubleSwap_high.M();
                D0_deltaM_doubleSwap_low = std::abs(D0_M_doubleSwap_low - D0_M_PDG);
                D0_deltaM_doubleSwap_high = std::abs(D0_M_doubleSwap_high - D0_M_PDG);

                // Also fill ETA and P branches for high and low pions
                D0Pi_low_P = v_D0Pi_low->P();
                D0Pi_high_P = v_D0Pi_high->P();
                if ((mode == "Kpipipi" && KstarK_ID < 0) || 
                        (mode == "piKpipi" && KstarK_ID > 0)) {
                    if (v_D0PiPlus_P.P() > v_D0Pi_P.P()) {
                        D0Pi_low_ETA = D0Pi_ETA;
                        D0Pi_high_ETA = D0PiPlus_ETA;
                    } else {
                        D0Pi_high_ETA = D0Pi_ETA;
                        D0Pi_low_ETA = D0PiPlus_ETA;
                    }
                } else {
                    if (v_D0PiMinus_P.P() > v_D0Pi_P.P()) {
                        D0Pi_low_ETA = D0Pi_ETA;
                        D0Pi_high_ETA = D0PiMinus_ETA;
                    } else {
                        D0Pi_high_ETA = D0Pi_ETA;
                        D0Pi_low_ETA = D0PiMinus_ETA;
                    }
                }

            }
        }

        // Fill the new tree
        outputTree->Fill();
    }

    // Print info
    std::cout << "Applied MVA " << mvaName << " to tree." << std::endl;
    std::cout << "New tree has " << outputTree->GetEntries() << " entries." <<
        std::endl;
    std::cout << "Skipped " << less_than_zero_entries << " entries with IPCHI2 < 0."
        << std::endl;
    std::cout << "Found " << error_entries << " entries with error status." 
        << std::endl;
}


// Helper function for setting up Lorentz vector branch
void MVAApplicator::LorentzVectorBranch(TTree* tree, TLorentzVector& vec,
        const std::string& name) {
    tree->SetBranchAddress((name + "_PE").c_str(), &vec[3]);
    tree->SetBranchAddress((name + "_PX").c_str(), &vec[0]);
    tree->SetBranchAddress((name + "_PY").c_str(), &vec[1]);
    tree->SetBranchAddress((name + "_PZ").c_str(), &vec[2]);
}
