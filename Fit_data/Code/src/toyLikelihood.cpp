#include <cmath>
#include <iostream>
#include <string>

#include "TCanvas.h"
#include "TTree.h"
#include "TIterator.h"
#include "TString.h"
#include "TFile.h"

#include "RooAbsReal.h"
#include "RooArgSet.h"
#include "RooMsgService.h"
#include "RooCategory.h"
#include "RooDataHist.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooMCStudy.h"
#include "RooMinuit.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "PlotStyle.hpp"

#include "Plotter.hpp"
#include "ShapeMaker.hpp"

// =============================================
// Script to scan likelihood in piK signal yield
// =============================================
int main(int argc, char * argv[]) {

    // Check args
    if (argc > 4) {
        std::cout << "Usage: ./ToyLikelihood "
            "(--tight-pid/--previous-analysis/--with-2017/--tight-bdt --save "
            "(name number))" 
            << std::endl;
        return -1;
    }

    // Get option
    bool use_tight_cut = false;
    bool use_prev_ana = false;
    bool use_2017 = false;
    bool use_tight_bdt = false;
    bool save = false;
    bool plot = true;
    std::string name;
    std::string number;
    if (argc > 1) {
        std::string option = std::string(argv[1]);
        if (option == "--tight-pid") use_tight_cut = true;
        else if (option == "--previous-analysis") use_prev_ana = true;
        else if (option == "--with-2017") use_2017 = true;
        else if (option == "--tight-bdt") use_tight_bdt = true;
        else if (option == "--save") {
            save = true;
            plot = false;
            name = std::string(argv[2]);
            number = std::string(argv[3]);
            use_tight_cut = false;
            use_prev_ana = false;
        }
        else {
            std::cout << "Unknown option! Aborting." << std::endl;
            return -1;
        }
    }

    // Name of file with combined data fit
    std::string result_file;
    std::string plotname;
    if (use_tight_cut) {
        std::cout << "Using tighter PID cut" << std::endl;
        result_file = "../Results/twoBody_tightPID.root";
        plotname = "toy_tightPID";
    } else {
        std::cout << "Using looser cut" << std::endl;
        result_file = "../Results/all_2011:2012:2015:2016_"
            "combined_binned.root";
        plotname = "toy";
    }
    if (use_prev_ana) {
        std::cout << "Using previous analysis yields" << std::endl;
        plotname = "toy_previousAnalysis";
    }
    if (use_2017) {
        std::cout << "Using estimated 2017 contributions" << std::endl;
        plotname = "toy_with2017";
    }
    if (use_tight_bdt) {
        std::cout << "Using tighter BDT cut" << std::endl;
        result_file = "../Results/BDT_studies/twoBody_2011:2012:2015:2016_"
            "combined_binned_0_0_0.5_0.5.root";
        plotname = "toy_tightBDT";
    }

    // Make mass variable
    RooRealVar * Bd_M = new RooRealVar("Bd_ConsD_MD", "", 5000, 5800);
    int binWidth = 8;
    double nBins = (Bd_M->getMax() - Bd_M->getMin()) / binWidth;
    Bd_M->setBins(nBins);

    // Make ShapeMaker
    ShapeMaker * sm = new ShapeMaker("Y", Bd_M, true);

    // Make tree to hold results if saving
    std::string filename = ".temp.root";
    if (save) filename = "/data/lhcb/users/pullen/B02DKstar/toys/significance/" 
        + name + "_" + number + ".root";
    TFile * file = TFile::Open(filename.c_str(), "RECREATE");
    TTree * toy_tree = new TTree("toy_tree", "toy_tree");
    RooFitResult * result_noPiK;
    TString names[100] = {""};
    double init_values[100] = {0};
    double signal_values[100] = {0};
    double signal_errors[100] = {0};
    double nosignal_values[100] = {0};
    double nosignal_errors[100] = {0};
    // std::map<std::string, double> result_map;
    int n_vars = 0;
    double significance;
    int status;

    // If saving, set up branches in toy tree
    if (save) {
        // Make PDF to get parameter names
        sm->makeGenerationPdf(result_file);
        RooSimultaneous * fit_pdf = sm->makeZeroYieldPdf();
        RooArgSet * param_list = fit_pdf->getParameters(RooArgList(*Bd_M,
                    *sm->getCategory()));
        TIterator * it_param = param_list->createIterator();
        RooRealVar * param = 0;

        // Fill map with floating parameter names and empty values
        while((param = (RooRealVar*)it_param->Next())) {
            if (!param->isConstant()) {
                names[n_vars] = param->GetName();
                n_vars++;
            }
        }
        std::cout << "Will fill tree with " << n_vars << " variables." << 
            std::endl;

        // Set up branches in toy tree
        for (int i = 0; i < n_vars; i++) {
            toy_tree->Branch("init_value_" + names[i], &init_values[i],
                    "init_value_" + names[i] + "/D");
            toy_tree->Branch("signal_value_" + names[i], &signal_values[i],
                    "signal_value_" + names[i] + "/D");
            toy_tree->Branch("signal_error_" + names[i], &signal_errors[i],
                    "signal_error_" + names[i] + "/D");
            toy_tree->Branch("nosignal_value_" + names[i], &nosignal_values[i],
                    "nosignal_value_" + names[i] + "/D");
            toy_tree->Branch("nosignal_error_" + names[i], &nosignal_errors[i],
                    "nosignal_error_" + names[i] + "/D");
        }
        toy_tree->Print();
    }

    // Set significance branch
    if (save) {
        toy_tree->Branch("significance", &significance, "significance/D");
        toy_tree->Branch("status", &status, "status/I");
    }

    // Begin loop for multiple toys
    double n_toys = (save) ? 2:1;
    for (int i = 0; i < n_toys; i++) {

        // Get toy PDF
        RooSimultaneous * toyPdf;
        if (use_prev_ana) {
            toyPdf = sm->makePreviousAnalysisPdf();
        } else if (use_2017) {
            // toyPdf = sm->make_2017_pdf();
        } else {
            toyPdf = sm->makeGenerationPdf(result_file);
        }
        int expectedEvents = toyPdf->expectedEvents(*sm->getCategory());

        // Generate dataset
        RooDataSet * data = toyPdf->generate(RooArgList(*Bd_M, *sm->getCategory()),
                expectedEvents);

        // Make binned dataset
        RooDataHist * data_hist = (RooDataHist*)data->binnedClone();

        // Separate categories of data for plotting
        std::map<std::string, RooDataSet*> dataMap;
        dataMap["Kpi"] = (RooDataSet*)data->reduce("category == category::Kpi");
        dataMap["piK"] = (RooDataSet*)data->reduce("category == category::piK");
        dataMap["KK"] = (RooDataSet*)data->reduce("category == category::KK");
        dataMap["pipi"] = (RooDataSet*)data->reduce("category == category::pipi");
        dataMap["Kpipipi"] = (RooDataSet*)data->reduce("category == category::Kpipipi");
        dataMap["piKpipi"] = (RooDataSet*)data->reduce("category == category::piKpipi");
        dataMap["pipipipi"] = (RooDataSet*)data->reduce("category == category::pipipipi");

        // Fit with free yield
        RooSimultaneous * fitPdf = sm->makeFitPdf(false);
        RooFitResult * result_floating = fitPdf->fitTo(*data_hist, RooFit::Save(),
                RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true),
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
        if (plot) {
            result_floating->Print("v");
            sm->saveFitHistograms("../Histograms/" + plotname + "_freePiK.root", dataMap,
                    false);
        }
        double nll_floating = result_floating->minNll();

        // Fit with piK yield fixed to zero
        RooSimultaneous * fitPdf_noPiK = sm->makeZeroYieldPdf();
        result_noPiK = fitPdf_noPiK->fitTo(*data_hist, RooFit::Save(),
                RooFit::NumCPU(8, 2), RooFit::Optimize(false), RooFit::Offset(true),
                RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
        if (plot) {
            result_noPiK->Print("v");
            sm->saveFitHistograms("../Histograms/" + plotname + "_zeroPiK.root", dataMap,
                    false);
        }
        double nll_noPiK = result_noPiK->minNll();

        // Calculate significance
        significance = sqrt(2 * (nll_noPiK - nll_floating));

        // Plot
        if (plot) {
            Plotter * plotter = new Plotter(false);
            plotter->plotFourModeFitsCombined("../Histograms/" + plotname + "_freePiK.root",
                    plotname + "_freePiK", "");
            plotter->plotFourModeFitsCombined("../Histograms/" + plotname + "_zeroPiK.root",
                    plotname + "_zeroPiK", "");
            plotter->plotFourBodyFitsCombined("../Histograms/" + plotname + "_freePiK.root",
                    plotname + "_freePiK", "");
            plotter->plotFourBodyFitsCombined("../Histograms/" + plotname + "_zeroPiK.root",
                    plotname + "_zeroPiK", "");
        }

        // Print results
        std::cout << "Ratio between piK and Kpi obtained: " <<
            sm->getFitVariable("R_piK_vs_Kpi")->getVal() << std::endl;
        std::cout << "Minimum NLL with free piK yield: " << nll_floating << std::endl;
        std::cout << "Minimum NLL with zero piK yield: " << nll_noPiK << std::endl;
        std::cout << "Significance: " << significance << std::endl;

        // Fill tree if saving results
        if (save) {

            // Get fit status 
            status = result_floating->status() + result_noPiK->status();

            // Fill floating fit parameters (and initial values)
            file->cd();
            RooArgList float_vars = result_floating->floatParsFinal();
            RooArgSet * init_vars = toyPdf->getParameters(RooArgList(*Bd_M,
                        *sm->getCategory()));
            TIterator * it_float = float_vars.createIterator();
            RooRealVar * var;
            RooRealVar * init_var;
            TString current_name;
            while ((var = (RooRealVar*)it_float->Next())) {
                current_name = var->GetName();
                for (int j = 0; j < n_vars; j++) {
                    if (strcmp(current_name, names[j]) == 0) {
                        signal_values[j] = var->getVal();
                        signal_errors[j] = var->getError();
                        init_var = (RooRealVar*)init_vars->find("toy_" + current_name);
                        init_values[j] = init_var->getVal();
                    }
                }
            }

            // Fill zero signal fit parameters
            file->cd();
            RooArgList nosignal_vars = result_noPiK->floatParsFinal();
            TIterator * it_nosignal = nosignal_vars.createIterator();
            while ((var = (RooRealVar*)it_nosignal->Next())) {
                current_name = var->GetName();
                for (int k = 0; k < n_vars; k++) {
                    if (strcmp(current_name, names[k]) == 0) {
                        nosignal_values[k] = var->getVal();
                        nosignal_errors[k] = var->getError();
                    }
                }
            }

            // Fill tree
            toy_tree->Fill();
        }
        significance = 0;
    }

    // Open file, write tree, save
    if (save) {
        file->cd();
        result_noPiK->Write(); // Use this to get list of variables later
        toy_tree->Write();
        file->Close();
    }

    return 0;
}

