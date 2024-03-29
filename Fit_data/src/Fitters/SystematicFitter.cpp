#include "TTree.h"
#include "TFile.h"

#include "RooRealVar.h"
#include "RooCategory.h"
#include "RooFitResult.h"
#include "RooDataHist.h"
#include "RooDataSet.h"

#include "SystematicFitter.hpp"
#include "SystematicPdfMaker.hpp"
#include "DataPdfMaker.hpp"


// ===========
// Constructor
// ===========
SystematicFitter::SystematicFitter(SysOption opt, bool combine_runs, bool split_obs) :

    DataFitter(new SystematicPdfMaker(opt, MakeFitVariable(), 
                MakeCategory(combine_runs), false, split_obs), 
            true, split_obs), 
    m_opt(opt),
    m_combine_runs(combine_runs), 
    m_split_obs(split_obs)
{

    // Load in data 
    std::vector<std::string> years = {"2011", "2012", "2015", "2016"};
    std::vector<std::string> modes_twoBody = {"Kpi", "piK", "KK", "pipi"};
    std::vector<std::string> modes_fourBody = {"Kpipipi", "piKpipi", "pipipipi"};
    std::vector<std::string> mags = {"up", "down"};
    std::string data_path = "/data/lhcb/users/pullen/B02DKstar/data/";
    for (auto year : years) {
        std::string run; 
        if (m_combine_runs) {
            run = "";
        } else {
            run = (year == "2011" || year == "2012") ? "_run1" : "_run2";
        }
        for (auto mag : mags) {
            for (auto mode : modes_twoBody) {
                AddFile(mode + run, data_path + "/twoBody/" + year + "_" + mag + "/" 
                        + mode + "_selected.root");
            }
            for (auto mode : modes_fourBody) {
                if (mode == "pipipipi" && (year == "2011" || year == "2012")) {
                    continue;
                }
                AddFile(mode + run, data_path + "/fourBody/" + year + "_" + mag + "/"
                        + mode + "_selected.root");
            }
        }
    }

}


// ================
// Perform the fits
// ================
void SystematicFitter::PerformFits(std::string filename, int n_repeats, bool binned) {

    // Set up the tree
    TFile * outfile = TFile::Open(filename.c_str(), "RECREATE");
    TTree * tree = new TTree("sys_tree", "");
    std::map<std::string, double*> params_list = SetupTree(tree);

    // Loop over fits
    for (int i = 0; i < n_repeats; i++) {
        PerformSingleFit(params_list, binned);
        tree->Fill();
        if (i < (n_repeats - 1)) RemakeShape();
    }

    // Save to file
    outfile->cd();
    tree->Write();
    outfile->Close();
}


// ===================================================
// Regenerate shape with reshifted constant parameters
// ===================================================
void SystematicFitter::RemakeShape() {
    m_pdf->RemakeShape();
}



// ==========================
// Create fit variable (Bd_M)
// ==========================
RooRealVar * SystematicFitter::MakeFitVariable() {
    RooRealVar * Bd_M = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);
    return Bd_M;
}


// ===============
// Create category
// ===============
RooCategory * SystematicFitter::MakeCategory(bool combine_runs) {

    // Make category
    RooCategory * cat = new RooCategory("category", "");

    // List of modes
    std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        "piKpipi", "pipipipi"};

    // Loop through and add
    std::vector<std::string> runs = {""};
    if (!combine_runs) runs = {"_run1", "_run2"};
    for (std::string run : runs) {
        for (auto mode : modes) {
            if (mode == "pipipipi" && run != "_run2") continue;
            cat->defineType((mode + run + "_plus").c_str());
            cat->defineType((mode + run + "_minus").c_str());
        }
    }
    return cat;
}


// ====================
// Set branches in tree
// ====================
std::map<std::string, double*> SystematicFitter::SetupTree(TTree * tree) {

    // Map to hold parameters
    std::map<std::string, double*> map;

    // Add floating parameters as branches
    for (auto par : m_pdf->Parameters()) {
        map.emplace(par, new double(0));
        tree->Branch(par.c_str(), map.at(par), (par + "/D").c_str());
        // std::cout << "Adding branch for " << par << std::endl;
    }

    // Add R_ds and R_Bs
    std::vector<std::string> runs = {""};
    if (!m_combine_runs) runs = {"_run1", "_run2"};
    for (std::string run : runs) {
        for (std::string mode : {"KK", "pipi", "pipipipi"}) {
            if (mode == "pipipipi" && run != "_run2") continue;
            for (std::string var_type : {"R_ds_", "R_Bs_"}) {
                std::string par = var_type + mode + run;
                map.emplace(par, new double(0));
                tree->Branch(par.c_str(), map.at(par), (par + "/D").c_str());
            }
        }
    }

    // Add R_ADS and A_ADS
    std::vector<std::string> obs_runs = {""};
    if (m_split_obs) obs_runs = {"_run1", "_run2"};
    for (std::string run : obs_runs) {
        for (std::string mode : {"piK", "piKpipi"}) {
            for (std::string var_type : {"R_ADS_", "A_ADS_"}) {
                std::string par = var_type + mode + run;
                map.emplace(par, new double(0));
                tree->Branch(par.c_str(), map.at(par), (par + "/D").c_str());
            }
        }
    }

    // Add constant parameters
    // RooArgSet * pars = m_pdf->Shape()->getParameters(GetData());
    // RooRealVar * var;
    // TIterator * it = pars->createIterator();
    // while ((var = (RooRealVar*)it->Next())) {
        // if (var->isConstant()) {
            // map.emplace(var->GetName(), new double(0));
            // tree->Branch(var->GetName(), map.at(var->GetName()), var->GetName());
            // // std::cout << "Adding branch for " << var->GetName() << std::endl;
        // }
    // }

    // Status branch
    map.emplace("status", new double(0));
    tree->Branch("status", map.at("status"), "status/D");
    map.emplace("covQual", new double(0));
    tree->Branch("covQual", map.at("covQual"), "covQual/D");

    // Fixed parameter branches
    for (auto par : ParsToSave()) {
        map.emplace(par, new double(0));
        tree->Branch(par.c_str(), map.at(par), (par + "/D").c_str());
    }

    // Return the parameters
    return map;
}


