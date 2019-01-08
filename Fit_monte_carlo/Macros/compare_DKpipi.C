// ================================================================
// Script to compare DKpipi PDF from RapidSim with full Monte Carlo
// ================================================================
void compare_DKpipi() {

    // Get PDF histogram
    TFile * pdf_file = TFile::Open("../Histograms/DKpipi_RapidSim.root", "READ");
    TH1F * pdf = (TH1F*)pdf_file->Get("fit");
    pdf->SetLineColor(kRed);

    // Get MC mass histogram
    TFile * mc_file = TFile::Open("/data/lhcb/users/pullen/B02DKstar/MC/"
            "backgrounds/DKpipi/all_simulations_selected.root", "READ");
    TTree * mc_tree = (TTree*)mc_file->Get("DecayTree");
    TH1F * mc_hist = new TH1F("mc_hist", "", pdf->GetNbinsX() / 10, 5000, 5200);
    mc_tree->Draw("Bd_ConsD_M[0]>>mc_hist", "Bd_ConsD_MD > 5000 && Bd_BKGCAT != 60");
    std::cout << "Tree entries: " << mc_tree->GetEntries() << std::endl;
    std::cout << "MC hist entries: " << mc_hist->Integral() << std::endl;

    // Fill histogram
    // double Kstar_PE, Kstar_PX, Kstar_PY, Kstar_PZ;
    // double D0_PE, D0_PX, D0_PY, D0_PZ;
    // int bkgcat;
    // mc_tree->SetBranchAddress("Kstar_TRUEP_E", &Kstar_PE);
    // mc_tree->SetBranchAddress("Kstar_TRUEP_X", &Kstar_PX);
    // mc_tree->SetBranchAddress("Kstar_TRUEP_Y", &Kstar_PY);
    // mc_tree->SetBranchAddress("Kstar_TRUEP_Z", &Kstar_PZ);
    // mc_tree->SetBranchAddress("D0_TRUEP_E", &D0_PE);
    // mc_tree->SetBranchAddress("D0_TRUEP_X", &D0_PX);
    // mc_tree->SetBranchAddress("D0_TRUEP_Y", &D0_PY);
    // mc_tree->SetBranchAddress("D0_TRUEP_Z", &D0_PZ);
    // mc_tree->SetBranchAddress("Bd_BKGCAT", &bkgcat);
    // for (int i = 0; i < mc_tree->GetEntries(); i++) {
        // mc_tree->GetEntry(i);
        // if (bkgcat != 40 && bkgcat != 50) continue;
        // TLorentzVector Kstar_P(Kstar_PX, Kstar_PY, Kstar_PZ, Kstar_PE);
        // TLorentzVector D0_P(D0_PX, D0_PY, D0_PZ, D0_PE);
        // double inv_mass = (D0_P + Kstar_P).M();
        // mc_hist->Fill(inv_mass);
    // }

    // MC histogram style
    mc_hist->SetLineColor(kBlack);
    mc_hist->SetLineWidth(1);
    pdf->Scale(10 * mc_hist->Integral() / pdf->Integral());

    // Set axis labels
    mc_hist->GetXaxis()->SetTitle("#it{m}([K#pi]_{D}K^{*0}) [MeV/#it{c}^{2}]");
    mc_hist->GetXaxis()->SetTitleOffset(1.1);
    int bin_width = mc_hist->GetBinWidth(1);
    mc_hist->GetYaxis()->SetTitle(("Candidates / (" + std::to_string(bin_width) + " MeV/#it{c}^{2})").c_str());

    // Make legend
    TLegend leg(0.55, 0.65, 0.85, 0.9);
    leg.AddEntry(mc_hist, "Full MC", "L");
    leg.AddEntry(pdf, "RapidSim PDF", "L");

    // Plot together
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    mc_hist->Draw("E");
    pdf->Draw("C SAME");
    mc_hist->Draw("E SAME");
    leg.Draw();
    canvas->SaveAs("../Plots/DKpipi_pdf_vs_mc.pdf");

}
