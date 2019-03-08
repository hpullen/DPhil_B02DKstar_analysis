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
ToySystematicFitter::ToySystematicFitter(SysOption opt, bool combine_runs) : 
    ToyFitter(MakeToyPdf({}), true, combine_runs), m_name("sys"), m_opt(opt),
    m_combine_runs(combine_runs) {

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

    // Make branches to save extra event numbers
    std::map<std::string, double*> extra_events;
    for (std::string mode : {"piK", "KK", "pipi", "piKpipi", "pipipipi"}) {
        for (std::string run : {"_run1", "_run2"}) {
            for (std::string parent : {"_Bs", "_Bd"}) {
                for (std::string sign : {"_plus", "_minus"}) {
                    std::string name = mode + parent + run + sign;
                    extra_events[name] = new double(0);
                    tree->Branch(name.c_str(), extra_events[name], (name + "/D").c_str());
                }
            }
        }
    }

    // Loop over and perform fits
    for (int i = 0; i < n_repeats; i++) {

        // Make new toy PDF with different yields
        m_toymaker = MakeToyPdf(extra_events);
        GenerateNewToy();

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
ToyPdfMaker * ToySystematicFitter::MakeToyPdf(std::map<std::string, double*> extra_events) {
    
    // Initialise mass variable and category
    m_x = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    m_cat = new RooCategory("modes", "");
    std::vector<std::string> runs;
    if (m_combine_runs) {
        std::cout << "m_combine_runs is true";
        runs = {""};
    } else { 
        std::cout << "m_combine_runs is false";
        runs = {"_run1", "_run2"};
    }
    for (TString run : runs) {
        std::cout << "Run : " << run << std::endl;
        for (TString sign : {"_plus", "_minus"}) {
            for (TString mode : {"Kpi", "piK", "KK", "pipi", "Kpipipi", "piKpipi"}) {
                TString cat_name = mode + run + sign;
                m_cat->defineType(cat_name);
            }
            if (run == "_run2") {
                m_cat->defineType("pipipipi" + run + sign);
            }
        }
    }
    std::string input_file = m_combine_runs ? 
        "Results/twoAndFourBody_data_split_combinedRuns.root" : 
        "Results/twoAndFourBody_data_split.root";
    ToyPdfMaker * pdf = new ToyPdfMaker("toy", m_x, m_cat, input_file);
    pdf->PrintToFile("pars.txt");

    // Adjust charmless yields
    for (std::string run : runs) {

        // Run number
        std::string run_num = "";
        if (run == "_run1") run_num = "1";
        else if (run == "_run2") run_num = "2";

        // Read yields for each mode
        ParameterReader * pr = new ParameterReader("../Backgrounds/Charmless/"
                "Results/high_stats/after_cut");
        for (std::string mode : {"piK", "KK", "pipi", "piKpipi", "pipipipi"}) {
            if (run != "_run2" && mode == "pipipipi") continue;
            if (m_combine_runs) {
                pr->ReadParameters(mode + "_run1", mode + "_run_1.txt");
                pr->ReadParameters(mode + "_run2", mode + "_run_2.txt");
            } else {
                pr->ReadParameters(mode, mode + "_run_" + run + ".txt");
            }
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
                if (run != "_run2" && mode == "pipipipi") continue;

                // Get mean and sigma
                double mean, sigma; 
                if (!m_combine_runs) {
                    mean = pr->GetValue(mode, parent);
                    sigma = pr->GetError(mode, parent);
                } else {
                    mean = pr->GetValue(mode + "_run1", parent) + 
                        pr->GetValue(mode + "_run2", parent);
                    sigma = sqrt(pow(pr->GetError(mode + "_run1", parent), 2) +
                            pow(pr->GetError(mode + "_run2", parent), 2));
                }

                // Generate random number of events
                gRandom->SetSeed(0);
                int events = std::round(gRandom->Gaus(mean, sigma));
                std::cout << "Will add " << events << " events to " <<
                    var_name << " " << run << " in " << mode << std::endl;

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
                std::string full_var = var_name + "_" + mode + run;
                if (N_plus) pdf->AddEvents(full_var + "_plus", N_plus);
                if (N_minus) pdf->AddEvents(full_var + "_minus", N_minus);

                // Add to map
                if (extra_events.size() > 0) {
                    std::string name = mode + "_" + parent + run;
                    *extra_events[name + "_plus"] = N_plus;
                    *extra_events[name + "_minus"] = N_minus;
                }
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