// ================================
// Perform one fit and fill doubles
// ================================
void SystematicFitter::PerformSingleFit(std::map<std::string, double*> params_list,
        bool binned) {

    // Peform fit
    RooAbsData * data;
    if (binned) {
        data = GetData();
    } else {
        data = GetUnbinnedData();
    }
    m_pdf->SetMaxYields(data);
    RooFitResult * result = m_pdf->Shape()->fitTo(*data, RooFit::Save(),
            RooFit::NumCPU(4), RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));

    // Get variables
    RooArgList params_final = result->floatParsFinal();

    // Fill doubles in parameter list
    for (auto par : m_pdf->Parameters()) {
        if (par == "e" || par == "tate") continue;
        if (params_list.find(par) == params_list.end()) {
            std::cout << "Parameter " << par << " not found in list. Skipping." 
                << std::endl;
            continue;
        }
        RooRealVar * final_var = (RooRealVar*)params_final.find(("sys_params_" 
                    + par).c_str());
        *params_list.at(par) = final_var->getVal();
    }

    // Add status
    *params_list.at("status") = result->status();
    *params_list.at("covQual") = result->covQual();

    // Add constant parameters
    for (auto par : ParsToSave()) {
        double val = ((RooRealVar*)result->constPars().find(("sys_"
                        "params_" + par).c_str()))->getVal();
        std::cout << par << ": " << val << std::endl;
        *params_list.at(par) = val;
    }

    // Add R_ds and R_Bs
    std::vector<std::string> runs = {""};
    if (!m_combine_runs) runs = {"_run1", "_run2"};
    for (std::string run : runs) {
        for (std::string mode : {"KK", "pipi", "pipipipi"}) {
            if (mode == "pipipipi" && run != "_run2") continue;
            for (std::string var_type : {"R_ds_", "R_Bs_"}) {
                *params_list.at(var_type + mode + run) = 
                    m_pdf->GetParameterValue(var_type + mode + run);
            }
        }
    }

    // Add R_ADS and A_ADS
    std::vector<std::string> obs_runs = {""};
    if (m_split_obs) obs_runs = {"_run1", "_run2"};
    for (std::string run : obs_runs) {
        for (std::string mode : {"piK", "piKpipi"}) {
            for (std::string var_type : {"R_ADS_", "A_ADS_"}) {
                *params_list.at(var_type + mode + run) = 
                    m_pdf->GetParameterValue(var_type + mode + run);
            }
        }
    }


}


// ======================
// Get parameters to save
// ======================
std::vector<std::string> SystematicFitter::ParsToSave() {

    // Vector of runs
    std::vector<std::string> runs = {""};
    if (!m_combine_runs) runs = {"_run1", "_run2"};

    // Set parameters to save
    std::vector<std::string> pars_to_save = {};
    if (m_opt == SysOption::production_asymmetry) {
        for (std::string run : runs) {
            for (std::string parent : {"Bs", "B0"}) {
                pars_to_save.push_back("A_prod_" + parent + run);
            }
        }
    } else if (m_opt == SysOption::Bs_low_shape_pars) {
        for (std::string part : {"gamma", "pi"}) {
            for (std::string hel : {"101", "010"}) {
                std::string name = "Bs_" + part + "_" + hel;
                pars_to_save.push_back(name + "_a");
                pars_to_save.push_back(name + "_b");
                pars_to_save.push_back(name + "_csi");
                pars_to_save.push_back(name + "_frac");
                pars_to_save.push_back(name + "_ratio");
                pars_to_save.push_back(name + "_sigma");
            }
        }
    } else if (m_opt == SysOption::detection_asymmetry) {
        for (std::string run : runs) {
            pars_to_save.push_back("A_det" + run);
        }
    } else if (m_opt == SysOption::signal_shape_pars) {
        pars_to_save.push_back("signal_alpha_L");
        pars_to_save.push_back("signal_alpha_R");
        pars_to_save.push_back("Bs_alpha_L");
        pars_to_save.push_back("Bs_alpha_R");
    }
    return pars_to_save;
}
