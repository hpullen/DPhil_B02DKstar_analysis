inline double Product(const TLorentzVector& p1, const TLorentzVector& p2,
        const TLorentzVector& pX) {
    return - p1.Dot(p2) + p1.Dot(pX) * p2.Dot(pX) / pX.Dot(pX);
}

void LorentzVectorBranch(TTree* tree, TLorentzVector& vec,
        const std::string& name) {
    tree->SetBranchAddress((name + "_E").c_str(), &vec[3]);
    tree->SetBranchAddress((name + "_PX").c_str(), &vec[0]);
    tree->SetBranchAddress((name + "_PY").c_str(), &vec[1]);
    tree->SetBranchAddress((name + "_PZ").c_str(), &vec[2]);
}


std::pair<std::string, std::string> GetKstarDaughterNames(std::string mode_dir) {

    if (mode_dir.find("/rho") != std::string::npos) {
        return std::make_pair("Kp_0", "pim_1");
    } else if (mode_dir.find("3_K1_1400/non_res") != std::string::npos ||
            mode_dir.find("4_Kstar_1430") != std::string::npos ||
            mode_dir.find("10_nonres") != std::string::npos) {
        return std::make_pair("Kp_0", "pim_0");
    } else {
        return std::make_pair("Kp_1", "pim_1");
    }
}

std::pair<TString, TString> GetDalitzVars(std::string mode_dir) {

    // if (mode_dir.find("3_K1_1400/Kstar") != std::string::npos) {
        // TString D0_Kstar_M2 = "pow(D0b_0_E + Kst0_0_E, 2) - "
            // "pow(D0b_0_PX + Kst0_0_PX, 2) - pow(D0b_0_PY + Kst0_0_PY, 2) - "
            // "pow(D0b_0_PZ + Kst0_0_PZ, 2)";
        // TString Kstar_pi_M2 = "pow(pip_0_E + Kst0_0_E, 2) - "
            // "pow(pip_0_PX + Kst0_0_PX, 2) - pow(pip_0_PY + Kst0_0_PY, 2) - "
            // "pow(pip_0_PZ + Kst0_0_PZ, 2)";
        // return std::make_pair(D0_Kstar_M2, Kstar_pi_M2);
    // }

    return std::make_pair("D0_Kstar_M2_TRUE", "Kstar_pi_M2_TRUE");


}




// ================================================================
// Script for making plots for a component of the DKpipi background
// ================================================================
void MakeModePlots(TString mode_dir) {

    // Open tree
    TFile * file = TFile::Open("/data/lhcb/users/pullen/RapidSim/DKpipi/" + 
            mode_dir + "/tree.root", "UPDATE");
    TTree * tree = (TTree*)file->Get("DecayTree");

    // Make mass plot after daughter mass cuts
    TH1F * mass_hist = new TH1F("mass_hist", "", 500, 2600, 5300);
    TCut cut = "abs(Kstar_M - 0.8955) < 0.05 && abs(D0_M - 1.86484) < 0.025";
    tree->Draw("Bd_M * 1000 >> mass_hist", cut);

    // Save mass plot
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    mass_hist->SetLineWidth(1);
    mass_hist->GetXaxis()->SetTitle("#it{m}(B^{0}) [MeV/#it{c}^{2}]");
    mass_hist->GetYaxis()->SetTitle("Candidates");
    mass_hist->Draw();
    canvas->SaveAs("../Plots/Components/" + mode_dir + "/B0_mass.pdf");

    // Make helicity angle plot
    TH1F * helicity_hist = new TH1F("helicity_hist", "", 500, -1, 1);
    tree->Draw(">> elist", cut);
    TEventList * elist = (TEventList*)gDirectory->Get("elist");

    // Setup for angle calculation
    double helicity_angle;
    TLorentzVector D0_P;
    TLorentzVector KstarK_P;
    TLorentzVector KstarPi_P;
    LorentzVectorBranch(tree, D0_P, "D0b_0");
    std::pair<std::string, std::string> daughters = 
        GetKstarDaughterNames(std::string(mode_dir));
    LorentzVectorBranch(tree, KstarK_P, daughters.first);
    LorentzVectorBranch(tree, KstarPi_P, daughters.second);

    // Make selected tree
    TTree * sel_tree = tree->CloneTree(0);
    sel_tree->Branch("helicity_angle", &helicity_angle, "helicity_angle/D");

    // Calculate angle
    for (int i = 0; i < elist->GetN(); i++) {
        int evt = elist->GetEntry(i);
        tree->GetEntry(evt);
        helicity_angle = Product(KstarK_P, D0_P, KstarK_P + KstarPi_P) / 
            sqrt(Product(KstarK_P, KstarK_P, KstarK_P + KstarPi_P) * 
                    Product(D0_P, D0_P, KstarK_P + KstarPi_P));
        helicity_hist->Fill(helicity_angle);
        sel_tree->Fill();
    }

    // Make Dalitz plot
    TH2F * dalitz_hist = new TH2F("dalitz_hist", "", 100, 0, 15, 100, 0, 30);
    std::pair<TString, TString> dvars = GetDalitzVars(std::string(mode_dir));
    sel_tree->Draw(dvars.first + ":" + dvars.second + " >> dalitz_hist",
            cut + "abs(helicity_angle) > 0.4");
    TCanvas * dal_canv = new TCanvas("dal_canv", "", 500, 500);
    dalitz_hist->GetXaxis()->SetTitle("#it{m}^{2}(K^{*0}#pi^{+})");
    dalitz_hist->GetYaxis()->SetTitle("#it{m}^{2}(#bar{D}^{0}K^{*0})");
    dalitz_hist->Draw("COLZ");
    dal_canv->SaveAs("../Plots/Components/" + mode_dir + "/dalitz.pdf");

    // Save tree
    file->cd();
    sel_tree->Write("SelectedTree");

    // Draw and save
    canvas->Clear();
    helicity_hist->SetLineWidth(1);
    helicity_hist->Draw();
    canvas->SaveAs("../Plots/Components/" + mode_dir + "/helicity_angle.pdf");


}
