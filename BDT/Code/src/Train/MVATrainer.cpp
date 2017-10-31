#include <fstream>
#include <iostream>

#include "TFile.h"
#include "TCut.h"
#include "TTree.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TInterpreter.h"
#include "TLorentzVector.h"

#include "MVATrainer.hpp"


// ===================
// Default constructor
// ===================
MVATrainer::MVATrainer() {
    m_nTrees = 1000;
    m_maxDepth = 2;
}


// =============================================
// Constructor with variable nTrees and maxDepth
// =============================================
MVATrainer::MVATrainer(int nTrees, int maxDepth) : 
    m_nTrees(nTrees),
    m_maxDepth(maxDepth) {
}


// ==========
// Destructor
// ==========
MVATrainer::~MVATrainer() {
}


// ============
// Train a BDTG
// ============
void MVATrainer::train(std::string name, std::vector<std::string> signalFiles, 
        std::vector<std::string> backgroundFiles, std::string varFile,
        std::string cutFile) {

    // Load the library
    TMVA::Tools::Instance();
    std::cout << "==> Starting TMVA classification" << std::endl;

    // Create ROOT output file
    std::string outfile_name = "../Outputs/Classification_" + name + ".root";
    TFile * outfile = TFile::Open(outfile_name.c_str(), "RECREATE");

    // Create TMVA factory object
    TMVA::Factory * factory = new TMVA::Factory("TMVAClassification", outfile,
            "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:"
            "AnalysisType=Classification");

    // Setup
    addVariables(factory, varFile);
    addTrees(factory, signalFiles, backgroundFiles, cutFile);
    bookMethods(factory, name);

    // Train and test MVAs
    factory->TrainAllMethods();
    factory->TestAllMethods();
    factory->EvaluateAllMethods();

    // Save output and clean up
    outfile->Close();
    std::cout << "==> Wrote root file: " << outfile->GetName() << std::endl;
    std::cout << "==> TMVA classification done!" << std::endl;
    delete factory;
} 


// ====================
// Add variables to MVA
// ====================
void MVATrainer::addVariables(TMVA::Factory * factory, std::string varFile) {

    // Open variable file
    std::ifstream file(varFile);
    std::string line;

    // Add each line to the factory
    while (std::getline(file, line)) {
        std::cout << "Adding variable: " << line << std::endl; 
        factory->AddVariable(line.c_str(), 'F');
    }
    file.close();
}


// ==========================================
// Add signal and background trees to the MVA
// ==========================================
void MVATrainer::addTrees(TMVA::Factory * factory, std::vector<std::string> 
        signalFiles, std::vector<std::string> backgroundFiles, std::string cutFile) 
{
    // Set weights 
    double signalWeight = 1.0;
    double backgroundWeight = 1.0;

    // Add signal trees
    for (auto signalFile : signalFiles) {
        TFile * sFile = TFile::Open(signalFile.c_str());
        TTree * sTree = (TTree*)sFile->Get("DecayTree");
        factory->AddSignalTree(sTree, signalWeight);
    }

    // Add background trees
    for (auto backgroundFile : backgroundFiles) {
        TFile * bFile = TFile::Open(backgroundFile.c_str());
        TTree * bTree = (TTree*)bFile->Get("DecayTree");
        factory->AddBackgroundTree(bTree, backgroundWeight);
    }

    // Read in cut from file
    std::ifstream file(cutFile);
    std::string line;
    TCut cut = "";
    while (std::getline(file, line)) {
        cut += line.c_str();
    }

    // Tell factory how to use training and test events
    factory->PrepareTrainingAndTestTree(cut, cut, "nTrain_Signal=0:"
            "nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V" );
}


// =============================
// Book the required MVA methods
// =============================
void MVATrainer::bookMethods(TMVA::Factory * factory, std::string name) {

    // Gradient boosted decision tree
    std::string opts_string = "!H:!V:NTrees=" + std::to_string(m_nTrees) +
            ":MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:"
            "BaggedSampleFraction=0.5:nCuts=20:MaxDepth=" + 
            std::to_string(m_maxDepth);
    factory->BookMethod(TMVA::Types::kBDT, ("BDTG_" + name).c_str(), 
            opts_string.c_str());
}
