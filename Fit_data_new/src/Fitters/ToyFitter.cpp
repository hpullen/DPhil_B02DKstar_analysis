#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"

#include "RooDataHist.h"
#include "RooFitResult.h"
#include "RooRandom.h"
#include "RooRealVar.h"

#include "ToyFitter.hpp"

// ===========
// Constructor
// ===========
ToyFitter::ToyFitter(ShapeMakerBase * toy_maker) :
    m_toymaker(toy_maker),
    m_toy(GenerateToy(toy_maker)) {
}


// ==========
// Destructor
// ==========
ToyFitter::~ToyFitter() {
}


// =========================
// Add a PDF to fitting list
// =========================
void ToyFitter::AddFitPdf(ShapeMakerBase * pdf_maker) {
    AddFitPdf(pdf_maker->Name(), pdf_maker);
}


// ==============================================
// Add a fit PDF to fitting list with custom name
// ==============================================
void ToyFitter::AddFitPdf(std::string name, ShapeMakerBase * pdf_maker) {
    
    // Check that pdf maker has same properties as toy maker
    if (pdf_maker->Category() != m_toymaker->Category()) {
        std::cout << "Error: PDF maker (" << pdf_maker->Name() << " and toy maker ("
            << m_toymaker->Name() << " have different RooCategories." << std::endl;
        return;
    } else if (pdf_maker->FitVariable() != m_toymaker->FitVariable()) {
        std::cout << "Error: PDF maker (" << pdf_maker->Name() << " and toy maker ("
            << m_toymaker->Name() << " have different fit variables." << std::endl;
        return;
    }

    // Add to map
    std::cout << "Successfully added PDF maker " << pdf_maker->Name() << 
        " to ToyFitter under key " << name << std::endl;
    m_pdfs.emplace(name, pdf_maker);
}


// ===============
// Perform the fit
// ===============
void ToyFitter::PerformFits(std::string filename, int n_repeats) {

    // Set up the tree
    TFile * outfile = TFile::Open(filename.c_str(), "RECREATE");
    TTree * tree = new TTree("toy_tree", "");
    std::map<std::string, double*> params_list = SetupTree(tree);

    // Loop over desired number of fits
    for (int i = 0; i < n_repeats; i++) {

        // Fit to toy
        PerformSingleFit(params_list);
        tree->Fill();

        // Generate a new toy
        if (i < (n_repeats - 1)) {
            GenerateNewToy();
        }
    }

    // Save plots if only using one
    if (n_repeats == 1) SaveHistograms();

    // Save tree to file
    outfile->cd();
    tree->Write();
    outfile->Close();
}


// =========================
// Save histograms to a file
// =========================
void ToyFitter::SaveHistograms() {
    for (auto pdf : m_pdfs) {
        pdf.second->SaveHistograms("./Histograms/toy_" + pdf.first + ".root",
                m_toy);
    }
}
    

// ========================
// Set branches in the tree
// ========================
std::map<std::string, double*> ToyFitter::SetupTree(TTree * tree) {
    
    // Map to hold parameters
    std::map<std::string, double*> map;

    // List of parameter types to add
    std::vector<std::string> param_types = {"init_value", "final_value",
        "error", "pull"};

    // Vector to store PDFs which have been processed
    std::vector<std::string> processed = {};

    // Loop through different fit PDFs
    for (auto pdf : m_pdfs) {

        // Add values/errors/pulls
        for (auto par : pdf.second->Parameters()) {
            for (auto type : param_types) {
                map.emplace(pdf.first + "_" + type + "_" + par, new double(0));
            }

            // Comparison pull with other PDFs
            for (auto proc : processed) {
                std::vector<std::string> proc_pars = m_pdfs.at(proc)->Parameters();
                if (std::find(proc_pars.begin(), proc_pars.end(), par) 
                        != proc_pars.end()) {
                        map.emplace(proc + "_vs_" + pdf.first + "_pull_" + par, 
                                new double(0));
                }

            } // End loop over processed PDFs

        } // End loop over parameter list

        processed.push_back(pdf.first);

    } // End loop over PDFs

    // Create branches in tree for each parameter
    for (auto param : map) {
        tree->Branch(param.first.c_str(), param.second, 
                (param.first + "/D").c_str());
    }

    // Add a status branch
    map.emplace("status", new double(0));
    tree->Branch("status", map.at("status"), "status/I");

    // Return the map containing the doubles
    return map;
}


