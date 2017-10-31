#include "LowMass/LowMassBackground.hpp"
#include "LowMass/LowMassFitter.hpp"

#include <cmath>
#include <fstream>
#include <iostream>

#include "TEventList.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TCanvas.h"

#include "RangeReader.hpp"


// ===========
// Constructor
// ===========
LowMassBackground::LowMassBackground(int run_number) {

    // Initialise data members
    m_name = getFullName(run_number);
    m_fitDone = false;
    m_cut = readCut();
    m_run = run_number;

    // Get range for histograms
    RangeReader rr("../Inputs/Ranges/" + m_name + ".txt");
    std::cout << "Range: " << rr.getLow() << " - " << rr.getHigh() << std::endl;

    // Make background components
    std::vector<Component> compList = {Component::gamma_010, Component::gamma_100,
        Component::pi_100, Component::pi_010, Component::Bs_gamma_010,
        Component::Bs_gamma_100, Component::Bs_pi_010, Component::Bs_pi_100};
    for (auto comp : compList) {
        std::string compName = compString(comp, run_number);
        m_components[comp].name = compName;
        m_components[comp].events = 0;
        m_components[comp].histogram = new TH1F((m_name + "_" + compName).c_str(),
                "", 100, rr.getLow() - 1, rr.getHigh() - 1);
    }

    // Print info
    std::cout << "Created background mode: " << m_name << std::endl;
}


// ==========
// Destructor
// ==========
LowMassBackground::~LowMassBackground() {
}


// =======================
// Add data to a histogram
// =======================
void LowMassBackground::addData(std::string filename, std::string treename,
        Component component) {

    // Open file and get tree
    TFile * file = TFile::Open(filename.c_str(), "READ");
    TTree * tree = (TTree*)file->Get(treename.c_str());

    // Process the tree
    processTree(tree, component);

    // Clean up
    delete tree;
    file->Close();
}


// =============================
// Fit histograms simultaneously
// =============================
void LowMassBackground::fitHistograms() {

    // Check if data has been added
    for (auto comp : m_components) {
        if (comp.second.events == 0) {
            std::cout << "Error: no events added for " << comp.second.name <<
                std::endl;
            return;
        }
    }

    // Make fitter and perform fit
    LowMassFitter * fitter = new LowMassFitter(m_name, m_components, m_run);
    m_fitDone = true;

    // Get parameters
    auto params = fitter->getParameters();
    for (auto comp : m_components) {
        m_components[comp.first].parameters = params[comp.first];
    }

    // Make parameter output files
    makeParamsFiles();
    makeOverallParamsFile();

    // Clean up
    delete fitter;
}


// ===============================
// Functions for getting variables
// ===============================
// Name of background
std::string LowMassBackground::getName() { return m_name; }

// Number of events in a component
int LowMassBackground::getEvents(Component comp) {
    return m_components[comp].events;
}


// ==================================================
// Read in cut for partially reconstructed background
// ==================================================
TCut LowMassBackground::readCut() {
    // Open file
    std::ifstream file("../../Library/cuts/Common/all_mc.cut");

    // Read out lines and add to cut
    TCut cut;
    std::string line;
    while (std::getline(file, line)) {
        cut += line.c_str();
    }

    // Add BKGCAT cut for part reco background
    cut += "Bd_BKGCAT == 50";

    // Return the full cut
    return cut;
}


// ===================================================
// Get full name of background mode to use as filename
// ===================================================
std::string LowMassBackground::getFullName(int run_number) {

    // Add run number
    std::string str = "lowmass_";
    if (run_number == 1) {
        str += "run1";
    } else if (run_number == 2) {
        str += "run2";
    }
    return str;
}


// ==================================
// Convert component enum to a string
// ==================================
std::string LowMassBackground::compString(Component comp, int run_number) { 

    // Add name of component to string
    std::string str = "";
    switch (comp) {
        case Component::pi_010: str += "pi_010";
                                break;
        case Component::pi_100: str += "pi_100";
                                break;
        case Component::gamma_010: str += "gamma_010";
                                   break;
        case Component::gamma_100: str += "gamma_100";
                                   break;
        case Component::Bs_pi_010: str += "Bs_pi_010";
                                   break;
        case Component::Bs_pi_100: str += "Bs_pi_100";
                                   break;
        case Component::Bs_gamma_010: str += "Bs_gamma_010";
                                   break;
        case Component::Bs_gamma_100: str += "Bs_gamma_100";
                                   break;
    }

    // Add run number to string
    if (run_number == 1) {
        str += "_run1";
    } else if (run_number == 2) {
        str += "_run2";
    }
    return str;
}


// ============================================
// Helper inline for helicity angle calculation
// ============================================
inline double Product(const TLorentzVector& p1, const TLorentzVector& p2,
        const TLorentzVector& pX) {
    return - p1.Dot(p2) + p1.Dot(pX) * p2.Dot(pX) / pX.Dot(pX);
}


