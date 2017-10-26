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
void MVAApplicator::apply(std::string inputPath, std::string outputPath, 
        std::string mvaName, std::string varFile) {

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
    TTree * inputTree = (TTree*)inputFile->Get("Tuple_Kpipipi/DecayTree");
    std::map<std::string, double> data_raw_vars_map;
    std::map<std::string, float> data_floats_map;
    setupDataVariables(inputTree, varFile, &data_raw_vars_map, &data_floats_map);

    // Set up output tree
    TFile * outputFile = TFile::Open(outputPath.c_str(), "RECREATE");
    TTree * outputTree = inputTree->CloneTree(0);
    Float_t mva_response;
    outputTree->Branch(("BDTG_" + mvaName).c_str(), &mva_response);

    // Evaluate MVA and add to tree
    evaluateMVA(reader, inputTree, outputTree, mvaName, &mva_response,
            &mva_vars_map, &data_raw_vars_map, &data_floats_map);

    // // Save new tree
    outputTree->Write();
    outputFile->Close();
    std::cout << "New TTree with MVA " << mvaName << " applied saved to " <<
        outputPath << "." << std::endl;
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
        std::map<std::string, Float_t> * mva_vars,
        std::map<std::string, double> * data_vars,
        std::map<std::string, float> * float_vars) {

    // Loop through tree
    int error_entries = 0;
    int less_than_zero_entries = 0;
    std::cout << "Looping over " << inputTree->GetEntries() << " entries." << 
        std::endl;
    for (Long64_t ievt = 0; ievt < inputTree->GetEntries(); ievt++) {

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
