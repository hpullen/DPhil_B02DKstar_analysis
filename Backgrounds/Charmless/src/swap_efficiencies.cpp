#include <map>
#include <iostream>

#include "TChain.h"
#include "TLorentzVector.h"
#include "TLegend.h"
#include "TH1F.h"
#include "TCanvas.h"

#include "CutReader.hpp"
#include "PlotStyle.hpp"

// ==============================================================
// Script to look for possible mis-ID swaps in D sidebands of piK
// ==============================================================
// Helper function for calculating helicity angle
inline double Product(const TLorentzVector& p1, const TLorentzVector& p2,
        const TLorentzVector& pX) {
    return - p1.Dot(p2) + p1.Dot(pX) * p2.Dot(pX) / pX.Dot(pX);
}

// Helper function for setting up Lorentz vector branch
void LorentzVectorBranch(TTree* tree, TLorentzVector& vec,
        const std::string& name) {
    tree->SetBranchAddress((name + "_PE").c_str(), &vec[3]);
    tree->SetBranchAddress((name + "_PX").c_str(), &vec[0]);
    tree->SetBranchAddress((name + "_PY").c_str(), &vec[1]);
    tree->SetBranchAddress((name + "_PZ").c_str(), &vec[2]);
}

void process(TTree * tree, const TString & D0K, const TString & D0Pi, 
        const TString & KstarK, const TString & KstarPi) {

    // "Reconstructed" D0 and K*0 masses
    TString D0_M_swap = "sqrt(pow(" + D0K + "_PE + " + D0Pi + "_PE, 2)"
        " - pow(" + D0K + "_PX + " + D0Pi + "_PX, 2)"
        " - pow(" + D0K + "_PY + " + D0Pi + "_PY, 2)"
        " - pow(" + D0K + "_PZ + " + D0Pi + "_PZ, 2))";
    TString Kstar_M = "sqrt(pow(" + KstarK + "_PE + " + KstarPi + "_PE, 2)"
        " - pow(" + KstarK + "_PX + " + KstarPi + "_PX, 2)"
        " - pow(" + KstarK + "_PY + " + KstarPi + "_PY, 2)"
        " - pow(" + KstarK + "_PZ + " + KstarPi + "_PZ, 2))";
    TString D0_M = "sqrt(pow(sqrt(pow(" + D0K + "_P, 2) + 139.6 * 139.6) + "
        "sqrt(pow(" + D0Pi + "_P, 2) + 493.7 * 493.7), 2)"
        " - pow(" + D0K + "_PX + " + D0Pi + "_PX, 2)"
        " - pow(" + D0K + "_PY + " + D0Pi + "_PY, 2)"
        " - pow(" + D0K + "_PZ + " + D0Pi + "_PZ, 2))";

    // Cuts
    TCut truth_cut = "Bd_BKGCAT == 0 && D0_BKGCAT == 0 && Kstar_BKGCAT == 0";
    TCut Kstar_cut = ("abs(" + std::string(Kstar_M) + " - 895.55) < 50").c_str();
    TCut D0_cut = ("abs(" + std::string(D0_M) + " - 1864.83) > 50").c_str();
    TCut D0_veto = ("abs(" + std::string(D0_M_swap) + " - 1864.83) > 15").c_str();
    TCut cut = truth_cut;

    // Plot the distributions
    TString name = "D0(" + D0K + "_" + D0Pi + ")_Kstar(" + KstarK + "_" + KstarPi + ")";
    TCanvas * canv = new TCanvas("canvas", "", 900, 600);
    canv->Clear();
    tree->Draw(D0_M, Kstar_cut + truth_cut);
    canv->SaveAs("Plots/checks/" + name + "_D0_M.pdf");
    canv->Clear();
    tree->Draw(Kstar_M, truth_cut);
    canv->SaveAs("Plots/checks/" + name + "_Kstar_M.pdf");
    canv->Clear();
    tree->Draw(D0_M_swap, Kstar_cut + D0_cut + truth_cut);
    canv->SaveAs("Plots/checks/" + name + "_D0_M_swap.pdf");

    // Plot helicity angle and get efficiency
    double helicityAngle;
    TLorentzVector KstarK_P, KstarPi_P, D0K_P, D0Pi_P;
    LorentzVectorBranch(tree, KstarK_P, std::string(KstarK));
    LorentzVectorBranch(tree, KstarPi_P, std::string(KstarPi));
    LorentzVectorBranch(tree, D0K_P, std::string(D0K));
    LorentzVectorBranch(tree, D0Pi_P, std::string(D0Pi));
    TH1F * hist_hel = new TH1F("hist_hel", "", 100, -100, 100);
    double count_tot = 0;
    double count_sel = 0;
    for (int i = 0; i < tree->GetEntries(); i++) {
        count_tot++;
        TLorentzVector D0_P = D0Pi_P + D0K_P;
        helicityAngle = Product(KstarK_P, D0_P, KstarK_P + KstarPi_P) / 
            sqrt(Product(KstarK_P, KstarK_P, KstarK_P + KstarPi_P) * 
                    Product(D0_P, D0_P, KstarK_P + KstarPi_P));
        hist_hel->Fill(helicityAngle);
        if (std::fabs(helicityAngle) > 0.4) count_sel++;
    }
    canv->Clear();
    hist_hel->SetLineWidth(1);
    hist_hel->Draw();
    canv->SaveAs("Plots/checks/" + name + "_helicity.pdf");
    delete canv;

    // Print efficiencies
    std::cout << "\nD0(" << D0K << ", " << D0Pi << ") + K*0(" 
        << KstarK << ", " << KstarPi << "):" << std::endl;
    std::cout << "Helicity angle efficiency: " << count_sel/count_tot
        << std::endl;
    std::cout << "K*0 mass cut efficiency: " << 
        (double)tree->GetEntries(Kstar_cut + cut) / (double)tree->GetEntries(cut)
        << std::endl;
    cut += Kstar_cut;
    std::cout << "D0 mass sideband efficiency: " << 
        (double)tree->GetEntries(D0_cut + cut) / (double)tree->GetEntries(cut)
        << std::endl;
    cut += D0_cut;
    std::cout << "D0 veto efficiency: " << 
        (double)tree->GetEntries(D0_veto + cut) / (double)tree->GetEntries(cut)
        << std::endl;
}


int main(int argc, char * argv[]) {

    // Read in Kpi MC
    std::string mode = "Kpi";
    std::string run = "2";
    TChain * tree = new TChain("Tuple_Kpi/DecayTree");
    setPlotStyle();
    std::string dir = "/data/lhcb/users/pullen/B02DKstar/MC/twoBody/Kpi/";
    for (std::string mag : {"up", "down"}) {
        if (run == "1" || run == "both") {
            tree->Add((dir + "/2011_" + mag + "/Total.root").c_str());
            tree->Add((dir + "/2012_" + mag + "/Total.root").c_str());
        } if (run == "2" || run == "both") {
            tree->Add((dir + "/2015_" + mag + "/Total.root").c_str());
            tree->Add((dir + "/2016_" + mag + "/Total.root").c_str());
        }
    }

    // Process both cases
    process(tree, "KstarK", "D0Pi", "D0K", "KstarPi");
    process(tree, "D0K", "KstarPi", "KstarK", "D0Pi");

}
