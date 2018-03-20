// =======================================================================
// Script to plot the B+ -> DK*0pi mode with predicted kinematic endpoints
// =======================================================================
void PlotNonRes_withEndpoints() {

    TFile * file = TFile::Open("/data/lhcb/users/pullen/RapidSim/B_DKpipi_study/"
            "with_evtGen/2_Kstar_tree.root", "READ");
    TTree * tree = (TTree*)file->Get("DecayTree");

    TH1F * hist = new TH1F("hist", "", 500, 2600, 5300);
    TCut cut = "abs(Kstar_M - 0.8955) < 0.05 && abs(D0_M - 1.86484) < 0.025";
    tree->Draw("Bd_M*1000>>hist", cut);

    TCanvas * canvas = new TCanvas("canvas", "", 800, 400);
    hist->SetLineWidth(1);
    hist->Draw();
    TLine * line_low = new TLine(2760.65, 0, 2760.65, hist->GetMaximum());
    TLine * line_up = new TLine(5139.69, 0, 5139.69, hist->GetMaximum());
    line_low->SetLineStyle(2);
    line_low->SetLineColor(kRed);
    line_low->Draw();
    line_up->SetLineStyle(2);
    line_up->SetLineColor(kRed);
    line_up->Draw();
    canvas->SaveAs("../Plots/non_res_with_endpoints.pdf");

}
