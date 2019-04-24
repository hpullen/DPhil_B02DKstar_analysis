#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include "TGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TwoAndFourBodyFitter.hpp"

// ========================================================
// Perform a fit with a parameter fixed to different values
// ========================================================
int main(int argc, char * argv[]) {

    // Get input args
    if (argc != 5) {
        std::cout << "Usage: ./ProfileLikelihood <par> <min> <max> <step>" 
            << std::endl;
        return -1;
    }
    std::string par = argv[1];
    double min = atof(argv[2]);
    double max = atof(argv[3]);
    double step = atof(argv[4]);

    // Make fitter
    std::vector<std::string> modes = {"Kpi", "piK", "KK", "pipi", "Kpipipi", 
        "piKpipi", "pipipipi"};
    TwoAndFourBodyFitter * fitter = new TwoAndFourBodyFitter(true,
            Data::Run::Both, modes, false);

    // Add data files
    std::string data_path = "/data/lhcb/users/pullen/B02DKstar/data/";
    for (std::string mode : modes) {
        std::string bod = (mode == "Kpipipi" || mode == "piKpipi" 
                || mode == "pipipipi") ? "fourBody/" : "twoBody/";
        for (std::string mag : {"_up", "_down"}) { 
            for (std::string year : {"2015", "2016"}) {
                fitter->AddFile(mode + "_run2", data_path + bod + year + mag + 
                        "/" + mode + "_selected.root");
            }
            if (mode != "pipipipi") {
                for (std::string year : {"2011", "2012"}) {
                    fitter->AddFile(mode + "_run1", data_path + bod + year + mag + 
                            "/" + mode + "_selected.root");
                }
            }
        }
    }

    // Setup arrays for graph
    Double_t val[100], sigma[100];

    // Make file to save results
    std::ofstream outfile("profile_likelihood/" + par + ".txt");

    // Get minimised likelihood
    double min_nll = fitter->PerformFit("profile_likelihood/fit_result.root", "profile_likelihood/histograms.root", true);
    outfile << "Min NLL: " << min_nll << std::endl;

    // Perform fits
    int n = 0;
    double current_val = min;
    while (current_val <= max) {
        val[n] = current_val;
        double nll = fitter->PerformFixedParFit(par, current_val, 
                "profile_likelihood/fit_result.root", "profile_likelihood/histograms.root", true);
        sigma[n] = sqrt(2*(nll - min_nll));
        outfile << par << " = " << current_val << ": NLL = " << nll << std::endl;
        current_val += step;
        n++;
    }

    // Plot
    TGraph * gr = new TGraph(n, val, sigma);
    gr->SetLineColor(2);
    gr->SetLineWidth(2);
    gr->SetMarkerColor(2);
    gr->SetMarkerStyle(21);
    gr->SetTitle("");
    gr->GetXaxis()->SetTitle(par.c_str());
    gr->GetYaxis()->SetTitle("#sigma=#surd(2#times#Delta(log(L)))");
    TCanvas * canvas = new TCanvas("canv", "", 900, 900);
    gr->Draw("ACP");
    canvas->SaveAs(("profile_likelihood/" + par + ".pdf").c_str());
    outfile.close();
}
