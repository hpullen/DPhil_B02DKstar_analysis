#include <string>
#include <sstream>

#include "TFile.h"
#include "TGraphErrors.h"
#include "TTree.h"
#include "TCut.h"
#include "TString.h"

#include "RooRealVar.h"

#include "CutReader.hpp"



// ==================================================
// Function to get and save efficiencies for one file
// ==================================================
void GetFileEfficiencies(TString filename, TString outfile_name, TString mode, 
        TCut cut) {

    // Open file and get tree
    TFile * file = TFile::Open(filename, "READ");
    TTree * tree = (TTree*)file->Get("DecayTree");

    // Open output file
    TFile * outfile = TFile::Open(outfile_name, "RECREATE");

    // Make TGraph to plot efficiency
    TGraphErrors * graph = new TGraphErrors();

    std::cout << "Processing file " << filename << std::endl;
    // Loop through BDT cuts
    double loosest = 0.0;
    int count = 0;
    for (double i = 0.0; i < 1.0; i += 0.1) {

        // Construct cut
        std::stringstream ss;
        ss << std::setprecision(1) << i;
        TString cut_string = "BDTG_" + mode + "_run2 > " + ss.str().c_str();
        TCut BDT_cut = (TCut)cut_string;
        TCut full_cut = cut + BDT_cut;

        // Get efficiency and write to file
        double entries = tree->GetEntries(full_cut);
        std::cout << "entries at cut " << ss.str() << ": " << entries << std::endl;
        if (i < 0.00001) loosest = entries;
        std::string var_name = "efficiency_" + ss.str();
        RooRealVar * eff = new RooRealVar(var_name.c_str(), "", entries/loosest);
        outfile->cd();
        eff->Write(var_name.c_str());

        // Fill graph
        graph->SetPoint(count, i, entries/loosest);
        graph->SetPointError(count, 0.0, entries/loosest * 
                sqrt(1/entries + 1/loosest));
        count++;
    }

    outfile->cd();
    graph->Write("graph");
    outfile->Close();
    file->Close();
}




// =========================================
// Main function to process all efficiencies
// =========================================
int main(int argc, char * argv[]) {

    for (TString year : {"2011", "2012", "2015", "2016"}) {
        for (TString mag : {"up", "down"}) {
            for (TString mode : {"Kpi", "KK", "pipi", "Kpipipi", "pipipipi"}) {

                // Skip Run 1 if pipipipi
                if (mode == "pipipipi" && (year == "2011" || year == "2012")) continue;

                // Get n bodies
                bool is_fourBody = (mode == "Kpipipi" || mode == "pipipipi");
                TString bod = (is_fourBody) ? "four" : "two";

                // Input and output directories
                TString data_dir = "/data/lhcb/users/pullen/B02DKstar/";
                TString out_dir = "../Efficiencies/" + year + "_" + mag + "/";

                // Get cut for mode
                CutReader * cr = new CutReader(std::string(mode));
                std::vector<std::string> except = {"BDTG", "PID"};
                TCut cut_sig = cr->GetCutExcept(except);
                cut_sig += "Bd_BKGCAT == 0";
                TCut cut_bg = "Bd_ConsD_M > 5800";

                // Background
                TString bg_file = data_dir + "data/" + bod + "Body/" + year + "_" 
                    + mag + "/" + mode + "_withBDTG.root";
                std::cout << "Processing background " << mode << std::endl;
                GetFileEfficiencies(bg_file, out_dir + "background_" + mode + ".root",
                        mode, cut_bg);

                // Skip MC if four body and 2015
                if (is_fourBody && year == "2015") continue;

                // Skip MC if 2011 and not Kpi
                if (mode != "Kpi" && year == "2011") continue;

                // Monte Carlo signal
                TString sig_file = data_dir + "MC/" + bod + "Body/" + mode + "/" 
                    + year + "_" + mag + "/" + mode + "_withBDTG.root";
                std::cout << "Processing signal " << mode << std::endl;
                GetFileEfficiencies(sig_file, out_dir + "signal_" + mode + ".root",
                        mode, cut_sig);

            }

        }
    }
}

