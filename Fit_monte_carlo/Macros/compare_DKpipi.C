// ================================================================
// Script to compare DKpipi PDF from RapidSim with full Monte Carlo
// ================================================================
void compare_DKpipi() {

    // Get PDF histogram
    TFile * pdf_file = TFile::Open("../Histograms/DKpipi_RapidSim.root", "READ");
    TH1F * pdf = (TH1F*)pdf_file->Get("hist__Bd_M");
    pdf->SetLineColor(kRed);

    // Get MC mass histogram
    TFile * mc_file = TFile::Open("/data/lhcb/users/pullen/B02DKstar/MC/"
            "backgrounds/DKpipi/all_simulations_selected.root", "READ");
    TTree * mc_tree = (TTree*)mc_file->Get("DecayTree");
    TH1F * mc_hist = new TH1F("mc_hist", "", pdf->GetNbinsX() / 10, 5000, 5200);
    mc_tree->Draw("Bd_ConsD_M>>mc_hist");
    mc_hist->SetLineColor(kBlack);
    mc_hist->SetLineWidth(1);
    pdf->Scale(10 * mc_hist->Integral() / pdf->Integral());

    // Plot together
    TCanvas * canvas = new TCanvas("canvas", "", 900, 600);
    mc_hist->Draw("E");
    pdf->Draw("C SAME");
    mc_hist->Draw("E SAME");
    canvas->SaveAs("../Plots/DKpipi_pdf_vs_mc.pdf");

}