// ==============
// Process a tree
// ==============
void LowMassBackground::processTree(TTree * tree, Component comp) {

    // Set up mass variable
    float Bd_M;
    tree->SetBranchAddress("Bd_ConsD_M", &Bd_M);

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
    std::cout << m_components[comp].name << ":" << std::endl;
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
        m_components[comp].histogram->Fill((float)Bd_M);

        // Increment event count 
        m_components[comp].events++;
    }

    // // Plot helicity angle for true candidates
    // int BKGCAT;
    // tree->SetBranchAddress("Bd_BKGCAT", &BKGCAT);
    // TH1F * helicity = new TH1F((compString(comp, m_run, m_parent) + "_helicityAngle").c_str(), "",
            // 100, -1, 1);
    // for (int i = 0; i < tree->GetEntries(); ++i) {
        // tree->GetEntry(i);
        // if (BKGCAT != 50) continue;
        // helicityAngle = Product(KstarK_P, D0_P, KstarK_P + KstarPi_P) /
            // sqrt(Product(KstarK_P, KstarK_P, KstarK_P + KstarPi_P) *
			// Product(D0_P, D0_P, KstarK_P + KstarPi_P));
        // helicity->Fill(helicityAngle);
    // }

    // TCanvas * canvas = new TCanvas((compString(comp, m_run, m_parent) + "helCanvas").c_str(), "", 500, 400);
    // helicity->Draw();
    // canvas->SaveAs((compString(comp, m_run, m_parent) + "_helicityAngle.pdf").c_str());
    // delete canvas;

    std::cout << "Successful: " << m_components[comp].events << " events added." 
        << std::endl;

}


// ===========================
// Make parameter output files
// ===========================
void LowMassBackground::makeParamsFiles() {

    // Check fit is done
    if (!m_fitDone) {
        std::cout << "Can't make parameter files before fit is complete!"
            << std::endl;
        return;
    }

    // Loop through components and write to file
    std::string dir = "../Results/LowMass/";
    for (auto comp : m_components) {
        std::ofstream file(dir + comp.second.name + ".param");
        file << "a " << comp.second.parameters.a << std::endl;
        file << "b " << comp.second.parameters.b << std::endl;
        file << "csi " << comp.second.parameters.csi << std::endl;
        file << "f " << comp.second.parameters.f << std::endl;
        file << "ratio " << comp.second.parameters.ratio << std::endl;
        file << "sigma " << comp.second.parameters.sigma << std::endl;
        file.close();
    }
}


// ==========================================
// Make single file containing all parameters
// ==========================================
void LowMassBackground::makeOverallParamsFile() {
   
    std::ofstream file("../Results/lowMass_all.param");
    file << "a_gamma " << m_components[Component::gamma_010].parameters.a 
        << std::endl;
    file << "b_gamma " << m_components[Component::gamma_010].parameters.b 
        << std::endl;
    file << "a_pi " << m_components[Component::pi_010].parameters.a << std::endl;
    file << "b_pi " << m_components[Component::pi_010].parameters.b << std::endl;
    file << "f " << m_components[Component::gamma_010].parameters.f << std::endl;
    file << "ratio " << m_components[Component::gamma_010].parameters.ratio << std::endl;
    file << "sigma_gamma_010 " << m_components[Component::gamma_010].parameters.sigma 
        << std::endl;
    file << "sigma_gamma_100 " << m_components[Component::gamma_100].parameters.sigma 
        << std::endl;
    file << "sigma_pi_010 " << m_components[Component::pi_010].parameters.sigma 
        << std::endl;
    file << "sigma_pi_100 " << m_components[Component::pi_100].parameters.sigma 
        << std::endl;
    file << "csi_gamma_010 " << m_components[Component::gamma_010].parameters.csi 
        << std::endl;
    file << "csi_gamma_100 " << m_components[Component::gamma_100].parameters.csi 
        << std::endl;
    file << "csi_pi_010 " << m_components[Component::pi_010].parameters.csi 
        << std::endl;
    file << "csi_pi_100 " << m_components[Component::pi_100].parameters.csi 
        << std::endl;
    file << "Bs_sigma_gamma_010 " << m_components[Component::Bs_gamma_010].parameters.sigma 
        << std::endl;
    file << "Bs_sigma_gamma_100 " << m_components[Component::Bs_gamma_100].parameters.sigma 
        << std::endl;
    file << "Bs_sigma_pi_010 " << m_components[Component::Bs_pi_010].parameters.sigma 
        << std::endl;
    file << "Bs_sigma_pi_100 " << m_components[Component::Bs_pi_100].parameters.sigma 
        << std::endl;
    file << "Bs_csi_gamma_010 " << m_components[Component::Bs_gamma_010].parameters.csi 
        << std::endl;
    file << "Bs_csi_gamma_100 " << m_components[Component::Bs_gamma_100].parameters.csi 
        << std::endl;
    file << "Bs_csi_pi_010 " << m_components[Component::Bs_pi_010].parameters.csi 
        << std::endl;
    file << "Bs_csi_pi_100 " << m_components[Component::Bs_pi_100].parameters.csi 
        << std::endl;
}


// =========================
// Set up momentum 4-vector 
// =========================
void LowMassBackground::LorentzVectorBranch(TTree* tree, TLorentzVector& vec,
        const std::string& name) {
    tree->SetBranchAddress((name + "_PE").c_str(), &vec[3]);
    tree->SetBranchAddress((name + "_PX").c_str(), &vec[0]);
    tree->SetBranchAddress((name + "_PY").c_str(), &vec[1]);
    tree->SetBranchAddress((name + "_PZ").c_str(), &vec[2]);
}
