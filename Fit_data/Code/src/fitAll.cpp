#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

#include "TApplication.h"
#include "TChain.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TRandom3.h"

#include "RooMCStudy.h"
#include "RooAddPdf.h"
#include "RooRandom.h"
#include "RooArgList.h"
#include "RooAbsData.h"
#include "RooDataHist.h"
#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooCBShape.h"
#include "RooDataSet.h"
#include "RooExponential.h"
#include "RooFitResult.h"
#include "RooFormulaVar.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooUnblindUniform.h"
#include "RooHist.h"

#include "RooHORNSdini.h"
#include "RooHILLdini.h"
#include "RooLITTLEHORNSdini.h"

#include "ParameterReader.hpp"
#include "ShapeMaker.hpp"
#include "Plotter.hpp"

int main(int argc, char * argv[]) {

    // ===============
    // Read input args
    // ===============
    // Check for parameters
    if (argc != 4) {
        std::cout << "Usage: ./FitAll <2011:2012:2015:2016> <Sum: Y/N>" 
            " <Binned: Y/N>" << std::endl;
        return -1;
    }

    // Year (colon separated string)
    std::string input_year = argv[1];

    // Sum over charge (Y/N)
    std::string sum = argv[2];

    // Choice of binned/unbinned
    std::string binned = argv[3];

    // Tighter PID cut on K*0 K
    bool tight_pid = true;

    // Vectors of years and D0 modes
    std::vector<std::string> years = {"2011", "2012", "2015", "2016"};
    std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi",
        "piKpipi", "pipipipi"};

    // ================
    // Set up variables
    // ================
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5000, 5800);
    RooRealVar KstarK_ID("KstarK_ID", "", -100000, 100000);
    RooRealVar BDT_Kpi("BDTG_Kpi_run2", "", 0.5, 1);
    RooRealVar BDT_piK("BDTG_Kpi_run2", "", 0.5, 1);
    RooRealVar BDT_KK("BDTG_KK_run2", "", 0.5, 1);
    RooRealVar BDT_pipi("BDTG_pipi_run2", "", 0.5, 1);
    RooRealVar BDT_Kpipipi("BDTG_Kpipipi_run2", "", 0.5, 1);
    RooRealVar BDT_pipipipi("BDTG_pipipipi_run2", "", 0.5, 1);

    // Make list of args to use
    std::map<std::string, RooArgList *> args;
    for (auto mode : modes) {
        args[mode] = new RooArgList();
        args[mode]->add(Bd_M);
        args[mode]->add(KstarK_ID);
        if (mode == "Kpi") args[mode]->add(BDT_Kpi);
        else if (mode == "piK") args[mode]->add(BDT_piK);
        else if (mode == "KK") args[mode]->add(BDT_KK);
        else if (mode == "pipi") args[mode]->add(BDT_pipi);
        else if (mode == "Kpipipi") args[mode]->add(BDT_Kpipipi);
        else if (mode == "piKpipi") args[mode]->add(BDT_Kpipipi);
        else if (mode == "pipipipi") args[mode]->add(BDT_pipipipi);
    }

    // Set up bins
    int binWidth = 8;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // ============
    // Read in data
    // ============
    std::string path; 
    std::map<std::string, RooDataSet *> data_both;
    std::map<std::string, RooDataSet *> data_plus;
    std::map<std::string, RooDataSet *> data_minus;

    // Open file to hold tree copies 
    TFile * file = TFile::Open("../Histograms/temp_trees.root", "RECREATE");

    // Loop through modes
    for (auto mode : modes) {

        // Make TChain to hold data
        TChain * chain = new TChain("DecayTree");

        // Get path 
        if (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi") {  
            path = "/data/lhcb/users/pullen/B02DKstar/data/fourBody/";
        } else {
            path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
        }

        // Loop through years
        for (auto year : years) {
            if (input_year.find(year) != std::string::npos) {
                if (mode == "pipipipi") {
                    if (year == "2011" || year == "2012") continue;
                }
                std::string filepath_down = path + year + "_down/" + mode + "_selected.root";
                std::string filepath_up = path + year + "_up/" + mode + "_selected.root";
                chain->Add(filepath_down.c_str());
                chain->Add(filepath_up.c_str());
            }
        }

        // Convert to RooDataSet
        std::cout << "Making RooDataSet for mode: " << mode << std::endl;
        if (!tight_pid) {
            data_both[mode] = new RooDataSet(("data_" + mode).c_str(), "", chain, *args[mode]);
        } else {
            TTree * tree = chain->CopyTree("KstarK_PIDK > 5");
            data_both[mode] = new RooDataSet(("data_" + mode).c_str(), "", tree, *args[mode]);
        }

        // Print info
        data_both[mode]->Print();

        // Get flavour separated modes if requested
        if (sum != "Y") {
            std::cout << "Splitting dataset by flavour." << std::endl;
            data_plus[mode] = (RooDataSet*)data_both[mode]->reduce("KstarK_ID > 0");
            data_minus[mode] = (RooDataSet*)data_both[mode]->reduce("KstarK_ID < 0");
            data_plus[mode]->Print();
            data_minus[mode]->Print();
        }
    }
    file->Close();

    // Number of entries in each mode 
    std::map<std::string, int> total_entries;
    for (auto mode : modes) {
        total_entries[mode] = data_both[mode]->sumEntries();
        std::cout << "Total entries " << mode << ": " << total_entries[mode] <<
            std::endl;
    }

    // Make shapes for two and four body modes
    ShapeMaker * sm = new ShapeMaker(sum, &Bd_M, true);
    RooSimultaneous * simPdf = sm->makeFitPdf(total_entries, true);

    // Make combined dataset
    RooAbsData * combData;
    if (binned == "Y") {

        // Binned fit: convert to RooDataHist
        std::map<std::string, RooDataHist *> all_data;
        for (auto mode : modes) {
            if (sum == "Y") {
                data_both[mode]->Print("v");
                all_data[mode] = data_both[mode]->binnedClone();
            } else {
                all_data[mode + "_plus"] = data_plus[mode]->binnedClone();
                all_data[mode + "_minus"] = data_minus[mode]->binnedClone();
            }
        }
        combData = new RooDataHist("combData", "", Bd_M, *sm->getCategory(), 
                all_data);
    } else {

        // Unbinned fit: keep as RooDataSet
        std::map<std::string, RooDataSet *> all_data;
        for (auto mode : modes) {
            if (sum == "Y") {
                all_data[mode] = data_both[mode];
            } else {
                all_data[mode + "_plus"] = data_plus[mode];
                all_data[mode + "_minus"] = data_minus[mode];
            }
        }
        combData = new RooDataSet("combData", "", Bd_M, 
                RooFit::Index(*(sm->getCategory())), RooFit::Import(all_data));
    }

    // ===========
    // Perform fit
    // ===========
    RooFitResult * data_result;
    if (binned == "Y") {
        data_result = simPdf->fitTo(*((RooDataHist*)combData), RooFit::Save(), 
                RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true), 
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    } else {
        data_result = simPdf->fitTo(*((RooDataSet*)combData), RooFit::Save(), 
                RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true), 
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    }
    data_result->Print("v");

    // Make name for results output
    std::string sum_string = ((sum == "Y") ? "combined" : "split");
    std::string bin_string = ((binned == "Y") ? "binned" : "unbinned");
    std::string result_filename;

    // Filename for RooFitResult
    result_filename = "../Results/all_" + input_year + "_" + sum_string + 
        "_" + bin_string + ".root";
    std::cout << "Will save result to: " << result_filename << std::endl;

    // Save result to file
    TFile * result_file = TFile::Open(result_filename.c_str(), "RECREATE");
    result_file->cd();
    data_result->Write("fit_result");
    result_file->Close();

    // ================================
    // Save results to a histogram file 
    // ================================
    // Filename
    std::string outfile_name;
    outfile_name = "../Histograms/fits_all_" + sum_string + "_" + 
        bin_string + ".root";

    // Save
    if(sum == "Y") {
        sm->saveFitHistograms(outfile_name, data_both);
    } else {
        std::map<std::string, RooDataSet*> data_split;
        for (auto mode : modes) {
            data_split[mode + "_plus"] = data_plus[mode];
            data_split[mode + "_minus"] = data_minus[mode];
        }
        sm->saveFitHistograms(outfile_name, data_split);
    }


    // ================
    // Plot the results
    // ================
    Plotter * plotter = new Plotter();
    // If using standard cuts, save to the usual plot directory
    if (sum == "Y") {
        plotter->plotFourModeFitsCombined(
                ("../Histograms/fits_all_combined_" + bin_string + 
                 ".root").c_str(), "all_" + input_year + "_" 
                + bin_string, "");
        plotter->plotFourBodyFitsCombined(
                ("../Histograms/fits_all_combined_" + bin_string + 
                 ".root").c_str(), "all_" + input_year + "_" +
                bin_string, "");
    } else {
        plotter->plotFourModeFitsSeparate(
                ("../Histograms/fits_all_split_" + bin_string + 
                 ".root").c_str(), "all_" + input_year + "_" + 
                bin_string, "");
        plotter->plotFourBodyFitsSeparate(
                ("../Histograms/fits_all_split_" + bin_string + 
                 ".root").c_str(), "all_" + input_year + "_" + 
                bin_string, "");
    }
    delete plotter;

    return 0;
}
