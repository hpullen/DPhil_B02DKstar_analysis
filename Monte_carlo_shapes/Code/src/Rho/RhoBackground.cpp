#include <cmath>
#include <fstream>
#include <iostream>

#include "TEventList.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLorentzVector.h"
#include "TTree.h"

#include "Rho/RhoBackground.hpp"
#include "DoubleCBFitter.hpp"
#include "RangeReader.hpp"


// ===========
// Constructor
// ===========
RhoBackground::RhoBackground(std::string name, bool usePidCuts) {
    // Initialize private members
    m_name = name;
    m_events = 0;
    m_fitDone = false;
    m_cut = readCut(usePidCuts);

    // Read in mass range and create histogram
    RangeReader rr("../Inputs/Ranges/rho_run2.txt");
    m_histo = new TH1F(name.c_str(), "", 100, rr.getLow(), rr.getHigh());

    // Make fitter for specified fit shape
    m_fitter.reset(new DoubleCBFitter(name));
}


// ==========
// Destructor
// ==========
RhoBackground::~RhoBackground() {
    delete m_histo;
}


// ===================================
// Add data from new file to histogram
// ===================================
void RhoBackground::addData(std::string filename, std::string treename) {
    // Open file and get tree
    TFile* file = TFile::Open(filename.c_str(), "READ");
    TTree* tree = (TTree*)file->Get(treename.c_str());

    // Process the tree
    processTree(tree);

    // Clean up
    delete tree;
    file->Close();
}


// ==============================================
// Take starting input params, then fit histogram
// ==============================================
void RhoBackground::fitHistogram(std::string paramInput) {
    // Add parameters to fitter
    m_fitter->readStartingParams("../Inputs/Starting_params/" + paramInput +
            ".param");

    // Check if data has been added yet 
    if (m_events == 1) {
        std::cerr << "Cannot fit histogram " << m_name << 
            " until data has been added!" << std::endl;
        return;
    }
    m_parameters = m_fitter->fit(m_histo);
    m_fitDone = true;
}


// ============================================
// Helper inline for helicity angle calculation
// ============================================
inline double Product(const TLorentzVector& p1, const TLorentzVector& p2,
        const TLorentzVector& pX) {
    return - p1.Dot(p2) + p1.Dot(pX) * p2.Dot(pX) / pX.Dot(pX);
}


// ========================================
// Apply cuts to a TTree and fill histogram
// ========================================
void RhoBackground::processTree(TTree* tree) {
	// Set up B0 mass variable
    double Bd_M;
    tree->SetBranchAddress("Bd_M", &Bd_M);

    // Set up Lorentz vector branches
    TLorentzVector KstarK_P, KstarPi_P, D0_P, D0K_P, D0Pi_P;
    LorentzVectorBranch(tree, KstarK_P, "KstarK"); 
    LorentzVectorBranch(tree, KstarPi_P, "KstarPi"); 
    LorentzVectorBranch(tree, D0_P, "D0"); 
    LorentzVectorBranch(tree, D0K_P, "D0K");
    LorentzVectorBranch(tree, D0Pi_P, "D0Pi");

    // Variables to calculate
    double helicityAngle;

    // Loop over entries and fill histogram
    int nEntries = tree->GetEntries();
    std::cout << m_name << ":" << std::endl;
    std::cout << nEntries << " entries." << std::endl;

    // Select only entries passing cut
    tree->Draw(">>elist", m_cut);
    TEventList* eventList = (TEventList*)gDirectory->Get("elist");

    // Loop over event list
    for (int i = 0; i < eventList->GetN(); ++i) {
        // Get current entry from tree
        tree->GetEntry(eventList->GetEntry(i));

        // Calculate helicity angle and cut on it
        helicityAngle = Product(KstarK_P, D0_P, KstarK_P + KstarPi_P) / 
            sqrt(Product(KstarK_P, KstarK_P, KstarK_P + KstarPi_P) * 
			Product(D0_P, D0_P, KstarK_P + KstarPi_P));
        if (std::abs(helicityAngle) < 0.4) continue;

        // Fill histogram
        m_histo->Fill((double)Bd_M);

        // Increment event count 
        m_events++;
    }

    std::cout << "Successful: " << this->m_events << " events added." 
        << std::endl;
}


// ======================
// Return name of dataset
// ======================
std::string RhoBackground::getName() {
    return this->m_name;
}


// ====================================
// Return number of events in histogram
// ====================================
int RhoBackground::getEvents() {
    return this->m_events;
}


// ==================
// Return fit parameters
// ==================
std::map<std::string, double> RhoBackground::getParameters() {
    // Check if parameters have been created
    if (m_parameters.empty()) {
        std::cerr << "Empty parameter map for " << m_name << 
            "Perform fit before getting parameters." << std::endl;
    }
    return m_parameters;
}

// =====================
// Read in cut from file
// =====================
TCut RhoBackground::readCut(bool usePidCuts) {
    // Open file
    std::ifstream file("../../Library/cuts/Common/all_mc.cut");

    // Read out lines and add to cut
    TCut cut;
    std::string line;
    while (std::getline(file, line)) {
        cut += line.c_str();
    }

    // Add BKGCAT cut 
    cut += "Bd_BKGCAT == 30";

    // Add PID cuts if using
    if (usePidCuts) {
        cut += "KstarK_PIDK_corr > 3";
        cut += "KstarPi_PIDK_corr < -1";
        cut += "D0K_PIDK_corr > 1";
        cut += "D0Pi_PIDK_corr < -1";
    }

    // Return the full cut
    std::cout << "Cut: " << cut << std::endl;
    return cut;
}


// =========================
// Set up momentum 4-vector 
// =========================
void RhoBackground::LorentzVectorBranch(TTree* tree, TLorentzVector& vec,
        const std::string& name) {
    tree->SetBranchAddress((name + "_PE").c_str(), &vec[3]);
    tree->SetBranchAddress((name + "_PX").c_str(), &vec[0]);
    tree->SetBranchAddress((name + "_PY").c_str(), &vec[1]);
    tree->SetBranchAddress((name + "_PZ").c_str(), &vec[2]);
}
