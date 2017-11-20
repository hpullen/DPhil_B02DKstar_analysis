#include <fstream>
#include <iostream>
#include <map>

#include "TApplication.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"

#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooAbsPdf.h"
#include "RooCategory.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooHist.h"
#include "RooPlotable.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "Plotter.hpp"
#include "RooHORNSdini.h"
#include "RooHILLdini.h"
#include "RooLITTLEHORNSdini.h"

// Program for fitting to low mass background MC shapes
int main(int argc, char * argv[]) {

    // Make TApplication to display ouput
    //TApplication * app = new TApplication("app", &argc, argv);

    // Make TChains to hold data
    std::map<std::string, TChain *> treeMap;
    treeMap["gamma_010"] = new TChain("DecayTree");
    treeMap["gamma_101"] = new TChain("DecayTree");
    treeMap["pi_010"] = new TChain("DecayTree");
    treeMap["pi_101"] = new TChain("DecayTree");
    treeMap["Bs_gamma_010"] = new TChain("DecayTree");
    treeMap["Bs_gamma_101"] = new TChain("DecayTree");
    treeMap["Bs_pi_010"] = new TChain("DecayTree");
    treeMap["Bs_pi_101"] = new TChain("DecayTree");

    // Location of data files
    std::string path = "/data/lhcb/users/pullen/B02DKstar/MC/backgrounds/";
    
    // B0 gamma 010
    // Note: use withBDTG here because selected tuples don't contain full mass range.
    treeMap["gamma_010"]->Add((path + 
                "lowMass/gamma/010/2016_down/Kpi_withVars_withCuts.root").c_str());
    treeMap["gamma_010"]->Add((path + 
                "lowMass/gamma/010/2016_up/Kpi_withVars_withCuts.root").c_str());

    // B0 gamma 100 and 001
    treeMap["gamma_101"]->Add((path + 
                "lowMass/gamma/100/2016_down/Kpi_withVars_withCuts.root").c_str());
    treeMap["gamma_101"]->Add((path + 
                "lowMass/gamma/100/2016_up/Kpi_withVars_withCuts.root").c_str());
    treeMap["gamma_101"]->Add((path + 
                "lowMass/gamma/001/2016_down/Kpi_withVars_withCuts.root").c_str());
    treeMap["gamma_101"]->Add((path + 
                "lowMass/gamma/001/2016_up/Kpi_withVars_withCuts.root").c_str());

    // B0 pi 010
    treeMap["pi_010"]->Add((path + 
                "lowMass/pi/010/2016_down/Kpi_withVars_withCuts.root").c_str());
    treeMap["pi_010"]->Add((path + 
                "lowMass/pi/010/2016_up/Kpi_withVars_withCuts.root").c_str());

    // B0 pi 100 and 001
    treeMap["pi_101"]->Add((path + 
                "lowMass/pi/100/2016_down/Kpi_withVars_withCuts.root").c_str());
    treeMap["pi_101"]->Add((path + 
                "lowMass/pi/100/2016_up/Kpi_withVars_withCuts.root").c_str());
    treeMap["pi_101"]->Add((path + 
                "lowMass/pi/001/2016_down/Kpi_withVars_withCuts.root").c_str());
    treeMap["pi_101"]->Add((path + 
                "lowMass/pi/001/2016_up/Kpi_withVars_withCuts.root").c_str());

    // Bs gamma 010
    treeMap["Bs_gamma_010"]->Add((path + 
                "Bs_lowMass/gamma/010/2016_down/Kpi_withVars_withCuts.root").c_str());
    treeMap["Bs_gamma_010"]->Add((path + 
                "Bs_lowMass/gamma/010/2016_up/Kpi_withVars_withCuts.root").c_str());

    // Bs gamma 100 and 001
    treeMap["Bs_gamma_101"]->Add((path + 
                "Bs_lowMass/gamma/100/2016_down/Kpi_withVars_withCuts.root").c_str());
    treeMap["Bs_gamma_101"]->Add((path + 
                "Bs_lowMass/gamma/100/2016_up/Kpi_withVars_withCuts.root").c_str());
    treeMap["Bs_gamma_101"]->Add((path + 
                "Bs_lowMass/gamma/001/2016_down/Kpi_withVars_withCuts.root").c_str());
    treeMap["Bs_gamma_101"]->Add((path + 
                "Bs_lowMass/gamma/001/2016_up/Kpi_withVars_withCuts.root").c_str());

    // Bs pi 010
    treeMap["Bs_pi_010"]->Add((path + 
                "Bs_lowMass/pi/010/2016_down/Kpi_withVars_withCuts.root").c_str());
    treeMap["Bs_pi_010"]->Add((path + 
                "Bs_lowMass/pi/010/2016_up/Kpi_withVars_withCuts.root").c_str());

    // Bs pi 100 and 001
    treeMap["Bs_pi_101"]->Add((path + 
                "Bs_lowMass/pi/100/2016_down/Kpi_withVars_withCuts.root").c_str());
    treeMap["Bs_pi_101"]->Add((path + 
                "Bs_lowMass/pi/100/2016_up/Kpi_withVars_withCuts.root").c_str());
    treeMap["Bs_pi_101"]->Add((path + 
                "Bs_lowMass/pi/001/2016_down/Kpi_withVars_withCuts.root").c_str());
    treeMap["Bs_pi_101"]->Add((path + 
                "Bs_lowMass/pi/001/2016_up/Kpi_withVars_withCuts.root").c_str());

    // Set variables
    RooRealVar Bd_M("Bd_ConsD_MD", "", 4800, 5450, "MeV/c^{2}");
    RooRealVar D0_M("D0_M", "", 1864.83 - 25, 1864.83 + 25);
    RooRealVar D0_FDS("D0_FDS", "", 2, 100000000);
    RooRealVar BDTG("BDTG_Kpi_run2", "", 0.5, 1);
    RooRealVar BKGCAT("Bd_BKGCAT", "", 50);

    // Set up bins
    int binWidth = 5;
    double nBins = (Bd_M.getMax() - Bd_M.getMin()) / binWidth;
    Bd_M.setBins(nBins);

    // Make args list
    RooArgList args;
    args.add(Bd_M);
    args.add(D0_M);
    args.add(D0_FDS);
    args.add(BDTG);
    args.add(BKGCAT);

    // Make RooDataSets
    std::map<std::string, RooDataSet *> dataMap;
    for (auto tree : treeMap) {
        dataMap[tree.first] = new RooDataSet(("data_" + tree.first).c_str(), "",
                    tree.second, args);
    }
    
    // Gamma 010 fit parameters
    RooRealVar a_gamma("a_gamma", "", 4919.1);
    RooRealVar b_gamma("b_gamma", "", 5225.5, 5222, 5228);
    RooRealVar csi_gamma_010("csi_gamma_010", "", 0.6, 0.2, 1);
    RooRealVar sigma_gamma_010("sigma_gamma_010", "", 15, 0, 50);
    RooRealVar ratio("ratio", "", 8, 2, 15);
    RooRealVar frac("frac", "", 0.9, 0, 1);
    RooRealVar shift("shift", "", 0);

    // Gamma 101 fit parameters
    RooRealVar csi_gamma_101("csi_gamma_101", "", 0.7, 0.2, 1);
    RooRealVar sigma_gamma_101("sigma_gamma_101", "", 15, 0, 50);

    // Pi 010 fit parameters
    RooRealVar a_pi("a_pi", "", 5021.6, 5020, 5023);
    RooRealVar b_pi("b_pi", "", 5116.9, 5112, 5118);
    RooRealVar csi_pi_010("csi_pi_010", "", 1, 0.6, 2.5);
    RooRealVar sigma_pi_010("sigma_pi_010", "", 15, 0, 50);

    // Pi 101 fit parameters
    RooRealVar csi_pi_101("csi_pi_101", "", 1, 0, 2);
    RooRealVar sigma_pi_101("sigma_pi_101", "", 15, 0, 50);

    // Make B0 shapes
    std::map<std::string, RooAbsPdf *> pdfMap;
    pdfMap.emplace("gamma_010", new RooHILLdini("gamma_010", "", Bd_M, a_gamma, 
                b_gamma, csi_gamma_010, shift, sigma_gamma_010, ratio, frac));
    pdfMap.emplace("gamma_101", new RooLITTLEHORNSdini("gamma_101", "", Bd_M, 
            a_gamma, b_gamma, csi_gamma_101, shift, sigma_gamma_101, ratio, frac,
            shift));
    pdfMap.emplace("pi_010", new RooHORNSdini("pi_010", "", Bd_M, a_pi, b_pi, 
            csi_pi_010, shift, sigma_pi_010, ratio, frac));
    pdfMap.emplace("pi_101", new RooHILLdini("pi_101", "", Bd_M, a_pi, b_pi, 
            csi_pi_101, shift, sigma_pi_101, ratio, frac));

    // Bs gamma 010 fit parameters
    RooRealVar delta_M("delta_M", "", 87.29, 87.29 - 5, 87.29 + 5);
    RooFormulaVar Bs_a_gamma("Bs_a_gamma", "", "@0 + @1", RooArgList(a_gamma, 
                delta_M));
    RooFormulaVar Bs_b_gamma("Bs_b_gamma", "", "@0 + @1", RooArgList(b_gamma, 
                delta_M));
    RooRealVar Bs_sigma_gamma_010("Bs_sigma_gamma_010", "", 15, 0, 50);
    RooRealVar Bs_csi_gamma_010("Bs_csi_gamma_010", "", 0.7, 0.05, 1);

    // Bs gamma 101 fit parameters
    RooRealVar Bs_sigma_gamma_101("Bs_sigma_gamma_101", "", 15, 0, 50);
    RooRealVar Bs_csi_gamma_101("Bs_csi_gamma_101", "", 0.7, 0.05, 1);

    // Bs pi 010 fit parameters
    RooFormulaVar Bs_a_pi("Bs_a_pi", "", "@0 + @1", RooArgList(a_pi, delta_M));
    RooFormulaVar Bs_b_pi("Bs_b_pi", "", "@0 + @1", RooArgList(b_pi, delta_M));
    RooRealVar Bs_sigma_pi_010("Bs_sigma_pi_010", "", 10, 0, 50);
    RooRealVar Bs_csi_pi_010("Bs_csi_pi_010", "", 0.9, 0.6, 2);

    // Bs pi 101 fit parameters
    RooRealVar Bs_sigma_pi_101("Bs_sigma_pi_101", "", 15, 0, 50);
    RooRealVar Bs_csi_pi_101("Bs_csi_pi_101", "", 1, 0, 2);

    // Make Bs shapes
    pdfMap.emplace("Bs_gamma_010", new RooHILLdini("Bs_gamma_010", "", Bd_M, 
            Bs_a_gamma, Bs_b_gamma, Bs_csi_gamma_010, shift, Bs_sigma_gamma_010, 
            ratio, frac));
    pdfMap.emplace("Bs_gamma_101", new RooLITTLEHORNSdini("Bs_gamma_101", "", Bd_M, 
            Bs_a_gamma, Bs_b_gamma, Bs_csi_gamma_101, shift, Bs_sigma_gamma_101, 
            ratio, frac, shift));
    pdfMap.emplace("Bs_pi_010", new RooHORNSdini("Bs_pi_010", "", Bd_M, Bs_a_pi, 
            Bs_b_pi, Bs_csi_pi_010, shift, Bs_sigma_pi_010, ratio, frac));
    pdfMap.emplace("Bs_pi_101", new RooHILLdini("Bs_pi_101", "", Bd_M, Bs_a_pi, 
            Bs_b_pi, Bs_csi_pi_101, shift, Bs_sigma_pi_101, ratio, frac));

    // Create categories
    RooCategory category("category", "");
    for (auto tree : treeMap) {
        category.defineType(tree.first.c_str());
    }

    // Make combined dataset
    RooDataSet combData("comdData", "", Bd_M, RooFit::Index(category),
            RooFit::Import(dataMap));

    // Make simultaneous fit
    RooSimultaneous * simPdf = new RooSimultaneous("simPdf", "", category);

    // Add the PDFs to the simultaneous fit
    for (auto pdf : pdfMap) {
        simPdf->addPdf(*pdf.second, pdf.first.c_str());
    }

    // Perform the fit
    RooFitResult * r = simPdf->fitTo(combData, RooFit::Save(), RooFit::NumCPU(8, 2), 
            RooFit::Optimize(false), RooFit::Offset(true),
            RooFit::Minimizer("Minuit2", "migrad"), RooFit::Strategy(2));
    r->Print("v");

    // Save output to a text file
    std::ofstream params("../Results/lowMass.param");
    params << "a_gamma " << a_gamma.getVal() << " " << a_gamma.getError() << std::endl;
    params << "b_gamma " << b_gamma.getVal() << " " << b_gamma.getError() << std::endl;
    params << "a_pi " << a_pi.getVal() << " " << a_pi.getError() << std::endl;
    params << "b_pi " << b_pi.getVal() << " " << b_pi.getError() << std::endl;
    params << "frac " << frac.getVal() << " " << frac.getError() << std::endl;
    params << "ratio " << ratio.getVal() << " " << ratio.getError() << std::endl;
    params << "sigma_gamma_010 " << sigma_gamma_010.getVal() << " " << sigma_gamma_010.getError() << std::endl;
    params << "sigma_gamma_101 " << sigma_gamma_101.getVal() << " " << sigma_gamma_101.getError() << std::endl;
    params << "sigma_pi_010 " << sigma_pi_010.getVal() << " " << sigma_pi_010.getError() << std::endl;
    params << "sigma_pi_101 " << sigma_pi_101.getVal() << " " << sigma_pi_101.getError() << std::endl;
    params << "csi_gamma_010 " << csi_gamma_010.getVal() << " " << csi_gamma_010.getError() << std::endl;
    params << "csi_gamma_101 " << csi_gamma_101.getVal() << " " << csi_gamma_101.getError() << std::endl;
    params << "csi_pi_010 " << csi_pi_010.getVal() << " " << csi_pi_010.getError() << std::endl;
    params << "csi_pi_101 " << csi_pi_101.getVal() << " " << csi_pi_101.getError() << std::endl;
    params << "Bs_sigma_gamma_010 " << Bs_sigma_gamma_010.getVal() << " " << Bs_sigma_gamma_010.getError() << std::endl;
    params << "Bs_sigma_gamma_101 " << Bs_sigma_gamma_101.getVal() << " " << Bs_sigma_gamma_101.getError() << std::endl;
    params << "Bs_sigma_pi_010 " << Bs_sigma_pi_010.getVal() << " " << Bs_sigma_pi_010.getError() << std::endl;
    params << "Bs_sigma_pi_101 " << Bs_sigma_pi_101.getVal() << " " << Bs_sigma_pi_101.getError() << std::endl;
    params << "Bs_csi_gamma_010 " << Bs_csi_gamma_010.getVal() << " " << Bs_csi_gamma_010.getError() << std::endl;
    params << "Bs_csi_gamma_101 " << Bs_csi_gamma_101.getVal() << " " << Bs_csi_gamma_101.getError() << std::endl;
    params << "Bs_csi_pi_010 " << Bs_csi_pi_010.getVal() << " " << Bs_csi_pi_010.getError() << std::endl;
    params << "Bs_csi_pi_101 " << Bs_csi_pi_101.getVal() << " " << Bs_csi_pi_101.getError() << std::endl;
    params.close();

    // Open file to save output
    TFile * outfile = TFile::Open("../Histograms/lowMass.root", "RECREATE");

    // Save each dataset and fit as a TH1F
    for (auto data : dataMap) {

        std::cout << "Making TH1F for " << data.first << std::endl;

        // Make TH1Fs
        TH1F * h_data = (TH1F*)data.second->createHistogram(("data_" + 
                    data.first).c_str(), Bd_M);
        TH1F * h_fit = (TH1F*)pdfMap[data.first]->createHistogram(("fit" + 
                    data.first).c_str(), Bd_M, RooFit::Binning(nBins * 10));
        h_fit->Scale(h_data->Integral() * 10); 

        // Get pulls
        RooPlot * frame = Bd_M.frame(RooFit::Title(""));
        data.second->plotOn(frame);
        pdfMap[data.first]->plotOn(frame);
        RooHist * pulls = frame->pullHist();

        // Save histograms and pulls
        outfile->cd();
        h_data->Write(("data_" + data.first).c_str());
        h_fit->Write(("fit_" + data.first).c_str());
        pulls->Write(("pulls_" + data.first).c_str());
    }
    outfile->Close();

    // Plot results
    Plotter * plotter = new Plotter();
    plotter->plotLowMassFits();
    delete plotter;

    // Display results in TApplication
    //app->Run();

    return 0;
}
