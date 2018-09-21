#include <iostream>
#include <string>
#include <fstream>

#include "TCut.h"
#include "TEventList.h"
#include "TFile.h"
#include "TTree.h"

#include "CutReader.hpp"

// ======================================
// Script to apply selection to an nTuple
// ======================================


int main(int argc, char * argv[]) {

    // Check args
    if (argc != 4 && argc !=5) {
        std::cout << "Usage: ./MakeSelectedTuple <2011/2012/2015/2016> <up/down> " <<
            "<D0mode> (<--fullD/--Bmass/--noBDTcut>)" << std::endl;
        return -1;
    }

    // Check fifth arg
    bool full_D = false;
    bool B_mass = false;
    bool no_BDT_cut = false;
    if (argc == 5) {
        if (std::string(argv[4]) == "--fullD") {
            full_D = true;
            std::cout << "Making an nTuple with full D0 mass and FDS range" 
                << std::endl;
        } else if (std::string(argv[4]) == "--Bmass") {
            B_mass = true;
            std::cout << "Making an nTuple with B0 mass window only"
                << std::endl;
        } else if (std::string(argv[4]) == "--noBDTcut") {
            no_BDT_cut = true;
            std::cout << "Making an nTuple with no cut on BDT response"
                << std::endl;
        } else {
            std::cout << "Input arg \"" << argv[4] << "\" not recognised." <<
                std::endl;
            return -1;
        }
    }

    // Get year, mode, polarity
    std::string year = argv[1];
    std::string mag = argv[2];
    std::string mode = argv[3];
    std::string bod = (mode == "Kpipipi" || mode == "piKpipi" || mode == "pipipipi")
        ? "fourBody" : "twoBody";

    // Name of input tuple
    std::string infile_name = "/data/lhcb/users/pullen/B02DKstar/data/" + bod + "/"
        + year + "_" + mag + "/" + mode + "_withBDTG.root";

    // Read input file and tree
    TFile * infile = TFile::Open(infile_name.c_str(), "READ");
    TTree * intree = (TTree*)infile->Get("DecayTree");

    // Make output file
    std::string outfile_name = "/data/lhcb/users/pullen/B02DKstar/data/" + bod + "/"
        + year + "_" + mag + "/" + mode + "_selected";
    if (full_D) outfile_name += "_full_D0_mass";
    else if (B_mass) outfile_name += "_B_mass_window";
    else if (no_BDT_cut) outfile_name += "_no_BDT_cut";
    outfile_name += ".root";
    TFile * outfile = TFile::Open(outfile_name.c_str(), "RECREATE");

    // Read in cut
    CutReader * cr = new CutReader(mode);

    // Copy tree with selection applied
    TCut cut;
    if (full_D) {
        std::vector<std::string> to_ignore = {"D0_M ", "D0_FDS"};
        cut = cr->GetCutExcept(to_ignore);
    } else if (no_BDT_cut) {
        cut = cr->GetCutExcept("BDTG");
    } else {
        cut = cr->GetCut();
    }
    if (B_mass) {
        cut += "abs(Bd_ConsD_MD - 5279.81) < 25.0";
    }
    // std::cout << "Cut: " << cut << std::endl;

    // Setup for multiple candidates
    unsigned long long evtNum;
    intree->SetBranchAddress("eventNumber", &evtNum);
    std::map<unsigned long long, std::vector<int>> emap;

    // Loop through and list candidates for each event
    intree->Draw(">> elist", cut);
    TEventList * elist = (TEventList*)gDirectory->Get("elist");
    for (int i = 0; i < elist->GetN(); i++) {
        intree->GetEntry(elist->GetEntry(i));
        if (emap.find(evtNum) == emap.end()) {
            emap[evtNum] = {i};
        } else {
            emap[evtNum].push_back(i);
        }
    }
 
    // Set up branch addresses for Bd FDS calculation
    double Bd_Z, Bd_ZERR;
    float PV_Z, PV_ZERR;
    intree->SetBranchAddress("Bd_ENDVERTEX_Z", &Bd_Z);
    intree->SetBranchAddress("PVZ", &PV_Z);
    intree->SetBranchAddress("Bd_ENDVERTEX_ZERR", &Bd_ZERR);
    intree->SetBranchAddress("PVZERR", &PV_ZERR);

    // Fill new tree with multiple candidates removed (keep largest Bd_FDS)
    outfile->cd();
    TTree * outtree = intree->CloneTree(0);
    for (int i = 0; i < elist->GetN(); i++) {
        intree->GetEntry(elist->GetEntry(i));
        if (emap[evtNum].size() == 1) {
            outtree->Fill();
        } else {

            // Check Bd_FDS for other candidates with same event number
            bool largest = true;
            double current_FDS = (Bd_Z - PV_Z)/sqrt(Bd_Z*Bd_Z + PV_Z*PV_Z);
            for (auto cand : emap[evtNum]) {
                if (cand == i) continue;
                intree->GetEntry(cand);
                double new_FDS = (Bd_Z - PV_Z)/sqrt(Bd_Z*Bd_Z + PV_Z*PV_Z);
                if (new_FDS > current_FDS) largest = false;
            }

            // If it's the largest, keep it
            if (largest) {
                intree->GetEntry(i);
                outtree->Fill();
            }
        }
    }

    // Save
    outfile->cd();
    outtree->Write("DecayTree");
    delete intree;
    intree = 0;
    delete outtree;
    outtree = 0;
    outfile->Close();
    infile->Close();

    // Count multiples and write to file
    int count_multiple = 0;
    int count_all = 0;
    for (auto e : emap) {
        count_all++;
        if (e.second.size() > 1) {
            count_multiple++;
        }
    }
    std::ofstream count_file("multiple_candidates/" + mode + "/" + year + "_" + mag + ".param");
    count_file << count_multiple << " " << count_all << std::endl;
    count_file.close();

}

