#include <algorithm>

#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"

#include "RooDataHist.h"
#include "RooFitResult.h"
#include "RooRandom.h"
#include "RooRealVar.h"
#include "RooMsgService.h"

#include "ToyPdfMaker.hpp"
#include "ToyFitter.hpp"

// ===========
// Constructor
// ===========
ToyFitter::ToyFitter(DataPdfMaker * toy_maker, bool binned,
        bool split_obs) :
    m_toymaker(toy_maker),
    m_toy(GenerateToy(toy_maker, binned)),
    m_binned(binned),
    m_combine_runs(false),
    m_split_obs(split_obs) {
}
ToyFitter::ToyFitter(DataPdfMaker * toy_maker, bool binned, bool combine_runs, 
        bool split_obs) :
    m_toymaker(toy_maker),
    m_toy(GenerateToy(toy_maker, binned)),
    m_binned(binned),
    m_combine_runs(combine_runs),
    m_split_obs(split_obs) {
}


// ==========
// Destructor
// ==========
ToyFitter::~ToyFitter() {
}


// =========================
// Add a PDF to fitting list
// =========================
void ToyFitter::AddFitPdf(DataPdfMaker * pdf_maker) {
    AddFitPdf(pdf_maker->Name(), pdf_maker);
}


// ==============================================
// Add a fit PDF to fitting list with custom name
// ==============================================
void ToyFitter::AddFitPdf(std::string name, DataPdfMaker * pdf_maker) {
    
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
    std::map<std::string, double*> * params_list = SetupTree(tree);

    // Set up covQual and status
    int covQual = 0;
    int status = 0;
    tree->Branch("covQual", &covQual, "covQual/I");
    tree->Branch("status", &status, "status/I");

    // Loop over desired number of fits
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

        // Generate a new toy
        if (i < (n_repeats - 1)) {
            GenerateNewToy();
        }
    }

    // Save plots if only using one
    if (n_repeats == 1) {
        std::cout << "SAVING HISTOGRAMS" << std::endl;
        SaveHistograms();
    }

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
        pdf.second->SaveHistograms("./Histograms/toy_" + pdf.first + ".root", m_toy, false);
    }
}
    

// ========================
// Set branches in the tree
// ========================
std::map<std::string, double*> * ToyFitter::SetupTree(TTree * tree) {
    
    // Map to hold parameters
    std::map<std::string, double*> * map = new std::map<std::string, double*>();

    // List of parameter types to add
    std::vector<std::string> param_types = {"init_value", "final_value",
        "init_error", "final_error", "pull"};

    // Vector to store PDFs which have been processed
    std::vector<std::string> processed = {};
    
    // List of runs
    std::vector<std::string> runs = {""};
    if (!m_combine_runs) runs = {"_run1", "_run2"};

    // Loop through different fit PDFs
    for (auto pdf : m_pdfs) {

        // Add values/errors/pulls
        for (auto par : pdf.second->Parameters()) {
            for (auto type : param_types) {
                map->emplace(pdf.first + "_" + type + "_" + par, new double(0));
            }

            // Comparison pull with other PDFs
            for (auto proc : processed) {
                std::vector<std::string> proc_pars = m_pdfs.at(proc)->Parameters();
                if (std::find(proc_pars.begin(), proc_pars.end(), par) 
                        != proc_pars.end()) {
                        map->emplace(proc + "_vs_" + pdf.first + "_pull_" + par, 
                                new double(0));
                }

            } // End loop over processed PDFs

        } // End loop over parameter list

        // Add R_ds and R_Bs values and error
        for (std::string mode : {"KK", "pipi", "pipipipi"}) {
            for (std::string run : runs) {
                if (mode == "pipipipi" && run == "_run1") continue;
                std::string R_ds_var = "R_ds_" + mode + run;
                std::string R_Bs_var = "R_Bs_" + mode + run;
                for (auto type : param_types) {
                    map->emplace(pdf.first + "_" + type + "_" + R_ds_var, new double(0));
                    map->emplace(pdf.first + "_" + type + "_" + R_Bs_var, new double(0));
                }
            }
        }

        // Add R_ADS and A_ADS
        if (pdf.second->IsSplit()) {
            std::vector<std::string> obs_runs = {""};
            if (m_split_obs) obs_runs = {"_run1", "_run2"};
            for (std::string mode : {"piK", "piKpipi"}) {
                for (std::string run : obs_runs) {
                    std::string R_ADS_var = "R_ADS_" + mode + run;
                    std::string A_ADS_var = "A_ADS_" + mode + run;
                    for (auto type : param_types) {
                        map->emplace(pdf.first + "_" + type + "_" + R_ADS_var, new double(0));
                        map->emplace(pdf.first + "_" + type + "_" + A_ADS_var, new double(0));
                    }
                }
            }
        }

        processed.push_back(pdf.first);

    } // End loop over PDFs

    // Add number of entries
    map->emplace("entries", new double(0));
    for (std::string run : runs) {
        for (std::string mode : {"Kpi", "piK", "KK", "pipi", 
            "Kpipipi", "piKpipi", "pipipipi"}) {
            if (run != "_run2" && mode == "pipipipi") continue;
            std::vector<std::string> flavs = {};
            if (m_toymaker->IsSplit()) flavs = {"_plus", "_minus"};
            for (std::string sign : flavs) {
                map->emplace("entries_" + mode + run + sign, new double(0));
            }
        }
    }

    // Create branches in tree for each parameter
    for (auto param : *map) {
        tree->Branch(param.first.c_str(), param.second, 
                (param.first + "/D").c_str());
    }

    // Return the map containing the doubles
    return map;
}


