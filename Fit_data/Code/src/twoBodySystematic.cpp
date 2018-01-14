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
    if (argc != 4 && argc != 8 && argc != 9) {
        std::cout << "Usage: ./FitTwoBody <2011:2012:2015:2016> <Sum: Y/N>"
            " <Binned: Y/N> (Custom BDT cuts: <Kpi-cut> <piK-cut> <KK-cut> <pipi-cut> <Save-plots: Y/N>)" << std::endl;
        return -1;
    }

    // Year (colon separated string)
    std::string input_year = argv[1];

    // Sum over charge (Y/N)
    std::string sum = argv[2];

    // Choice of binned/unbinned
    std::string binned = argv[3];

    // Custom BDT cut if using
    bool custom_cuts = false;
    double cut_Kpi = 0.5;
    double cut_piK = 0.5;
    double cut_KK = 0.5;
    double cut_pipi = 0.5;
    std::string save_plots = "Y";
    if (argc >= 8) {
        custom_cuts = true;
        save_plots = "N";
        cut_Kpi = atof(argv[4]);
        cut_piK = atof(argv[5]);
        cut_KK = atof(argv[6]);
        cut_pipi = atof(argv[7]);
        if (argc == 9) save_plots = argv[8];
    }

    // Tighter PID cut on K*0 K
    bool tight_pid = true;

    // Vectors of years and D0 modes
    std::vector<std::string> years = {"2011", "2012", "2015", "2016"};
    std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi"};

    // ================
    // Set up variables
    // ================
    RooRealVar Bd_M("Bd_ConsD_MD", "", 5000, 5800);
    RooRealVar KstarK_ID("KstarK_ID", "", -100000, 100000);
    RooRealVar BDT_Kpi("BDTG_Kpi_run2", "", cut_Kpi, 1);
    RooRealVar BDT_piK("BDTG_Kpi_run2", "", cut_piK, 1);
    RooRealVar BDT_KK("BDTG_KK_run2", "", cut_KK, 1);
    RooRealVar BDT_pipi("BDTG_pipi_run2", "", cut_pipi, 1);
    //RooRealVar KstarK_PIDK("KstarK_PIDK", "", -100000, 1000000);

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
    }

    // Set up bins
    int binWidth = 8;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // ============
    // Read in data
    // ============
    std::string path = "/data/lhcb/users/pullen/B02DKstar/data/twoBody/";
    std::map<std::string, RooDataSet *> data_both;
    std::map<std::string, RooDataSet *> data_plus;
    std::map<std::string, RooDataSet *> data_minus;

    // Open file to hold tree copies 
    TFile * file = TFile::Open("../Histograms/temp_trees.root", "RECREATE");

    // Loop through modes
    for (auto mode : modes) {

        // Make TChain to hold data
        TChain * chain = new TChain("DecayTree");

        // Loop through years
        for (auto year : years) {
            if (input_year.find(year) != std::string::npos) {
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

    // Make shapes
    ShapeMaker * sm = new ShapeMaker(sum, &Bd_M);
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
    std::stringstream bdt_stream;

    // Filename for RooFitResult
    if (custom_cuts) {
        bdt_stream << "_" << std::setprecision(2) << cut_Kpi << "_" << cut_piK << "_"
            << cut_KK << "_" << cut_pipi;
        result_filename = "../Results/BDT_studies/twoBody_" + input_year + "_" +
            sum_string + "_" + bin_string + bdt_stream.str() + ".root";
    } else {
        result_filename = "../Results/twoBody_" + input_year + "_" + sum_string + 
            "_" + bin_string + ".root";
    }
    if (tight_pid) result_filename = "../Results/twoBody_tightPID.root";
    std::cout << "Will save result to: " << result_filename << std::endl;

    // Save result to file
    TFile * result_file = TFile::Open(result_filename.c_str(), "RECREATE");
    result_file->cd();
    data_result->Write("fit_result");
    result_file->Close();

    // ================================
    // Save results to a histogram file (only if not doing BDT study)
    // ================================
    if (save_plots == "Y") {

        // Filename
        std::string outfile_name;
        if (custom_cuts) {
            outfile_name = "../Histograms/BDT_studies/fits_twoBody_" + sum_string + "_" + bin_string + bdt_stream.str() + ".root";
        } else {
            outfile_name = "../Histograms/fits_twoBody_" + sum_string + "_" + bin_string + ".root";
        }
        if (tight_pid) {
            outfile_name = "../Histograms/fits_twoBody_tightPID.root";
        }

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
        if (custom_cuts) {

            // Save to a subdirectory for BDT study plots
            if (sum == "Y") {
                plotter->plotFourModeFitsCombined(("../Histograms/BDT_studies/fits_twoBody_combined_" + bin_string + bdt_stream.str() + ".root").c_str(), 
                        "BDT_studies/twoBody_" + input_year + "_" + bin_string + bdt_stream.str(), "");
            } else {
                plotter->plotFourModeFitsSeparate(("../Histograms/BDT_studies/fits_twoBody_split_" + bin_string + bdt_stream.str() + ".root").c_str(), 
                        "BDT_studies/twoBody_" + input_year + "_" + bin_string + bdt_stream.str(), "");
            }
        } else {

            // If using standard cuts, save to the usual plot directory
            if (!tight_pid) {
                if (sum == "Y") {
                    plotter->plotFourModeFitsCombined(("../Histograms/fits_twoBody_combined_" + bin_string + ".root").c_str(), 
                            "twoBody_" + input_year + "_" + bin_string, "");
                } else {
                    plotter->plotFourModeFitsSeparate(("../Histograms/fits_twoBody_split_" + bin_string + ".root").c_str(), 
                            "twoBody_" + input_year + "_" + bin_string, "");
                }
            } else {
                plotter->plotFourModeFitsCombined("../Histograms/fits_twoBody_tightPID.root", "twoBody_tightPID", "");
            }
        }
        delete plotter;
    }

    return 0;
}