// ================================
// Peform a fit to each of the PDFs
// ================================
std::map<std::string, RooFitResult*> ToyFitter::PerformSingleFit(const std::map<std::string, double*> & params_list) {

    // Map to hold fit results for processed PDFs
    std::map<std::string, RooFitResult*> results;

    // Reset shapes to their original starting values
    for (auto pdf : m_pdfs) {
        pdf.second->RemakeShape();
    }

    // Loop through PDFs
    for (auto pdf : m_pdfs) {

        // Perform the fit
        RooFitResult * result = pdf.second->Shape()->fitTo(*m_toy, 
                RooFit::Save(), RooFit::NumCPU(8, 2), RooFit::Optimize(false), 
                RooFit::Offset(true), RooFit::Minimizer("Minuit2", "migrad"), 
                RooFit::Strategy(2));
        std::cout << "Min NLL: " << result->minNll() << std::endl;
        // result->Print("v");

        // Get variables
        RooArgList params_init = result->floatParsInit();
        RooArgList params_final = result->floatParsFinal();

        // Loop through variable list and fill doubles
        for (auto par : pdf.second->Parameters()) {
            RooRealVar * init_var = (RooRealVar*)params_init.find((pdf.first + 
                        "_params_" + par).c_str());
            RooRealVar * final_var = (RooRealVar*)params_final.find((pdf.first + 
                        "_params_" + par).c_str());
            *params_list.at(pdf.first + "_init_value_" + par) = init_var->getVal();
            *params_list.at(pdf.first + "_final_value_" + par) = final_var->getVal();
            *params_list.at(pdf.first + "_error_" + par) = final_var->getError();
            *params_list.at(pdf.first + "_pull_" + par) = (final_var->getVal() -
                    init_var->getVal()) / final_var->getError();

            // Save comparisons with other PDF fit results
            for (auto res : results) {

                std::vector<std::string> proc_pars = m_pdfs.at(res.first)->Parameters();
                if (std::find(proc_pars.begin(), proc_pars.end(), par) != proc_pars.end()) {
                    RooRealVar * proc_par = 
                        (RooRealVar*)res.second->floatParsFinal().find((res.first 
                                    + "_params_" + par).c_str());
                    proc_par->Print();
                    *params_list.at(res.first + "_vs_" + pdf.first + "_pull_" + par)
                        = (final_var->getVal() - proc_par->getVal()) /
                        sqrt(pow(final_var->getError(), 2) + 
                                pow(proc_par->getError(), 2));
                }

            } // End loop over completed results

        } // End loop over free parameters

        // Add to status
        *params_list.at("status") += result->status();

        // Add to map of results
        results.emplace(pdf.first, result);

    } // End loop over PDFs

    // Return RooFitResults
    return results;
}


// ==========================
// Remake the toy data member
// ==========================
void ToyFitter::GenerateNewToy() {
    m_toy = GenerateToy(m_toymaker);
}


// ==============
// Generate a toy
// ==============
RooDataHist * ToyFitter::GenerateToy(ShapeMakerBase * toy_maker) {

    // Set random seed
    TRandom * rand = new TRandom;
    rand->SetSeed();
    RooRandom::setRandomGenerator(rand);

    // Generate a binned dataset
    RooDataHist * data = toy_maker->Shape()->generateBinned(RooArgList(*toy_maker->FitVariable(),
                *toy_maker->Category()), toy_maker->ExpectedEvents());
    return data;

}