// ================================
// Peform a fit to each of the PDFs
// ================================
std::map<std::string, RooFitResult*> ToyFitter::PerformSingleFit(std::map<std::string, double*> * params_list) {

    // Map to hold fit results for processed PDFs
    std::map<std::string, RooFitResult*> results;

    // Reset shapes to their original starting values
    for (auto pdf : m_pdfs) {
        pdf.second->RemakeShape();
    }

    // Save number of entries in toy
    *params_list->at("entries") = m_toy->sumEntries();
    std::string cat_name = m_toymaker->Category()->GetName();
    std::vector<std::string> runs = {""};
    if (!m_combine_runs) runs = {"_run1", "_run2"};
    for (std::string run : runs) {
        for (std::string mode : {"Kpi", "piK", "KK", "pipi", 
                "Kpipipi", "piKpipi", "pipipipi"}) {
            if (run != "_run2" && mode == "pipipipi") continue;
            std::vector<std::string> flavs = {};
            if (m_toymaker->IsSplit()) flavs = {"_plus", "_minus"};
            for (std::string sign : flavs) {
                *params_list->at("entries_" + mode + run + sign) = 
                    m_toy->sumEntries((cat_name + "==" + cat_name + "::" 
                                + mode + run + sign).c_str());
            }
        }
    }

    // Loop through PDFs
    for (auto pdf : m_pdfs) {

        // Perform the fit
        pdf.second->SetMaxYields(m_toy);
        RooSimultaneous * fit_pdf = pdf.second->Shape();
        RooFitResult * result = fit_pdf->fitTo(*m_toy, 
                RooFit::Save(), RooFit::NumCPU(4), RooFit::Optimize(false),
                RooFit::Strategy(2), RooFit::Minimizer("Minuit2", "migrad"),
                RooFit::PrintEvalErrors(-1), RooFit::PrintLevel(-1));
        result->Print("v");

        // Get variables
        RooArgList params_final = result->floatParsFinal();

        // Loop through variable list and fill doubles
        for (auto par : pdf.second->Parameters()) {

            // Skip DummyBlindState
            if (par == "e") continue;

            // Fill values 
            RooRealVar * final_var = (RooRealVar*)params_final.find((pdf.first + 
                        "_params_" + par).c_str());
            double var_gen = m_toymaker->GetParameterValue(par);
            double var_fit = final_var->getVal();
            *params_list->at(pdf.first + "_init_value_" + par) = var_gen;
            *params_list->at(pdf.first + "_final_value_" + par) = final_var->getVal();

            // Fill errors
            double err = final_var->getError();
            *params_list->at(pdf.first + "_init_error_" + par) = 
                m_toymaker->GetParameterError(par);
            *params_list->at(pdf.first + "_final_error_" + par) = err;

            // Calculate pull
            double pull;
            pull = (var_fit - var_gen)/err;
            *params_list->at(pdf.first + "_pull_" + par) = pull;

            // Save comparisons with other PDF fit results
            for (auto res : results) {

                std::vector<std::string> proc_pars = 
                    m_pdfs.at(res.first)->Parameters();
                if (std::find(proc_pars.begin(), proc_pars.end(), par) 
                        != proc_pars.end()) {
                    RooRealVar * proc_par = 
                        (RooRealVar*)res.second->floatParsFinal().find((res.first 
                                    + "_params_" + par).c_str());
                    *params_list->at(res.first + "_vs_" + pdf.first + "_pull_" + par)
                        = (final_var->getVal() - proc_par->getVal()) /
                        sqrt(pow(final_var->getError(), 2) + 
                                pow(proc_par->getError(), 2));
                }

            } // End loop over completed results

        } // End loop over free parameters

        // Write R_ds values
        std::vector<std::string> runs = {""};
        if (!m_combine_runs) runs = {"_run1", "_run2"};
        for (std::string mode : {"KK", "pipi", "pipipipi"}) {
            for (std::string run : runs) {
                if (mode == "pipipipi" && run == "_run1") continue;
                for (std::string var_type : {"R_ds_", "R_Bs_"}) {
                    std::string var = var_type + mode + run;

                    // Get initial value and uncertainty
                    RooFormulaVar * R_ds_init = 
                        (RooFormulaVar*)m_toymaker->GetParameter(var_type + mode + run);
                    double val_init = R_ds_init->getVal();
                    *params_list->at(pdf.first + "_init_value_" + var) = val_init;

                    // Write new value and uncertainty
                    RooFormulaVar * R_ds_final = 
                        (RooFormulaVar*)pdf.second->GetParameter(var_type + mode + run);
                    double val_final = R_ds_final->getVal();
                    double err_final = R_ds_final->getPropagatedError(*result);
                    *params_list->at(pdf.first + "_final_value_" + var) = val_final;
                    *params_list->at(pdf.first + "_final_error_" + var) = err_final;

                    // Calculate pull
                    *params_list->at(pdf.first + "_pull_" + var) =
                        (val_final - val_init)/err_final;
                }
            }
        }

        // Write R_ADS and A_ADS values
        std::vector<std::string> obs_runs = {""};
        if (m_split_obs) obs_runs = {"_run1", "_run2"};
        if (pdf.second->IsSplit()) {
            for (std::string mode : {"piK", "piKpipi"}) {
                for (std::string run : obs_runs) {
                    for (std::string var_type : {"R_ADS_", "A_ADS_"}) {
                        std::string var = var_type + mode + run;

                        // Get initial value and uncertainty
                        RooFormulaVar * R_ds_init = 
                            (RooFormulaVar*)m_toymaker->GetParameter(var_type + mode + run);
                        double val_init = R_ds_init->getVal();
                        *params_list->at(pdf.first + "_init_value_" + var) = val_init;

                        // Write new value and uncertainty
                        RooFormulaVar * R_ds_final = 
                            (RooFormulaVar*)pdf.second->GetParameter(var_type + mode + run);
                        double val_final = R_ds_final->getVal();
                        double err_final = R_ds_final->getPropagatedError(*result);
                        *params_list->at(pdf.first + "_final_value_" + var) = val_final;
                        *params_list->at(pdf.first + "_final_error_" + var) = err_final;

                        // Calculate pull
                        *params_list->at(pdf.first + "_pull_" + var) =
                            (val_final - val_init)/err_final;
                    }
                }
            }
        }

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
    m_toy = GenerateToy(m_toymaker, m_binned);
}


// ==============
// Generate a toy
// ==============
RooAbsData * ToyFitter::GenerateToy(DataPdfMaker * toy_maker, bool binned) {

    // Set random seed
    TRandom * rand = new TRandom;
    // rand->SetSeed(1044086272);
    rand->SetSeed();
    std::cout << "Seed: " << rand->GetSeed() << std::endl;
    RooRandom::setRandomGenerator(rand);
    toy_maker->Shape();
    std::cout << "Shape generation PDF: " << toy_maker->Shape()->getVal() 
        << std::endl;
    // toy_maker->PrintToFile("toy_generation_pdf.txt");

    // Generate a binned dataset
    RooAbsData * data;
    if (!binned) {
        std::cout << "Making RooDataSet" << std::endl;
        data = (RooAbsData*)toy_maker->Shape()->generate(RooArgList(*toy_maker->FitVariable(),
                    *toy_maker->Category()), RooFit::Extended(kTRUE));
    } else {
        std::cout << "Making RooDataHist" << std::endl;
        data = toy_maker->Shape()->generateBinned(RooArgList(*toy_maker->FitVariable(),
                    *toy_maker->Category()), RooFit::Extended(kTRUE));
    }
    std::cout << "Generated toy with " << data->sumEntries() << "entries." << std::endl;
    TString cat_name = m_toymaker->Category()->GetName();
    return data;

}
