#include <sstream> 
#include <cmath>

#include "TFile.h"
#include "TTree.h"

#include "RooCategory.h"
#include "RooRealVar.h"
#include "RooFitResult.h"
#include "RooRandom.h"

#include "ToySystematicFitter.hpp"
#include "ToyPdfMaker.hpp"
#include "DataPdfMaker.hpp"
#include "ParameterReader.hpp"


// ===========
// Constructor
// ===========
ToySystematicFitter::ToySystematicFitter(SysOption opt) : 
    ToyFitter(MakeToyPdf(), true), m_name("sys"), m_opt(opt) {

    // Add fit PDFs
    AddFitPdf(MakeSignalPdf());
}



// ==========
// Destructor
// ==========
ToySystematicFitter::~ToySystematicFitter() {}


// ==========================================================
// Override standard fitting function to include significance
// ==========================================================
void ToySystematicFitter::PerformFits(std::string filename, int n_repeats) {

    // Make tree and set up
    TFile * outfile = TFile::Open(filename.c_str(), "RECREATE");
    TTree * tree = new TTree("toy_tree", "");
    std::map<std::string, double*> * params_list = SetupTree(tree);

    // Set up covQual and status
    int covQual = 0;
    int status = 0;
    tree->Branch("covQual", &covQual, "covQual/I");
    tree->Branch("status", &status, "status/I");

    // Loop over and perform fits
    for (int i = 0; i < n_repeats; i++) {

        // Fit to toy
        std::map<std::string, RooFitResult*> results = PerformSingleFit(params_list);
        // Fill covQual and status
        status = 0;
        covQual = 0;
        for (auto pdf : results) {
            covQual += pdf.second->covQual();
            status += pdf.second->status();
        }

        // Fill tree
        tree->Fill();

        // Make new toy PDF with different yields
        m_toymaker = MakeToyPdf();
        GenerateNewToy();
    }

    // Save plots if only using one repetition
    if (n_repeats == 1) SaveHistograms();

    // Save tree to file
    outfile->cd();
    tree->Write();
    outfile->Close();

}


// =======================
// Make toy generation PDF
// =======================
ToyPdfMaker * ToySystematicFitter::MakeToyPdf() {
    
    // Initialise mass variable and category
    m_x = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    m_cat = new RooCategory("modes", "");
    for (TString run : {"_run1", "_run2"}) {
        for (TString sign : {"_plus", "_minus"}) {
            m_cat->defineType("Kpi" + run + sign);
            m_cat->defineType("piK" + run + sign);
            m_cat->defineType("KK" + run + sign);
            m_cat->defineType("pipi" + run + sign);
            m_cat->defineType("Kpipipi" + run + sign);
            m_cat->defineType("piKpipi" + run + sign);
            if (run != "_run1") {
                m_cat->defineType("pipipipi" + run + sign);
            }
        }
    }
    std::string input_file = "Results/twoAndFourBody_data_split.root";
    ToyPdfMaker * pdf = new ToyPdfMaker("toy", m_x, m_cat, input_file);
    pdf->PrintToFile("pars.txt");

    // Adjust charmless yields
    for (std::string run : {"1", "2"}) {

        // Read yields for each mode
        ParameterReader * pr = new ParameterReader("../Backgrounds/Charmless/"
                "Results/high_stats/after_cut");
        for (std::string mode : {"piK", "KK", "pipi", "piKpipi", "pipipipi"}) {
            if (run == "1" && mode == "pipipipi") continue;
            pr->ReadParameters(mode, mode + "_run_" + run + ".txt");
        }

        // Add to yields
        for (std::string parent : {"Bd", "Bs"}) {

            // Modes to consider
            std::vector<std::string> modes;
            if (parent == "Bd") {
                modes = {"piK", "KK", "pipi", "piKpipi", "pipipipi"};
            } else {
                modes = {"piK", "piKpipi"};
            }

            // Variable name
            std::string var_name = (parent == "Bs") ? "N_Bs" : "N_signal";
            for (std::string mode : modes) {
                if (run == "1" && mode == "pipipipi") continue;

                // Get mean and sigma
                double mean = pr->GetValue(mode, parent);
                double sigma = pr->GetError(mode, parent);

                // Generate random number of events
                gRandom->SetSeed(0);
                int events = std::round(gRandom->Gaus(mean, sigma));
                std::cout << "Will add " << events << " events to " <<
                    var_name << " run " << run << " in " << mode << std::endl;

                // Split between B0 and B0bar 
                double N_plus = 0;
                double N_minus = 0;
                for (int i = 0; i < std::abs(events); i++) {
                    gRandom->SetSeed(0);
                    int rand = gRandom->Integer(2);
                    if (rand == 0) {
                        if (events > 0) {
                            N_plus++;
                        } else {
                            N_plus--;
                        }
                    } else {
                        if (events > 0) {
                            N_minus++;
                        } else {
                            N_minus--;
                        }
                    }
                }

                // Adjust the values
                std::string full_var = var_name + "_" + mode + "_run" + run;
                if (N_plus) pdf->AddEvents(full_var + "_plus", N_plus);
                if (N_minus) pdf->AddEvents(full_var + "_minus", N_minus);
            }
        }
    }

    // Return PDF with adjusted values
    return pdf;
}


// ===================
// Make signal fit PDF
// ===================
ShapeMakerBase * ToySystematicFitter::MakeSignalPdf() {
    DataPdfMaker * pdf = new DataPdfMaker(m_name + "_signal", m_x, m_cat, false);
    return pdf;
}
