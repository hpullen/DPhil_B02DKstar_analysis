#include <iostream>
#include <string>

#include "TChain.h"
#include "TString.h"
#include "TCut.h"

// ================================================
// Script to find number of events passing stripping
// ================================================
int main(int argc, char * argv[]) {

    // Input directory
    TString mode = argv[1]; 
    TChain * tree = new TChain("Tuple_" + mode + "/DecayTree");
    for (int i = 2; i < argc; i++) {
        tree->Add(argv[i]);
    }

    // Total entries
    std::cout << "Events passing stripping: " << tree->GetEntries() << std::endl;

    // Entries passing trigger cut
    TCut cut;
    cut += "Bd_L0Global_TIS || Bd_L0HadronDecision_TOS";
    cut += "Bd_Hlt1TrackMVADecision_TOS || Bd_Hlt1TwoTrackMVADecision_TOS";
    cut += "Bd_Hlt2Topo2BodyDecision_TOS || Bd_Hlt2Topo3BodyDecision_TOS || "
        "Bd_Hlt2Topo4BodyDecision_TOS";
    std::cout << "Events passing stripping and trigger: " << tree->GetEntries(cut) 
        << std::endl;




}
